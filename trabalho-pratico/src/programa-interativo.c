#include "queries/query1.h"
#include "queries/query2.h"
#include "queries/query3.h"
#include "queries/query4.h"
#include "queries/query5.h"
#include "queries/query6.h"
#include "dataset.h"
#include "parser/parser.h"
#include "programa-interativo.h"
#include "output_format.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>

// Título do programa, subtítulo e separador e as respetivas cores 
void draw_header(WINDOW *win) {
    wattron(win, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(win, 2, (COLS - 30) / 2, "SISTEMA DE GESTAO DE VOOS");
    wattroff(win, COLOR_PAIR(1) | A_BOLD);
    
    wattron(win, COLOR_PAIR(2));
    mvwprintw(win, 3, (COLS - 38) / 2, "Interface Interativa - LI3 2025/2026");
    wattroff(win, COLOR_PAIR(2));
    
    wattron(win, COLOR_PAIR(1));
    for (int i = 1; i < COLS - 1; i++) {
        mvwaddch(win, 4, i, '=');
    }
    wattroff(win, COLOR_PAIR(1));
}

// Desenha o menu de queries disponíveis
void draw_menu(WINDOW *win, int selected) {
    const char *queries[] = {
        "1. Resumo de Aeroporto",
        "2. Top N Aeronaves",
        "3. Aeroporto com Mais Partidas",
        "4. Passageiro Mais Frequente no Top 10",
        "5. Top N Companhias com Atraso",
        "6. Aeroporto de Destino Mais Comum"
    };
    
     // Título da secção de queries
    wattron(win, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(win, 5, 4, "QUERIES DISPONIVEIS:");
    wattroff(win, COLOR_PAIR(1) | A_BOLD);
    
    int start_y = 7; // Posição Y inicial para as opções do menu
    
    // Desenha cada opção do menu
    for (int i = 0; i < 6; i++) {
        if (i == selected) {
            // Destacar opção atualmente selecionada com cor verde, negrito e reverse
            wattron(win, COLOR_PAIR(4) | A_BOLD | A_REVERSE);
            mvwprintw(win, start_y + i, 4, "  %s  ", queries[i]);
            wattroff(win, COLOR_PAIR(4) | A_BOLD | A_REVERSE);
        } else { 
            // Mostrar opções não selecionadas com cor normal
            wattron(win, COLOR_PAIR(5));
            mvwprintw(win, start_y + i, 4, "  %s  ", queries[i]);
            wattroff(win, COLOR_PAIR(5));
        }
    }
}

 // Mostrar separador atual e instruções para alternar entre ';' e '='
void draw_separator_option(WINDOW *win, int y, char current_sep) {
    wattron(win, COLOR_PAIR(2));
    mvwprintw(win, y, 4, "Separador de output: [%c]  (pressione 's' para alternar)", current_sep);
    wattroff(win, COLOR_PAIR(2));
}

// Função para obter input do utilizador
void get_input(WINDOW *win, int y, const char *label, char *buffer, int max_len) {
    echo(); // Ativar echo para mostrar o que o utilizador digita
    curs_set(1); // Mostrar cursor durante input
    
    // Limpar área de input para evitar restos de texto anterior
    for (int i = 0; i < 3; i++) {
        wmove(win, y + i, 1);
        wclrtoeol(win);
    }
    
    // Mostrar label do campo com cor ciano e negrito
    wattron(win, COLOR_PAIR(6) | A_BOLD);
    mvwprintw(win, y, 4, "%s:", label);
    wattroff(win, COLOR_PAIR(6) | A_BOLD);
    
    // Mostrar prompt de input (">") na linha seguinte
    mvwprintw(win, y + 1, 4, "> ");
    wrefresh(win);
    
    // Ler input do utilizador
    char temp[QUERY_INPUT_SIZE] = {0};
    wgetnstr(win, temp, max_len - 1);
    strncpy(buffer, temp, max_len - 1);
    
    noecho();
    curs_set(0);
}

void show_results(WINDOW *win, const char *result) {
    int start_y = 16; // Posição Y onde começam os resultados
    int max_lines = LINES - start_y - 4; // Calcular número máximo de linhas disponíveis para resultados
    
    // Limpar área de resultados
    for (int i = start_y; i < LINES - 2; i++) {
        wmove(win, i, 1);
        wclrtoeol(win);
    }
    
    // Desenhar cabeçalho de resultados
    wattron(win, COLOR_PAIR(1));
    for (int i = 1; i < COLS - 1; i++) {
        mvwaddch(win, start_y, i, '=');
    }
    mvwprintw(win, start_y, COLS/2 - 5, " RESULTADO ");
    wattroff(win, COLOR_PAIR(1));
    
    // Verificar se é mensagem de erro, escrevendo ERRO em amarelo
    if (strncmp(result, "ERRO:", 5) == 0) {
        wattron(win, COLOR_PAIR(3));
        mvwprintw(win, start_y + 2, 4, "%s", result);
        wattroff(win, COLOR_PAIR(3));
    }
    // Verificar se é mensagem de "nenhum resultado"
    else if (strcmp(result, "Nenhum resultado encontrado") == 0) {
        wattron(win, COLOR_PAIR(2));
        mvwprintw(win, start_y + 2, 4, "%s", result);
        wattroff(win, COLOR_PAIR(2));
    }
    // Mostrar resultados normais
    else {
        char *result_copy = strdup(result);
        char *line = strtok(result_copy, "\n");
        int line_num = 0;
        
        while (line != NULL && line_num < max_lines) {
            wattron(win, COLOR_PAIR(7));
            mvwprintw(win, start_y + 2 + line_num, 4, "%s", line);
            wattroff(win, COLOR_PAIR(7));
            line = strtok(NULL, "\n");
            line_num++;
        }
        
        free(result_copy);
        
        // Indicar se há mais resultados para além do que foi mostrado
        if (line != NULL) {
            wattron(win, COLOR_PAIR(2) | A_BLINK);
            mvwprintw(win, LINES - 2, 4, "[Mais resultados... pressione qualquer tecla para continuar]");
            wattroff(win, COLOR_PAIR(2) | A_BLINK);
        }
    }
}

// Verifica se um diretório existe
bool directory_exists(const char *path) {
    struct stat info;
    if (stat(path, &info) != 0) return false;
    return (info.st_mode & S_IFDIR) != 0;
}

// Valida se uma string contém apenas dígitos (0-9)
bool is_numeric(const char *str) {
    if (!str || strlen(str) == 0) return false;
    for (int i = 0; str[i]; i++) {
        if (!isdigit((unsigned char)str[i])) return false;
    }
    return true;
}

// Valida formato de data básico (YYYY-MM-DD)
bool is_valid_date_format(const char *date) {
    if (!date || strlen(date) != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit((unsigned char)date[i])) return false;
    }
    return true;
}

// Executa a query selecionada e mostra os resultados
void execute_query(WINDOW *win, Dataset dataset, int query_num, QueryInputs *inputs, char separator) {
    set_output_separator(separator);
    
    FILE *temp = tmpfile();
    if (!temp) {
        show_results(win, "Erro ao criar arquivo temporario");
        return;
    }
    
    // Limpar tela de inputs
    for (int i = 15; i < LINES - 2; i++) {
        wmove(win, i, 1);
        wclrtoeol(win);
    }
    
    // Mostrar mensagem de processamento em verde
    wattron(win, COLOR_PAIR(4) | A_BOLD);
    mvwprintw(win, 17, 4, "A executar query...");
    wattroff(win, COLOR_PAIR(4) | A_BOLD);
    wrefresh(win);
    
    bool input_invalid = false;
    
    switch(query_num) {
        case 0: // Query 1
            get_input(win, 15, "Codigo do Aeroporto (ex: OPO)", inputs->code, QUERY_INPUT_SIZE);
            if (strlen(inputs->code) == 0) {
                input_invalid = true;
            } else if (strlen(inputs->code) != 3) {
                input_invalid = true;
            } else {
                for (int i = 0; i < 3; i++) {
                    if (!isupper(inputs->code[i])) {
                        input_invalid = true;
                        break;
                    }
                }
                if (!input_invalid) {
                    q1(dataset_get_airports(dataset), inputs->code, temp);
                }
            }
            break;
            
        case 1: // Query 2
            get_input(win, 15, "Numero N", inputs->manufacturer, QUERY_INPUT_SIZE);
            if (!is_numeric(inputs->manufacturer)) {
                input_invalid = true;
            } else {
                inputs->n = atoi(inputs->manufacturer);
                if (inputs->n <= 0) {
                    input_invalid = true;
                } else {
                    get_input(win, 18, "Fabricante (opcional, ex: Boeing)", inputs->manufacturer, QUERY_INPUT_SIZE);
                    q2(dataset, inputs->n, 
                       strlen(inputs->manufacturer) > 0 ? inputs->manufacturer : NULL, 
                       temp);
                }
            }
            break;
            
        case 2: // Query 3
            get_input(win, 15, "Data Inicio (YYYY-MM-DD)", inputs->start_date, QUERY_INPUT_SIZE);
            get_input(win, 18, "Data Fim (YYYY-MM-DD)", inputs->end_date, QUERY_INPUT_SIZE);
            
            if (strlen(inputs->start_date) == 0 || strlen(inputs->end_date) == 0) {
                input_invalid = true;
            } else if (!is_valid_date_format(inputs->start_date) || 
                       !is_valid_date_format(inputs->end_date)) {
                input_invalid = true;
            } else {
                char *dates[2] = {inputs->start_date, inputs->end_date};
                q3(dataset, dates, temp);
            }
            break;
            
        case 3: // Query 4
            get_input(win, 15, "Data Inicio (opcional, YYYY-MM-DD)", inputs->begin_date, QUERY_INPUT_SIZE);
            get_input(win, 18, "Data Fim (opcional, YYYY-MM-DD)", inputs->end_date, QUERY_INPUT_SIZE);
            
            // Validar apenas se não estiver vazio
            if (strlen(inputs->begin_date) > 0 && !is_valid_date_format(inputs->begin_date)) {
                input_invalid = true;
            } else if (strlen(inputs->end_date) > 0 && !is_valid_date_format(inputs->end_date)) {
                input_invalid = true;
            } else {
                query4_execute(dataset,
                              strlen(inputs->begin_date) > 0 ? inputs->begin_date : NULL,
                              strlen(inputs->end_date) > 0 ? inputs->end_date : NULL,
                              temp);
            }
            break;
            
        case 4: // Query 5
            get_input(win, 15, "Numero N", inputs->manufacturer, QUERY_INPUT_SIZE);
            if (!is_numeric(inputs->manufacturer)) {
                input_invalid = true;
            } else {
                inputs->n = atoi(inputs->manufacturer);
                if (inputs->n <= 0) {
                    input_invalid = true;
                } else {
                    query5(dataset, inputs->n, temp);
                }
            }
            break;
            
        case 5: // Query 6
            get_input(win, 15, "Nacionalidade (ex: Brazil)", inputs->nationality, QUERY_INPUT_SIZE);
            if (strlen(inputs->nationality) == 0) {
                input_invalid = true;
            } else {
                q6(dataset, inputs->nationality, temp);
            }
            break;
    }
    
    // Ler resultados
    rewind(temp);
    char result[8192] = {0};
    char line[512];
    while (fgets(line, sizeof(line), temp) && strlen(result) < sizeof(result) - 512) {
        strcat(result, line);
    }
    fclose(temp);
    
    // Mostrar resultado ou mensagem apropriada
    if (input_invalid) {
        show_results(win, "ERRO: Input invalido");
    } else if (strlen(result) == 0) {
        show_results(win, "Nenhum resultado encontrado");
    } else {
        show_results(win, result);
    }
}

// Desenha o rodapé com instruções
void draw_footer(WINDOW *win) {
    wattron(win, COLOR_PAIR(1));
    for (int i = 1; i < COLS - 1; i++) {
        mvwaddch(win, LINES - 3, i, '=');
    }
    wattroff(win, COLOR_PAIR(1));
    
    wattron(win, COLOR_PAIR(2));
    mvwprintw(win, LINES - 2, 4, "SETAS: Navegar  |  ENTER: Executar  |  S: Separador  |  Q: Sair");
    wattroff(win, COLOR_PAIR(2));
}

int main(int argc, char **argv) {
    // Configurar para UTF-8
    setlocale(LC_ALL, "");
    
    // Pedir caminho do dataset ao utilizador
    char dataset_path[512];
    bool path_valid = false;
    
    while (!path_valid) {
        printf("Introduza o caminho dos ficheiros de dados: ");
        fflush(stdout);
        
        if (fgets(dataset_path, sizeof(dataset_path), stdin) == NULL) {
            fprintf(stderr, "Erro ao ler caminho\n");
            return 1;
        }
        
        // Remover newline
        dataset_path[strcspn(dataset_path, "\n")] = 0;
        
        // Se vazio, usar default
        if (strlen(dataset_path) == 0) {
            strcpy(dataset_path, "./dataset");
        }
        
        // Verificar se o diretório existe
        if (!directory_exists(dataset_path)) {
            printf("Erro: O caminho '%s' nao existe ou nao e um diretorio.\n", dataset_path);
            printf("Por favor, tente novamente.\n\n");
            continue;
        }
        
        // Verificar se os ficheiros CSV necessários existem
        const char *required_files[] = {
            "airports.csv",
            "aircrafts.csv", 
            "flights.csv",
            "passengers.csv",
            "reservations.csv"
        };
        
        bool all_files_exist = true;
        for (int i = 0; i < 5; i++) {
            char test_path[1024];
            snprintf(test_path, sizeof(test_path), "%s/%s", dataset_path, required_files[i]);
            FILE *test_file = fopen(test_path, "r");
            if (!test_file) {
                printf("Erro: Ficheiro '%s' nao encontrado em '%s'.\n", 
                       required_files[i], dataset_path);
                all_files_exist = false;
                break;
            }
            fclose(test_file);
        }
        
        if (!all_files_exist) {
            printf("Certifique-se de que o caminho contem todos os ficheiros CSV necessarios.\n\n");
            continue;
        }
        
        path_valid = true;
    }
    
    // Carregar dataset
    Dataset dataset = dataset_create();
    if (!dataset) {
        fprintf(stderr, "Erro ao criar dataset\n");
        return 1;
    }
    
    // Anunciar carregamento do dataset
    printf("\nA carregar dataset de: %s\n", dataset_path);
    parse_all(dataset, dataset_path);
    printf("Dataset carregado...\n");
    printf("A iniciar interface...\n");
    sleep(1);
    
    // Iniciar ncurses
    initscr();
    start_color();
    cbreak();
    noecho();
    curs_set(0);
    
    // Cores
    init_pair(1, COLOR_CYAN, COLOR_BLACK);      // Título
    init_pair(2, COLOR_WHITE, COLOR_BLACK);     // Info
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);    // Separadores
    init_pair(4, COLOR_GREEN, COLOR_BLACK);     // Selecionado
    init_pair(5, COLOR_WHITE, COLOR_BLACK);     // Normal
    init_pair(6, COLOR_CYAN, COLOR_BLACK);      // Labels
    init_pair(7, COLOR_GREEN, COLOR_BLACK);     // Resultados
    
    // Criar janela principal
    WINDOW *main_win = newwin(LINES, COLS, 0, 0);
    keypad(main_win, TRUE);  
    box(main_win, 0, 0);
    
    int selected = 0;
    char separator = ';';
    QueryInputs inputs = {0};
    
    // Loop principal
    while (1) {
        wclear(main_win);
        box(main_win, 0, 0);
        
        draw_header(main_win);
        draw_menu(main_win, selected);
        draw_separator_option(main_win, 14, separator);
        draw_footer(main_win);
        
        wrefresh(main_win);
        
        int ch = wgetch(main_win);
        
        if (ch == KEY_UP || ch == 'k' || ch == 'K') {
            selected = (selected - 1 + 6) % 6;
        }
        else if (ch == KEY_DOWN || ch == 'j' || ch == 'J') {
            selected = (selected + 1) % 6;
        }
        else if (ch == 's' || ch == 'S') {
            separator = (separator == ';') ? '=' : ';';
        }
        else if (ch == 10 || ch == 13 || ch == KEY_ENTER) {  // Enter
            execute_query(main_win, dataset, selected, &inputs, separator);
            wgetch(main_win); // Esperar tecla para continuar
        }
        else if (ch == 'q' || ch == 'Q' || ch == 27) {  // Q ou ESC
            break;
        }
    }
    delwin(main_win);
    endwin();
    dataset_destroy(dataset);
    
    printf("\nPrograma terminado. Ate breve!\n");
    
    return 0;
}