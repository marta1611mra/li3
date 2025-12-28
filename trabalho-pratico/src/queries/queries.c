#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
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
// Função auxiliar para separar argumentos respeitando aspas
void parse_query_line(char *line, char *cmd, char *arg1, char *arg2) {
    cmd[0] = '\0'; arg1[0] = '\0'; arg2[0] = '\0';
    
    char *p = line;
    
    // 1. Ler Comando
    while (*p && isspace((unsigned char)*p)) p++;
    int i = 0;
    while (*p && !isspace((unsigned char)*p)) {
        cmd[i++] = *p++;
    }
    cmd[i] = '\0';

    // 2. Ler Arg1
    while (*p && isspace((unsigned char)*p)) p++;
    if (*p) {
        i = 0;
        if (*p == '"') { // Se tiver aspas, lê até à próxima aspa
            p++; 
            while (*p && *p != '"') arg1[i++] = *p++;
            if (*p == '"') p++;
        } else { // Senão, lê até ao espaço
            while (*p && !isspace((unsigned char)*p)) arg1[i++] = *p++;
        }
        arg1[i] = '\0';
    }

    // 3. Ler Arg2
    while (*p && isspace((unsigned char)*p)) p++;
    if (*p) {
        i = 0;
        if (*p == '"') {
            p++;
            while (*p && *p != '"') arg2[i++] = *p++;
            if (*p == '"') p++;
        } else {
            while (*p && !isspace((unsigned char)*p) && *p != '\n') arg2[i++] = *p++;
        }
        arg2[i] = '\0';
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
        parse_query_line(line, cmd, arg1, arg2);

        int qid = atoi(cmd);
        char sep = strchr(cmd, 'S') ? '=' : ';';
        set_output_separator(sep);

        switch (qid) {
            case 1:{
                q1(dataset_get_airports(d), arg1, out);
                break;
            }

            case 2: {
                q2(d, atoi(arg1), (strlen(arg2) > 0 ? arg2 : NULL), out); 
                break;
            }
            case 3:{
                /** Query 3: 3 <start_date> <end_date> */
                if (strlen(arg1) > 0 && strlen(arg2) > 0)
                    q3(d, (char *[]){arg1, arg2}, out);
                break;
            }
            case 4:{
                query4_execute(d, (strlen(arg1)>0 ? arg1 : NULL), (strlen(arg2)>0 ? arg2 : NULL), out); 
                break;
            }   
            case 5: {
                query5(d, atoi(arg1), out); 
                break;
            }
            case 6: {
                q6(d,arg1,out);
                break;
            }
            default:
                //  Query desconhecida 
                fprintf(stderr, "Query %d não implementada.\n", qid);
                fprintf(out, "\n");
                break;

        }

        fclose(out);
        command_number++;
    }

    fclose(queries_file);
    printf("Todas as queries foram processadas.\n");
}

