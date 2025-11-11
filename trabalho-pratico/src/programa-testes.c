#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include "programa-testes.h"
#include <sys/resource.h>


int compare_files(const char *generated, const char *expected) {
    FILE *fg = fopen(generated, "r");
    FILE *fe = fopen(expected, "r");
    if (!fg || !fe) {
        fprintf(stderr, " Erro ao abrir ficheiros: %s ou %s\n", generated, expected);
        if (fg) fclose(fg);
        if (fe) fclose(fe);
        return 0;
    }

    char line_g[4096], line_e[4096];
    int line_num = 1;
    int identical=1;

    while (fgets(line_g, sizeof(line_g), fg) && fgets(line_e, sizeof(line_e), fe)) {
        line_g[strcspn(line_g, "\r\n")] = 0;
        line_e[strcspn(line_e, "\r\n")] = 0;  

        if (strcmp(line_g, line_e) != 0) {
            printf("   Diferença na linha %d\n", line_num);
            printf("   Gerado:   %s", line_g);
            printf("   Esperado: %s\n", line_e);
            identical=0;
            break;
        }
        line_num++;
    }
    // Verifica se um ficheiro tem mais linhas que o outro
    int iguais = (feof(fg) && feof(fe));
    fclose(fg);
    fclose(fe);
    return iguais;
}


//funcao para imprimir o tempo total e memória usada
void print_performance_info(struct timespec start, struct timespec end, struct rusage usage) {
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("\n Tempo total de execução: %.6f segundos\n", elapsed);

#ifdef __APPLE__
    printf(" Medição de memória não suportada no macOS com getrusage()\n");
#else
    printf(" Memória máxima usada: %ld KB\n", usage.ru_maxrss);
#endif

}


// Função principal de execução e comparação
int run_programa_testes(const char *dataset_path, const char *commands_file, const char *expected_dir) {
    printf("A iniciar testes automáticos...\n");
    struct timespec start, end;
    struct rusage usage;
    clock_gettime(CLOCK_REALTIME, &start);

    // Executa o programa principal
    printf("A correr o programa principal...\n");
    char command[512];
    snprintf(command, sizeof(command), "./programa-principal %s %s", dataset_path, commands_file);

    struct timespec start_prog, end_prog;
    clock_gettime(CLOCK_REALTIME, &start_prog);
    int ret = system(command);
    clock_gettime(CLOCK_REALTIME, &end_prog);

    if (ret != 0) {
        fprintf(stderr, "Erro ao executar ./programa-principal (ret=%d)\n", ret);
    }

    double exec_time = (end_prog.tv_sec - start_prog.tv_sec) +
                       (end_prog.tv_nsec - start_prog.tv_nsec) / 1e9;
    printf("Tempo de execução do programa principal: %.6f s\n", exec_time);


    // Comparação dos outputs
    int correct = 0, total = 0;
    double query_times[10] = {0};
    int query_counts[10] = {0};
    char generated[256], expected[256];

    for (int i = 1; i <= 10; i++) {  // assume até 10 queries para teste
        snprintf(generated, sizeof(generated), "resultados/command%d_output.txt", i);
        snprintf(expected, sizeof(expected), "%s/command%d_output.txt", expected_dir, i);
        FILE *test = fopen(expected, "r");
        if (!test) {
            printf("⚠️  Ficheiro esperado não encontrado: %s\n", expected);
            continue;
        }
        fclose(test);

        total++;
        printf("\n A comparar %s...\n", generated);

        struct timespec start_q, end_q;
        clock_gettime(CLOCK_REALTIME, &start_q);

        int ok = compare_files(generated, expected);

        clock_gettime(CLOCK_REALTIME, &end_q);
        double elapsed_q = (end_q.tv_sec - start_q.tv_sec) +
                           (end_q.tv_nsec - start_q.tv_nsec) / 1e9;
        query_times[i - 1] += elapsed_q;
        query_counts[i - 1]++;

        if (ok) {
            printf(" command%d_output.txt está correto.\n", i);
            correct++;
        } else {
            printf(" command%d_output.txt difere do esperado.\n", i);
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    getrusage(RUSAGE_SELF, &usage);

    // Resultados globais
    printf("\n Resultados dos testes:\n");
    printf("   %d de %d ficheiros corretos (%.1f%%)\n",
           correct, total, total > 0 ? (100.0 * correct / total) : 0.0);

    printf("\n Tempo médio por query:\n");
    for (int i = 0; i < total; i++) {
        if (query_counts[i] > 0)
            printf("   Query %d: %.6fs\n", i + 1, query_times[i] / query_counts[i]);
    }

    print_performance_info(start, end, usage);
    return (correct == total) ? 0 : 1;
}

int main(int argc, char *argv[]) {
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "Uso: %s <dataset_path> <ficheiro_comandos> <resultados_esperados>\n", argv[0]);
        return 1;
    }
    return run_programa_testes(argv[1], argv[2], argv[3]);
}



