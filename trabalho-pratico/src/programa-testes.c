#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include "programa-testes.h"
#include <sys/resource.h>

#define MAX_QUERIES 120


// Compara dois ficheiros linha a linha.
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


// Imprime informações de desempenho do programa.
void print_performance_info(struct timespec start, struct timespec end, struct rusage usage) {
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("\nTempo total de execução: %.6f segundos\n", elapsed);

#ifdef __APPLE__
    printf("Memória máxima usada: não suportado no macOS\n");
#else
    printf("Memória máxima usada: %ld KB\n", usage.ru_maxrss);
#endif
}


// Executa testes automáticos comparando ficheiros gerados com resultados esperados.
int run_programa_testes(const char *dataset_path, const char *commands_file, const char *expected_dir) {
    (void)dataset_path; 
    (void)commands_file; 

    printf("A iniciar testes automáticos...\n\n");

    struct timespec start_total, end_total;
    struct rusage usage;
    clock_gettime(CLOCK_REALTIME, &start_total);

    int total_cmds = 0;
    int total_correct = 0;

    double times[MAX_QUERIES] = {0};
    int correct[MAX_QUERIES] = {0};
    double sum_q[4] = {0}; 
    int count_q[4] = {0};

    for (int cmd = 1; cmd <= MAX_QUERIES; cmd++) {
        char generated[256], expected[256];
        snprintf(generated, sizeof(generated), "resultados/command%d_output.txt", cmd);
        snprintf(expected, sizeof(expected), "%s/command%d_output.txt", expected_dir, cmd);

        printf("A comparar %s...\n", generated);

         // Verifica se o ficheiro esperado existe
        FILE *test = fopen(expected, "r");
        if (!test) {
            printf("Ficheiro esperado não encontrado: %s\n\n", expected);
            continue;
        }
        fclose(test);

        total_cmds++;
        int diff_line = 0;
        struct timespec t_start, t_end;
        clock_gettime(CLOCK_REALTIME, &t_start);

        int ok = compare_files(generated, expected, &diff_line);

        clock_gettime(CLOCK_REALTIME, &t_end);
        double elapsed = (t_end.tv_sec - t_start.tv_sec) +
                               (t_end.tv_nsec - t_start.tv_nsec) / 1e9;

        times[total_cmds - 1] = elapsed;
        correct[total_cmds - 1] = ok;

        if (ok) {
            printf("command%d_output.txt está correto.\n\n", cmd);
            printf("Comando %d correta. Tempo: %.6f s\n", cmd, elapsed);
            total_correct++;
        } else {
            printf("command%d_output.txt difere do esperado. Primeira diferença na linha %d.\n\n", cmd, diff_line);
            printf("Comando %d incorreta. Tempo: %.6f s\n", cmd, elapsed);
        }

        FILE *fcmd = fopen(commands_file, "r");
        if (fcmd) {
            char line[256];
            int qtype = 0;
            int curr = 1;

            while (fgets(line, sizeof(line), fcmd)) {
                if (curr == cmd) {
                    sscanf(line, "%d", &qtype);
                    break;
                }
                curr++;
            }
            fclose(fcmd);

            if (qtype >= 1 && qtype <= 3) {
                sum_q[qtype] += elapsed;
                count_q[qtype]++;
            }
        }
    }
    
    clock_gettime(CLOCK_REALTIME, &end_total);
    getrusage(RUSAGE_SELF, &usage);

    printf("\n--- Tempo por comando ---\n");
    for (int i = 0; i < total_cmds; i++) {
        printf("Comando %d: %s, tempo = %.6f s\n",
            i + 1,
            correct[i] ? "correto" : "incorreto",
            times[i]);
    }

    printf("\n--- Resultados gerais ---\n");
    printf("%d de %d comandos corretos (%.1f%%)\n",
        total_correct,
        total_cmds,
        total_cmds ? (100.0 * total_correct / total_cmds) : 0.0);


    for (int q = 1; q <= 4; q++) {
        if (count_q[q] > 0)
            printf("Query %d: média = %.6f s  (%d comandos)\n",
                   q, sum_q[q] / count_q[q], count_q[q]);
        else
            printf("Query %d: (nenhum comando encontrado)\n", q);
    }

    print_performance_info(start_total, end_total, usage);

    return (total_correct == total_cmds) ? 0 : 1;
}


// Função principal do programa.
int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <dataset_path> <ficheiro_comandos> <resultados_esperados>\n", argv[0]);
        return 1;
    }

    return run_programa_testes(argv[1], argv[2], argv[3]);
}
