#include "queries/query1.h"
#include "queries/query2.h"
#include "queries/query3.h"
#include "queries/query4.h"
#include "queries/query5.h"
#include "queries/query6.h"
#include "dataset.h"
#include "parser/parser.h"
#include "output_format.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <locale.h>
#include <unistd.h>

#define QUERY_INPUT_SIZE 256

typedef struct {
    char code[QUERY_INPUT_SIZE];
    int n;
    char manufacturer[QUERY_INPUT_SIZE];
    char start_date[QUERY_INPUT_SIZE];
    char end_date[QUERY_INPUT_SIZE];
    char begin_date[QUERY_INPUT_SIZE];
    char nationality[QUERY_INPUT_SIZE];
} QueryInputs;

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

void draw_menu(WINDOW *win, int selected) {
    const char *queries[] = {
        "1. Resumo de Aeroporto",
        "2. Top N Aeronaves",
        "3. Aeroporto com Mais Partidas",
        "4. Passageiro Mais Frequente no Top 10",
        "5. Top N Companhias com Atraso",
        "6. Aeroporto de Destino Mais Comum"
    };
    
    // Título da seção
    wattron(win, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(win, 5, 4, "QUERIES DISPONIVEIS:");
    wattroff(win, COLOR_PAIR(1) | A_BOLD);
    
    int start_y = 7;
    
    for (int i = 0; i < 6; i++) {
        if (i == selected) {
            wattron(win, COLOR_PAIR(4) | A_BOLD | A_REVERSE);
            mvwprintw(win, start_y + i, 4, "  %s  ", queries[i]);
            wattroff(win, COLOR_PAIR(4) | A_BOLD | A_REVERSE);
        } else {
            wattron(win, COLOR_PAIR(5));
            mvwprintw(win, start_y + i, 4, "  %s  ", queries[i]);
            wattroff(win, COLOR_PAIR(5));
        }
    }
}

void draw_separator_option(WINDOW *win, int y, char current_sep) {
    wattron(win, COLOR_PAIR(2));
    mvwprintw(win, y, 4, "Separador de output: [%c]  (pressione 's' para alternar)", current_sep);
    wattroff(win, COLOR_PAIR(2));
}

void get_input(WINDOW *win, int y, const char *label, char *buffer, int max_len) {
    echo();
    curs_set(1);
    
    // Limpar área
    for (int i = 0; i < 3; i++) {
        wmove(win, y + i, 1);
        wclrtoeol(win);
    }
    
    wattron(win, COLOR_PAIR(6) | A_BOLD);
    mvwprintw(win, y, 4, "%s:", label);
    wattroff(win, COLOR_PAIR(6) | A_BOLD);
    
    mvwprintw(win, y + 1, 4, "> ");
    wrefresh(win);
    
    char temp[QUERY_INPUT_SIZE] = {0};
    wgetnstr(win, temp, max_len - 1);
    strncpy(buffer, temp, max_len - 1);
    
    noecho();
    curs_set(0);
}

void show_results(WINDOW *win, const char *result) {
    int start_y = 16;
    int max_lines = LINES - start_y - 4;
    
    // Limpar área de resultados
    for (int i = start_y; i < LINES - 2; i++) {
        wmove(win, i, 1);
        wclrtoeol(win);
    }
    
    wattron(win, COLOR_PAIR(1));
    for (int i = 1; i < COLS - 1; i++) {
        mvwaddch(win, start_y, i, '=');
    }
    mvwprintw(win, start_y, COLS/2 - 5, " RESULTADO ");
    wattroff(win, COLOR_PAIR(1));
    
    if (strlen(result) == 0) {
        wattron(win, COLOR_PAIR(2));
        mvwprintw(win, start_y + 2, 4, "Nenhum resultado encontrado");
        wattroff(win, COLOR_PAIR(2));
    } else {
        // Copiar string para não modificar original
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
        
        if (line != NULL) {
            wattron(win, COLOR_PAIR(2) | A_BLINK);
            mvwprintw(win, LINES - 2, 4, "[Mais resultados... pressione qualquer tecla para continuar]");
            wattroff(win, COLOR_PAIR(2) | A_BLINK);
        }
    }
}

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
    
    wattron(win, COLOR_PAIR(4) | A_BOLD);
    mvwprintw(win, 17, 4, "A executar query...");
    wattroff(win, COLOR_PAIR(4) | A_BOLD);
    wrefresh(win);
    
    switch(query_num) {
        case 0: // Query 1
            get_input(win, 15, "Codigo do Aeroporto (ex: OPO)", inputs->code, QUERY_INPUT_SIZE);
            if (strlen(inputs->code) > 0) {
                q1(dataset_get_airports(dataset), inputs->code, temp);
            }
            break;
            
        case 1: // Query 2
            get_input(win, 15, "Numero N", inputs->manufacturer, QUERY_INPUT_SIZE);
            inputs->n = atoi(inputs->manufacturer);
            get_input(win, 18, "Fabricante (opcional, ex: Boeing)", inputs->manufacturer, QUERY_INPUT_SIZE);
            if (inputs->n > 0) {
                q2(dataset, inputs->n, 
                   strlen(inputs->manufacturer) > 0 ? inputs->manufacturer : NULL, 
                   temp);
            }
            break;
            
        case 2: // Query 3
            get_input(win, 15, "Data Inicio (YYYY-MM-DD)", inputs->start_date, QUERY_INPUT_SIZE);
            get_input(win, 18, "Data Fim (YYYY-MM-DD)", inputs->end_date, QUERY_INPUT_SIZE);
            if (strlen(inputs->start_date) > 0 && strlen(inputs->end_date) > 0) {
                char *dates[2] = {inputs->start_date, inputs->end_date};
                q3(dataset, dates, temp);
            }
            break;
            
        case 3: // Query 4
            get_input(win, 15, "Data Inicio (opcional, YYYY-MM-DD)", inputs->begin_date, QUERY_INPUT_SIZE);
            get_input(win, 18, "Data Fim (opcional, YYYY-MM-DD)", inputs->end_date, QUERY_INPUT_SIZE);
            query4_execute(dataset,
                          strlen(inputs->begin_date) > 0 ? inputs->begin_date : NULL,
                          strlen(inputs->end_date) > 0 ? inputs->end_date : NULL,
                          temp);
            break;
            
        case 4: // Query 5
            get_input(win, 15, "Numero N", inputs->manufacturer, QUERY_INPUT_SIZE);
            inputs->n = atoi(inputs->manufacturer);
            if (inputs->n > 0) {
                query5(dataset, inputs->n, temp);
            }
            break;
            
        case 5: // Query 6
            get_input(win, 15, "Nacionalidade (ex: Brazil)", inputs->nationality, QUERY_INPUT_SIZE);
            if (strlen(inputs->nationality) > 0) {
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
    
    show_results(win, result);
}

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
    // Configurar locale para UTF-8
    setlocale(LC_ALL, "");
    
    // Carregar dataset
    Dataset dataset = dataset_create();
    if (!dataset) {
        fprintf(stderr, "Erro ao criar dataset\n");
        return 1;
    }
    
    char *dataset_path = (argc > 1) ? argv[1] : "./src/dataset-errors";
    printf("Carregando dataset de: %s\n", dataset_path);
    parse_all(dataset, dataset_path);
    printf("Dataset carregado!\n");
    printf("Iniciando interface...\n");
    sleep(1);
    
    // Inicializar ncurses
    initscr();
    start_color();
    cbreak();
    noecho();
    curs_set(0);
    
    // Cores melhoradas
    init_pair(1, COLOR_CYAN, COLOR_BLACK);      // Título
    init_pair(2, COLOR_WHITE, COLOR_BLACK);     // Info
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);    // Separadores
    init_pair(4, COLOR_GREEN, COLOR_BLACK);     // Selecionado
    init_pair(5, COLOR_WHITE, COLOR_BLACK);     // Normal
    init_pair(6, COLOR_CYAN, COLOR_BLACK);      // Labels
    init_pair(7, COLOR_GREEN, COLOR_BLACK);     // Resultados
    
    WINDOW *main_win = newwin(LINES, COLS, 0, 0);
    keypad(main_win, TRUE);  // IMPORTANTE: keypad na janela, não no stdscr
    box(main_win, 0, 0);
    
    int selected = 0;
    char separator = ';';
    QueryInputs inputs = {0};
    
    while (1) {
        wclear(main_win);
        box(main_win, 0, 0);
        
        draw_header(main_win);
        draw_menu(main_win, selected);
        draw_separator_option(main_win, 14, separator);
        draw_footer(main_win);
        
        wrefresh(main_win);
        
        int ch = wgetch(main_win);
        
        // Debug: descomentar para ver o código da tecla
        // mvwprintw(main_win, 1, 1, "Tecla: %d", ch);
        // wrefresh(main_win);
        
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