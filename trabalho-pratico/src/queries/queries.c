#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataset.h"
#include "queries/query1.h"
#include "queries/query2.h"
#include "queries/query3.h"

/**
 * Abre um ficheiro de forma segura, verificando erros.
 *
 * Caso o ficheiro não consiga ser aberto, imprime uma mensagem de erro
 * no stderr e retorna NULL.
 *
 * @param path Caminho do ficheiro a abrir.
 * @param mode Modo de abertura (ex.: "r", "w").
 * @return Ponteiro para FILE caso sucesso, ou NULL caso ocorra erro.
 */

static FILE *safe_fopen(const char *path, const char *mode) {
    FILE *f = fopen(path, mode);
    if (!f) {
        fprintf(stderr, " Erro ao abrir ficheiro: %s\n", path);
    }
    return f;
}

/**
 * Lê e processa todas as queries fornecidas num ficheiro de texto.
 *
 * Para cada linha do ficheiro:
 *  - Identifica o número da query (1, 2, 3, ...)
 *  - Interpreta parâmetros adicionais (se existirem)
 *  - Executa a query correspondente
 *  - Gera um ficheiro "commandX_output.txt" com o resultado
 *
 * Formatos das queries suportadas:
 *
 *   Query 1: 1 <airport_code>
 *   Query 2: 2 <N> [manufacturer]
 *   Query 3: 3 <start_date> <end_date>
 *
 * Se uma query estiver mal formatada ou for desconhecida,
 * é reportada no stderr.
 *
 * @param d Dataset carregado contendo aeroportos, voos, aeronaves, etc.
 * @param queries_path Caminho para o ficheiro que contém as queries.
 */

void process_queries(Dataset d, const char *queries_path) {
    FILE *queries_file = safe_fopen(queries_path, "r");
    if (!queries_file) return;

    printf(" A executar queries de '%s'...\n", queries_path);

    int command_number = 1;
    char line[256];

    while (fgets(line, sizeof(line), queries_file)) {
        if (strlen(line) == 0 || line[0] == '\n') continue;

        /** 
         * Cria o nome do ficheiro de saída da query corrente.
         * Exemplo: command1_output.txt
         */

        char output_path[128];
        snprintf(output_path, sizeof(output_path),
                 "command%d_output.txt", command_number);

        FILE *out = safe_fopen(output_path, "w");
        if (!out) {
            fprintf(stderr, "Erro ao criar o ficheiro de output da query %d\n", command_number);
            continue;
        }

         /**
         * Interpreta a linha da query:
         * qid = número da query
         * arg1, arg2 = parâmetros opcionais
         */
        int qid;
        char arg1[128] = "", arg2[128] = "";
        int n = sscanf(line, "%d %127s %127s", &qid, arg1, arg2);

        switch (qid) {
             /**
             * Query 1
             * Formato: 1 <airport_code>
             * Obtém estatísticas de um aeroporto específico.
             */
            case 1:
                query1(dataset_get_airports(d), arg1, out);
                break;
             /**
             * Query 2
             * Formato: 2 <N> [manufacturer]
             * Lista o top N de aeronaves por número de voos,
             * opcionalmente filtradas por fabricante.
             */
            case 2: {
                int N = atoi(arg1);
                const char *filter = (n == 3) ? arg2 : NULL;
                query2_topN_aircrafts(dataset_get_flights(d),
                                      dataset_get_aircrafts(d),
                                      N, filter, out);
                break;
            }
            /**
             * Query 3
             * Formato: 3 <start_date> <end_date>
             * Lista voos ocorridos num intervalo de datas.
             */
            case 3:
                if (n == 3)
                    q3(arg1, arg2, dataset_get_flights(d),
                       dataset_get_airports(d), out);
                else
                    fprintf(stderr, "Query 3 mal formatada na linha %d\n", command_number);
                break;
            /**
            * Query não reconhecida
            */

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
