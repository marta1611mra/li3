/**
 * @file programa-testes.c
 * @brief Programa para execução automática de testes comparando ficheiros gerados com resultados esperados.
 *
 * Este programa lê os ficheiros gerados por um conjunto de queries e compara com
 * os ficheiros esperados, reportando diferenças e tempos de execução.
 */

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

#define MAX_QUERIES 10

/**
 * @brief Compara dois ficheiros linha a linha.
 *
 * Esta função abre dois ficheiros de texto, lê cada linha e compara-as.
 * Se houver diferença, retorna imediatamente e fornece a linha da primeira discrepância.
 *
 * @param generated Caminho para o ficheiro gerado pelo programa.
 * @param expected Caminho para o ficheiro esperado.
 * @param diff_line Ponteiro para armazenar a linha da primeira diferença (se houver).
 * @return int Retorna 1 se os ficheiros forem idênticos, 0 caso contrário.
 *
 * @note Remove automaticamente os caracteres de nova linha (\n ou \r\n) antes da comparação.
 */
int compare_files(const char *generated, const char *expected, int *diff_line) {
    FILE *fg = fopen(generated, "r");
    FILE *fe = fopen(expected, "r");
    if (!fg || !fe) {
        fprintf(stderr, "Erro ao abrir ficheiros: %s ou %s\n", generated, expected);
        if (fg) fclose(fg);
        if (fe) fclose(fe);
        return 0;
    }

    char line_g[4096], line_e[4096];
    int line_num = 1;
    int identical = 1;

    while (fgets(line_g, sizeof(line_g), fg) && fgets(line_e, sizeof(line_e), fe)) {
        line_g[strcspn(line_g, "\r\n")] = 0;
        line_e[strcspn(line_e, "\r\n")] = 0;  

        if (strcmp(line_g, line_e) != 0) {
            printf("Diferença na linha %d\n", line_num);
            printf("Gerado:   %s", line_g);
            printf("Esperado: %s\n", line_e);
            identical=0;
            break;
        }
        line_num++;
    }

    if (identical) {
        if ((fgets(line_g, sizeof(line_g), fg) != NULL) ||
            (fgets(line_e, sizeof(line_e), fe) != NULL)) {
            *diff_line = line_num;
            identical = 0;
        }
    }

    fclose(fg);
    fclose(fe);
    return identical;
}

/**
 * @brief Imprime informações de desempenho do programa.
 *
 * Mostra o tempo total de execução e a memória máxima utilizada.
 *
 * @param start Timestamp de início da execução.
 * @param end Timestamp de fim da execução.
 * @param usage Estrutura contendo informações de recursos usados.
 *
 * @note A memória máxima usada não é suportada no macOS.
 */
void print_performance_info(struct timespec start, struct timespec end, struct rusage usage) {
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("\nTempo total de execução: %.6f segundos\n", elapsed);

#ifdef __APPLE__
    printf("Memória máxima usada: não suportado no macOS\n");
#else
    printf("Memória máxima usada: %ld KB\n", usage.ru_maxrss);
#endif
}

/**
 * @brief Executa testes automáticos comparando ficheiros gerados com resultados esperados.
 *
 * Lê cada ficheiro gerado pelo programa, compara com o correspondente ficheiro esperado,
 * calcula o tempo de execução de cada query e imprime um resumo dos resultados.
 *
 * @param dataset_path Caminho para o dataset (não utilizado nesta função, reservado para extensão futura).
 * @param commands_file Caminho para ficheiro de comandos (não utilizado nesta função, reservado para extensão futura).
 * @param expected_dir Diretório que contém os ficheiros esperados.
 * @return int Retorna 0 se todos os ficheiros estiverem corretos, 1 caso contrário.
 */
int run_programa_testes(const char *dataset_path, const char *commands_file, const char *expected_dir) {
    (void)dataset_path; 
    (void)commands_file; 

    printf("A iniciar testes automáticos...\n\n");

    struct timespec start_total, end_total;
    struct rusage usage;
    clock_gettime(CLOCK_REALTIME, &start_total);

    int total_queries = 0;
    int total_correct = 0;

    double times[MAX_QUERIES];
    int correct_flags[MAX_QUERIES];

    for (int query_index = 1; query_index <= MAX_QUERIES; query_index++) {
        char generated[256], expected[256];
        snprintf(generated, sizeof(generated), "resultados/command%d_output.txt", query_index);
        snprintf(expected, sizeof(expected), "%s/command%d_output.txt", expected_dir, query_index);

        printf("🔍 A comparar %s...\n", generated);

         // Verifica se o ficheiro esperado existe
        FILE *test = fopen(expected, "r");
        if (!test) {
            printf("Ficheiro esperado não encontrado: %s\n\n", expected);
            continue;
        }
        fclose(test);

        total_queries++;
        int diff_line = 0;
        struct timespec t_start, t_end;
        clock_gettime(CLOCK_REALTIME, &t_start);

        int ok = compare_files(generated, expected, &diff_line);

        clock_gettime(CLOCK_REALTIME, &t_end);
        double elapsed_query = (t_end.tv_sec - t_start.tv_sec) +
                               (t_end.tv_nsec - t_start.tv_nsec) / 1e9;

        times[total_queries - 1] = elapsed_query;
        correct_flags[total_queries - 1] = ok;

        if (ok) {
            printf("command%d_output.txt está correto.\n\n", query_index);
            printf("Query %d correta. Tempo: %.6f s\n", query_index, elapsed_query);
            total_correct++;
        } else {
            printf("command%d_output.txt difere do esperado. Primeira diferença na linha %d.\n\n", query_index, diff_line);
            printf("Query %d incorreta. Tempo: %.6f s\n", query_index, elapsed_query);
        }
    }

    clock_gettime(CLOCK_REALTIME, &end_total);
    getrusage(RUSAGE_SELF, &usage);

    printf("\n--- Resultados dos testes ---\n");
    for (int i = 0; i < total_queries; i++) {
        printf("Query %d: %s, tempo = %.6f s\n",
               i + 1,
               correct_flags[i] ? "correta" : "incorreta",
               times[i]);
    }

    printf("\nResultados dos testes:\n");
    printf("   %d de %d ficheiros corretos (%.1f%%)\n",
           total_correct, total_queries,
           total_queries > 0 ? 100.0 * total_correct / total_queries : 0.0);

    print_performance_info(start_total, end_total, usage);

    return (total_correct == total_queries) ? 0 : 1;
}

/**
 * @brief Função principal do programa.
 *
 * Espera três argumentos: caminho para o dataset, ficheiro de comandos e diretório de resultados esperados.
 *
 * @param argc Número de argumentos.
 * @param argv Array de argumentos.
 * @return int 0 se os testes forem executados com sucesso, 1 caso contrário.
 *
 * @usage ./programa <dataset_path> <ficheiro_comandos> <resultados_esperados>
 */
int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <dataset_path> <ficheiro_comandos> <resultados_esperados>\n", argv[0]);
        return 1;
    }

    return run_programa_testes(argv[1], argv[2], argv[3]);
}
