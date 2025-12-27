#include "queries/query1.h"
#include "queries/query2.h"
#include "queries/query3.h"
#include "queries/query4.h"
#include "queries/query5.h"
#include "queries/query6.h"
#include "dataset.h"
#include "parser/parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Limpa o buffer de entrada para evitar lixo após scanf
static void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// lê uma linha do stdin 
static int read_line(char *buffer, size_t size) {
    if (!fgets(buffer, size, stdin)) {
        return 0;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    return 1;
}

// Exexuta o programa interativo
void interactive_program(char *path) {
    char buffer[256];
    Dataset dataset = NULL;

    // Criar dataset
    dataset = dataset_create();
    if (!dataset) {
        printf("Erro ao criar o dataset.\n");
        return;
    }

    // Pedir caminho do dataset se não fornecido
    if (path == NULL) {
        printf("Introduza o caminho dos ficheiros de dados: ");
        if (!read_line(buffer, sizeof(buffer))) {
            printf("Erro ao ler o caminho.\n");
            dataset_destroy(dataset);
            return;
        }
        
        // 
        if (strlen(buffer) == 0) {
            strncpy(buffer, "dataset", sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = '\0';
        }
    } else {
        strncpy(buffer, path, sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';
    }

    // Parse dataset
    parse_all(dataset, buffer);
    printf("Dataset carregado...\n");

    // loop para executar queries
    while (1) {
        int query_number;
        
        printf("\nQue query deseja executar (1-6, 0 para sair)? ");
        
        if (scanf("%d", &query_number) != 1) {
            printf("Entrada inválida. Por favor, introduza um número.\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        if (query_number == 0) {
            printf("A sair...\n");
            break;
        }

        if (query_number < 1 || query_number > 6) {
            printf("Query inválida. Escolha entre 1 e 6.\n");
            continue;
        }

        switch (query_number) {
            case 1: {
                char airport_code[10];
                printf("Introduza o código do aeroporto: ");
                if (!read_line(airport_code, sizeof(airport_code))) {
                    printf("Erro ao ler o código.\n");
                    break;
                }
                
                if (strlen(airport_code) == 0) {
                    printf("Código não pode estar vazio.\n");
                    break;
                }

                printf("\nResultado:\n");
                
                // Capturar output para verificar se está vazio
                FILE *temp = tmpfile();
                if (temp) {
                    q1(dataset_get_airports(dataset), airport_code, temp);
                    rewind(temp);
                    
                    char result[1024];
                    if (fgets(result, sizeof(result), temp)) {
                        if (strlen(result) <= 1) {  // apenas '\n' ou vazio
                            printf("Nenhum resultado encontrado para o código '%s'.\n", airport_code);
                        } else {
                            printf("%s", result);
                        }
                    }
                    fclose(temp);
                } else {
                    q1(dataset_get_airports(dataset), airport_code, stdout);
                }
                break;
            }

            case 2: {
                int N;
                char manufacturer[100];
                
                printf("Introduza o número N: ");
                if (scanf("%d", &N) != 1 || N <= 0) {
                    printf("N inválido. Deve ser um número positivo.\n");
                    clear_input_buffer();
                    break;
                }
                clear_input_buffer();

                printf("Introduza o fabricante (ou deixe vazio): ");
                if (!read_line(manufacturer, sizeof(manufacturer))) {
                    printf("Erro ao ler o fabricante.\n");
                    break;
                }

                printf("\nResultado:\n");
                
                FILE *temp = tmpfile();
                if (temp) {
                    q2(dataset,
                       N,
                       strlen(manufacturer) > 0 ? manufacturer : NULL,
                       temp);
                    rewind(temp);
                    
                    char line[1024];
                    int has_results = 0;
                    while (fgets(line, sizeof(line), temp)) {
                        if (strlen(line) > 1) {
                            printf("%s", line);
                            has_results = 1;
                        }
                    }
                    
                    if (!has_results) {
                        if (strlen(manufacturer) > 0) {
                            printf("Nenhum resultado encontrado para o fabricante '%s'.\n", manufacturer);
                        } else {
                            printf("Nenhuma aeronave encontrada.\n");
                        }
                    }
                    fclose(temp);
                } else {
                    q2(dataset,
                       N,
                       strlen(manufacturer) > 0 ? manufacturer : NULL,
                       stdout);
                }
                break;
            }

            case 3: {
                char start_date[20], end_date[20];

                printf("Introduza a data de início (YYYY-MM-DD): ");
                if (!read_line(start_date, sizeof(start_date))) {
                    printf("Erro ao ler a data.\n");
                    break;
                }

                printf("Introduza a data de fim (YYYY-MM-DD): ");
                if (!read_line(end_date, sizeof(end_date))) {
                    printf("Erro ao ler a data.\n");
                    break;
                }

                if (strlen(start_date) == 0 || strlen(end_date) == 0) {
                    printf("Datas não podem estar vazias.\n");
                    break;
                }

                printf("\nResultado:\n");
                
                FILE *temp = tmpfile();
                if (temp) {
                    q3(dataset, (char *[]){start_date, end_date}, temp);
                    rewind(temp);
                    
                    char result[1024];
                    if (fgets(result, sizeof(result), temp)) {
                        if (strlen(result) <= 1) {
                            printf("Nenhum voo encontrado no intervalo de %s a %s.\n", 
                                   start_date, end_date);
                        } else {
                            printf("%s", result);
                        }
                    }
                    fclose(temp);
                } else {
                    q3(dataset, (char *[]){start_date, end_date}, stdout);
                }
                break;
            }

            case 4: {
                char begin_date[20], end_date[20];

                printf("Introduza a data de início (YYYY-MM-DD) ou deixe vazio: ");
                if (!read_line(begin_date, sizeof(begin_date))) {
                    printf("Erro ao ler a data.\n");
                    break;
                }

                printf("Introduza a data de fim (YYYY-MM-DD) ou deixe vazio: ");
                if (!read_line(end_date, sizeof(end_date))) {
                    printf("Erro ao ler a data.\n");
                    break;
                }

                printf("\nResultado:\n");
                
                FILE *temp = tmpfile();
                if (temp) {
                    query4_execute(dataset,
                                  strlen(begin_date) > 0 ? begin_date : NULL,
                                  strlen(end_date) > 0 ? end_date : NULL,
                                  temp);
                    rewind(temp);
                    
                    char result[1024];
                    if (fgets(result, sizeof(result), temp)) {
                        if (strlen(result) <= 1) {
                            if (strlen(begin_date) > 0 && strlen(end_date) > 0) {
                                printf("Nenhum passageiro encontrado no intervalo de %s a %s.\n", 
                                       begin_date, end_date);
                            } else {
                                printf("Nenhum passageiro encontrado.\n");
                            }
                        } else {
                            printf("%s", result);
                        }
                    }
                    fclose(temp);
                } else {
                    query4_execute(dataset,
                                  strlen(begin_date) > 0 ? begin_date : NULL,
                                  strlen(end_date) > 0 ? end_date : NULL,
                                  stdout);
                }
                break;
            }

            case 5: {
                int N;
                
                printf("Introduza o número N: ");
                if (scanf("%d", &N) != 1 || N <= 0) {
                    printf("N inválido. Deve ser um número positivo.\n");
                    clear_input_buffer();
                    break;
                }
                clear_input_buffer();

                printf("\nResultado:\n");
                
                FILE *temp = tmpfile();
                if (temp) {
                    query5(dataset, N, temp);
                    rewind(temp);
                    
                    char line[1024];
                    int has_results = 0;
                    while (fgets(line, sizeof(line), temp)) {
                        if (strlen(line) > 1) {
                            printf("%s", line);
                            has_results = 1;
                        }
                    }
                    
                    if (!has_results) {
                        printf("Nenhuma companhia aérea com atrasos encontrada.\n");
                    }
                    fclose(temp);
                } else {
                    query5(dataset, N, stdout);
                }
                break;
            }

            case 6: {
                char nationality[100];
                
                printf("Introduza a nacionalidade: ");
                if (!read_line(nationality, sizeof(nationality))) {
                    printf("Erro ao ler a nacionalidade.\n");
                    break;
                }

                if (strlen(nationality) == 0) {
                    printf("Nacionalidade não pode estar vazia.\n");
                    break;
                }

                printf("\nResultado:\n");
                
                FILE *temp = tmpfile();
                if (temp) {
                    q6(dataset,
                       nationality,
                       temp);
                    rewind(temp);
                    
                    char result[1024];
                    if (fgets(result, sizeof(result), temp)) {
                        if (strlen(result) <= 1) {
                            printf("Nenhum passageiro encontrado com a nacionalidade '%s'.\n", 
                                   nationality);
                        } else {
                            printf("%s", result);
                        }
                    }
                    fclose(temp);
                } else {
                    q6(dataset,
                       nationality,
                       stdout);
                }
                break;
            }

            default:
                printf("Query não implementada.\n");
                break;
        }

        // Perguntar se deseja executar outra query
        printf("\nDeseja executar outra query? (s/n): ");
        char choice;
        if (scanf(" %c", &choice) != 1) {
            break;
        }
        clear_input_buffer();
        
        if (choice != 's' && choice != 'S') {
            printf("A sair...\n");
            break;
        }
    }

    dataset_destroy(dataset);
}

int main(void) {
    interactive_program(NULL);
    return 0;
}