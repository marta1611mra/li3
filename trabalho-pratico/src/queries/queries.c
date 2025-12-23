#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "dataset.h"
#include "queries/query1.h"
#include "queries/query2.h"
#include "queries/query3.h"

typedef enum {
    format_def,   // ;
    format_alt        // =
} OutputFormat;

// Impressão genérica 
static void print_fields(FILE *out, OutputFormat fmt,
                         const char **fields, int n) {
    char sep = (fmt == format_alt) ? '=' : ';';

    for (int i = 0; i < n; i++) {
        fprintf(out, "%s", fields[i]);
        if (i < n - 1)
            fputc(sep, out);
    }
    fputc('\n', out);
}

// Abre um ficheiro de forma segura.
static FILE *safe_fopen(const char *path, const char *mode) {
    FILE *f = fopen(path, mode);
    if (!f) {
        fprintf(stderr, "Erro ao abrir ficheiro: %s\n", path);
    }
    return f;
}

// Processa todas as queries listadas num ficheiro de input.

void process_queries(Dataset d, const char *queries_path) {
    FILE *queries_file = safe_fopen(queries_path, "r");
    if (!queries_file) return;

    printf("A executar queries de '%s'...\n", queries_path);

    int command_number = 1;
    char line[256];

    while (fgets(line, sizeof(line), queries_file)) {
        if (strlen(line) == 0 || line[0] == '\n') continue;

        char cmd[16], arg1[128] = "", arg2[128] = "";
        int n = sscanf(line, "%15s %127s %127s", cmd, arg1, arg2);

        int qid = atoi(cmd);
        OutputFormat fmt = strchr(cmd, 'S') ? format_alt : format_def ;

        char output_path[128];
        snprintf(output_path, sizeof(output_path),
                 "resultados/command%d_output.txt", command_number);

        FILE *out = safe_fopen(output_path, "w");
        if (!out) {
            fprintf(stderr, "Erro ao criar o ficheiro de output da query %d\n", command_number);
            continue;
        }

        switch (qid) {
            case 1:{
                  Q1Result r = q1(dataset_get_airports(d), arg1);
                if (!r.found) {
                    fprintf(out, "\n");
                    break;
                }
                const char *fields[] = {
                    r.code, r.name, r.city, r.country, r.type
                };
                print_fields(out, fmt, fields, 5);
                break;
            }

            case 2: {
                int N = atoi(arg1);
                const char *filter = (n == 3) ? arg2 : NULL;
                Q2Result res = q2(dataset_get_flights(d),
                                      dataset_get_aircrafts(d),
                                      N, filter);
                    if (res.used == 0) {
                    fprintf(out, "\n");
                    free_q2_result(&res);
                    break;
                    }

                    for (int i = 0; i < res.used; i++) {
                    const char *fields[] = {
                    res.rows[i].id,
                    res.rows[i].manufacturer,
                    res.rows[i].model,
                    res.rows[i].count_str
                    };
                    print_fields(out, fmt, fields, 4);
                }
                free_q2_result(&res);
                break;
            }

            case 3:{
                /** Query 3: 3 <start_date> <end_date> */
                Q3Result r3 = q3(arg1, arg2,
                                    dataset_get_flights(d),
                                    dataset_get_airports(d));

                if (!r3.found) {
                    fprintf(out, "\n");
                    break;
                }

                const char *fields[] = {
                    r3.code, r3.name, r3.city, r3.country, r3.count_str
                };
                print_fields(out, fmt, fields, 5);
                break;
        }
        default:
        fprintf(out, "\n");
        }

        fclose(out);
        command_number++;
    }

    fclose(queries_file);
    printf("Todas as queries foram processadas.\n");
}

