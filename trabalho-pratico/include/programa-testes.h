#ifndef PROGRAMA_TESTES_H
#define PROGRAMA_TESTES_H

#include <time.h>
#include <sys/resource.h>

/**
 * @brief Compara os ficheiros de saída gerados com os esperados.
 *
 * Lê os ficheiros da pasta "resultados/" e compara-os com os da pasta
 * "resultados-esperados/", linha a linha. Indica as diferenças e o número
 * de acertos por tipo de query.
 *
 * @param dataset_path Caminho para os CSVs de entrada (para consistência).
 * @param commands_file Caminho para o ficheiro com os comandos (input.txt).
 * @param expected_dir Caminho para a pasta com os outputs esperados.
 * @return int Retorna 0 se todas as comparações foram bem-sucedidas, ou 1 se houve diferenças.
 */
int run_programa_testes(const char *dataset_path, const char *commands_file, const char *expected_dir);

/**
 * @brief Compara dois ficheiros linha a linha.
 *
 * @param generated Caminho para o ficheiro gerado.
 * @param expected Caminho para o ficheiro esperado.
 * @return int 1 se os ficheiros forem iguais, 0 caso contrário.
 */
int compare_files(const char *generated, const char *expected,int *diff_line);

/**
 * @brief Mede e imprime o tempo e memória usados.
 *
 * @param start Tempo inicial (struct timespec)
 * @param end Tempo final (struct timespec)
 * @param usage Estrutura rusage com estatísticas de memória.
 */
void print_performance_info(struct timespec start, struct timespec end, struct rusage usage);

#endif
