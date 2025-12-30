#ifndef OUTPUT_FORMAT_H
#define OUTPUT_FORMAT_H
#include <stdio.h>

/**
 * @brief Define o separador a usar no output das queries.
 *
 * Permite alternar entre diferentes formatos de output.
 * O separador é usado para delimitar campos nos resultados das queries.
 *
 * @param sep Caractere separador a usar.
 */
void set_output_separator(char sep);

/**
 * @brief Obtém o separador atual de output.
 *
 * Retorna o caractere separador atualmente configurado para delimitar campos nos resultados das queries.
 *
 * @return Caractere separador em uso.
 */
char get_output_separator(void);

/**
 * @brief Imprime uma linha vazia no ficheiro de output.
 *
 * Função auxiliar usada pelas queries para indicar ausência de resultados, escrevendo apenas \n no ficheiro de output.
 *
 * @param output Ficheiro onde será escrita a linha vazia.
 */
void output_empty(FILE *output);

#endif
