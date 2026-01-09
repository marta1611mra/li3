#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "output_format.h"
#include "dataset.h"
#include"queries/queries.h"
#include "queries/query1.h"
#include "queries/query2.h"
#include "queries/query3.h"
#include "queries/query4.h"
#include "queries/query5.h"
#include "queries/query6.h"

// Abre um ficheiro de forma segura
static FILE *safe_fopen(const char *path, const char *mode) {
    FILE *f = fopen(path, mode);
    if (!f) {
        fprintf(stderr, "Erro ao abrir ficheiro: %s\n", path);
    }
    return f;
}

// Remove o \n e espaços brancos do final da string
static void trim_end(char *str) {
    if (!str) return;
    size_t len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[--len] = '\0';
    }
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

        // Limpa espaços no fim da linha (\n, \r, etc)
        trim_end(line);

        char output_path[128];
        snprintf(output_path, sizeof(output_path),
                 "resultados/command%d_output.txt", command_number);

        FILE *out = safe_fopen(output_path, "w");
        if (!out) {
            command_number++;
            continue;
        }

        char cmd[32] = {0};
        char *args = NULL;

        // Encontra o primeiro espaço
        char *space_ptr = strchr(line, ' ');

        if (space_ptr) {
            // Copia o comando até ao espaço
            size_t cmd_len = space_ptr - line;
            if (cmd_len > 31) cmd_len = 31;
            strncpy(cmd, line, cmd_len);
            cmd[cmd_len] = '\0';

            // O argumento é tudo o que está depois do espaço
            args = space_ptr + 1; 
            
            // Ignora espaços extra entre o comando e o argumento
            while (*args && isspace((unsigned char)*args)) args++;
        } else {
            // Se não houver espaço, a linha toda é o comando (ex: "5")
            strcpy(cmd, line);
            args = "";
        }

        int qid = atoi(cmd);
        char sep = strchr(cmd, 'S') ? '=' : ';';
        set_output_separator(sep);

        switch (qid) {
            case 1:{
                q1(dataset_get_airports(d), args, out);
                break;
            }

            case 2: {
                // Tenta ler N e Filtro. Se ler 2 coisas, usa o filtro.
                int N = 0;
                char filter[128] = "";
                if (sscanf(args, "%d %s", &N, filter) == 2) {
                    q2(d, N, filter, out);
                } else {
                    sscanf(args, "%d", &N);
                    q2(d, N, NULL, out);
                }
                break;
            }
            
            case 3:{
                char d1[64] = "";
                char d2[64] = "";
                if (sscanf(args, "%s %s", d1, d2) == 2) {
                    char *dates[2] = {d1, d2};
                    q3(d, dates, out);
                } else {
                    output_empty(out);
                }
                break;
            }
            
            case 4:{
                char begin_date[64] = "";
                char end_date[64] = "";
                int n = sscanf(args, "%s %s", begin_date, end_date);
                if (n == 2) {
                    // Ambas as datas fornecidas
                    query4_execute(d, begin_date, end_date, out);
                } else if (n == 0 || strlen(args) == 0) {
                    // Nenhuma data fornecida
                    query4_execute(d, NULL, NULL, out);
                } else {
                    // Apenas uma data fornecida (inválido)
                    output_empty(out);
                }
                break;
            }
            
            case 5: {
                int N = atoi(args);
                if (N > 0) {
                    query5(d, N, out);
                } else {
                    output_empty(out);
                }
                break;
            }
            
            case 6: {
                if (args && strlen(args) > 0) {
                    q6(d, args, out);
                } else {
                    output_empty(out);
                }
                break;
            }
            
            default:
                // Query desconhecida 
                fprintf(stderr, "Query %d não implementada.\n", qid);
                output_empty(out);
                break;
        }

        fclose(out);
        command_number++;
    }

    fclose(queries_file);
    printf("Todas as queries foram processadas.\n");
}