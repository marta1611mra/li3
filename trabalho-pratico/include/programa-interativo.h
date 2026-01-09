#ifndef PROGRAMA_INTERATIVO_NCURSES_H
#define PROGRAMA_INTERATIVO_NCURSES_H

#include "dataset.h"
#include <ncurses.h>
#include <stdbool.h>

#define QUERY_INPUT_SIZE 256

/**
 * @brief Estrutura para armazenar inputs das queries
 */
typedef struct {
    char code[QUERY_INPUT_SIZE];           // Query 1: código do aeroporto
    int n;                                 // Query 2, 5: número N
    char manufacturer[QUERY_INPUT_SIZE];   // Query 2: fabricante
    char start_date[QUERY_INPUT_SIZE];     // Query 3: data início
    char end_date[QUERY_INPUT_SIZE];       // Query 3, 4: data fim
    char begin_date[QUERY_INPUT_SIZE];     // Query 4: data início
    char nationality[QUERY_INPUT_SIZE];    // Query 6: nacionalidade
} QueryInputs;

/**
 * @brief Desenha o cabeçalho da interface
 * @param win Janela ncurses onde desenhar
 * @return void
 */
void draw_header(WINDOW *win);

/**
 * @brief Desenha o menu com as queries disponíveis
 * @param win Janela ncurses onde desenhar
 * @param selected Índice da query selecionada (0-5)
 * @return void
 */
void draw_menu(WINDOW *win, int selected);

/**
 * @brief Desenha a opção de alternar separador
 * @param win Janela ncurses onde desenhar
 * @param y Posição Y onde desenhar
 * @param current_sep Separador atual
 * @return void
 */
void draw_separator_option(WINDOW *win, int y, char current_sep);

/**
 * @brief Obtém input do usuário
 * @param win Janela ncurses
 * @param y Posição Y onde mostrar o input
 * @param label Label a mostrar
 * @param buffer Buffer onde guardar o input
 * @param max_len Tamanho máximo do input
 * @return void
 */
void get_input(WINDOW *win, int y, const char *label, char *buffer, int max_len);

/**
 * @brief Mostra os resultados de uma query
 * @param win Janela ncurses onde mostrar
 * @param result String com os resultados
 * @return void
 */
void show_results(WINDOW *win, const char *result);

/**
 * @brief Verifica se um diretório existe
 * @param path Caminho do diretório
 * @return true se existe, false caso contrário
 */
bool directory_exists(const char *path);

/**
 * @brief Valida se uma string contém apenas dígitos
 * @param str String a validar
 * @return true se é numérico, false caso contrário
 */
bool is_numeric(const char *str);

/**
 * @brief Valida formato de data YYYY-MM-DD
 * @param date String da data a validar
 * @return true se formato válido, false caso contrário
 */
bool is_valid_date_format(const char *date);

/**
 * @brief Executa uma query e mostra os resultados
 * @param win Janela ncurses
 * @param dataset Dataset carregado
 * @param query_num Número da query (0-5)
 * @param inputs Estrutura com os inputs
 * @param separator Separador a usar no output
 * @return void
 */
void execute_query(WINDOW *win, Dataset dataset, int query_num, QueryInputs *inputs, char separator);

/**
 * @brief Desenha o rodapé com instruções
 * @param win Janela ncurses onde desenhar
 * @return void
 */
void draw_footer(WINDOW *win);

#endif // PROGRAMA_INTERATIVO_NCURSES_H