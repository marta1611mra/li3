#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataset.h"
#include "queries/query1.h"
#include "queries/query2.h"
#include "queries/query3.h"

// Função auxiliar para abrir ficheiros com verificação de erros
static FILE *safe_fopen(const char *path, const char *mode) {
    FILE *f = fopen(path, mode);
    if (!f) {
        fprintf(stderr, "❌ Erro ao abrir ficheiro: %s\n", path);
    }
    return f;
}

// Função principal que processa todas as queries do ficheiro input.txt
void process_queries(Dataset d, const char *queries_path) {
    FILE *queries_file = safe_fopen(queries_path, "r");
    if (!queries_file) return;

    printf("⚙️  A executar queries de '%s'...\n", queries_path);

    int command_number = 1;
    char line[256];

    while (fgets(line, sizeof(line), queries_file)) {
        if (strlen(line) == 0 || line[0] == '\n') continue;

        // Cria o nome do ficheiro de saída
        char output_path[128];
        snprintf(output_path, sizeof(output_path),
                 "resultados/command%d_output.txt", command_number);

        FILE *out = safe_fopen(output_path, "w");
        if (!out) {
            fprintf(stderr, "Erro ao criar o ficheiro de output da query %d\n", command_number);
            continue;
        }

        // Identifica a query e argumentos
        int qid;
        char arg1[128] = "", arg2[128] = "";
        int n = sscanf(line, "%d %127s %127s", &qid, arg1, arg2);

        //limpeza de argumentos (remove \r e \n) ---
        arg1[strcspn(arg1, "\r\n")] = '\0';
        arg2[strcspn(arg2, "\r\n")] = '\0';


        switch (qid) {
            case 1:
                // Query 1: 1 <airport_code>
                query1(dataset_get_airports(d), arg1, out);
                break;

            case 2: {
                // Query 2: 2 <N> [manufacturer]
                int N = atoi(arg1);
                const char *filter = (n == 3) ? arg2 : NULL;
                query2_topN_aircrafts(dataset_get_flights(d),
                                      dataset_get_aircrafts(d),
                                      N, filter, out);
                break;
            }

            case 3:
                // Query 3: 3 <start_date> <end_date>
                if (n == 3)
                    q3(arg1, arg2, dataset_get_flights(d),
                       dataset_get_airports(d), out);
                else
                    fprintf(stderr, "Query 3 mal formatada na linha %d\n", command_number);
                break;

            default:
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
