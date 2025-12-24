#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "output_format.h"
#include "dataset.h"
#include "queries/query1.h"
#include "queries/query2.h"
#include "queries/query3.h"
#include "queries/query4.h"
#include "queries/query5.h"
#include "queries/query6.h"

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

        char output_path[128];
        snprintf(output_path, sizeof(output_path),
                 "resultados/command%d_output.txt", command_number);

        FILE *out = safe_fopen(output_path, "w");
        if (!out) {
            fprintf(stderr, "Erro ao criar o ficheiro de output da query %d\n", command_number);
            continue;
        }

        /* parsing do comando */
        char cmd[16], arg1[128] = "", arg2[128] = "";
        int n = sscanf(line, "%15s %127s %127s", cmd, arg1, arg2);

        int qid = atoi(cmd);
        char sep = strchr(cmd, 'S') ? '=' : ';';
        set_output_separator(sep);

        switch (qid) {
            case 1:{
                q1(dataset_get_airports(d), arg1, out);
                break;
            }

            case 2: {
                int N = atoi(arg1);
                const char *filter = (n == 3) ? arg2 : NULL;
                q2(dataset_get_flights(d),
                       dataset_get_aircrafts(d),
                       N, filter, out);
                break;
            }

            case 3:{
                /** Query 3: 3 <start_date> <end_date> */
                if (n == 3)
                    q3(arg1, arg2,
                       dataset_get_flights(d),
                       dataset_get_airports(d),
                       out);
                else
                    fprintf(stderr, "Query 3 mal formatada na linha %d\n", command_number);
                break;
            }
            case 4:
                /** Query 4: 4 [start_date end_date] */
               
            case 5: {
                int N = atoi(arg1);
                query5(dataset_get_flights(d), N, out);
                break;
            }
            default:
                //  Query desconhecida 
                fprintf(stderr, "Query %d não implementada.\n", qid);
                fprintf(out, "\n");
                break;
            case 6: {
                q6(dataset_get_passengers(d),dataset_get_flights(d),dataset_get_reservations(d),arg1,out);
                break;
            }
        }

        fclose(out);
        command_number++;
    }

    fclose(queries_file);
    printf("Todas as queries foram processadas.\n");
}

