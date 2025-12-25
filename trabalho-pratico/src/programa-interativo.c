#include "queries/query1.h"
#include "queries/query2.h"
#include "queries/query3.h"
#include "queries/query4.h"
#include "queries/query5.h"
#include "queries/query6.h"
#include "dataset.h"
#include <glib.h>
#include "flights_manager.h"
#include "airports_manager.h"
#include "aircrafts_manager.h"
#include "reservations_manager.h"
#include "passengers_manager.h"
#include <stdio.h>
#include <string.h>
#include "parser/parser.h"

/*
make
./programa-interativo
Output do terminal:
Introduza o caminho dos ficheiros de dados: <input do utilizador>
Dataset carregado...
Que query deseja executar? <input do utilizador>
Código do aeroporto: <input do utilizador>
Output: ...
*/

void interactive_program(char *path){
    char buffer[256];

    // Criar dataset
    Dataset dataset = dataset_create();
    if (!dataset)
    {
        printf("Erro ao criar o dataset.\n");
        return;
    }
    if (path == NULL)
    {
        // Ler caminho dos ficheiros
        printf("Introduza o caminho dos ficheiros de dados: ");
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            dataset_destroy(dataset);
            return;
        }
        buffer[strcspn(buffer, "\n")] = 0;
    }
    else
    {
        strncpy(buffer, path, sizeof(buffer));
        buffer[sizeof(buffer) - 1] = 0;
    }

    parse_all(dataset, buffer);

    printf("Dataset carregado...\n");

    // Ler número da query a executar
    int query_number;
    printf("Que query deseja executar? ");
    if (scanf("%d", &query_number) != 1)
    {
        dataset_destroy(dataset);
        return;
    }
    while (getchar() != '\n')
        ; // limpa todo o resto da linha, incluindo '\n'

    switch (query_number)
    {

    case 1:
    {
        char airport_code[10];
        printf("Introduza o código do aeroporto: ");
        if (!fgets(airport_code, sizeof(airport_code), stdin))
            break;
        airport_code[strcspn(airport_code, "\n")] = 0;

        q1(
            dataset_get_airports(dataset),
            airport_code,
            stdout);
        break;
    }

    case 2:
    {
        int N;
        printf("Introduza o número N: ");
        if (scanf("%d", &N) != 1)
            break;
        getchar();

        char manufacturer[100];
        printf("Introduza o fabricante (ou deixe vazio): ");
        if (!fgets(manufacturer, sizeof(manufacturer), stdin))
            break;
        manufacturer[strcspn(manufacturer, "\n")] = 0;

        q2(
            dataset_get_flights(dataset),
            dataset_get_aircrafts(dataset),
            N,
            strlen(manufacturer) > 0 ? manufacturer : NULL,
            stdout);
        break;
    }

    case 3:
{
    char start_date[11], end_date[11];

    printf("Introduza a data de início (YYYY-MM-DD): ");
    if (!fgets(start_date, sizeof(start_date), stdin))
        break;
    start_date[strcspn(start_date, "\n")] = 0;

    // limpar buffer antes do próximo fgets
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }

    printf("Introduza a data de fim (YYYY-MM-DD): ");
    if (!fgets(end_date, sizeof(end_date), stdin))
        break;
    end_date[strcspn(end_date, "\n")] = 0;

    q3(start_date, end_date,
       dataset_get_flights(dataset),
       dataset_get_airports(dataset),
       stdout);
    break;
}

case 4:
{
    char begin_date[11], end_date[11];

    printf("Introduza a data de início (YYYY-MM-DD) ou deixe vazio: ");
    // limpar buffer antes de fgets
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }

    if (!fgets(begin_date, sizeof(begin_date), stdin))
        break;
    begin_date[strcspn(begin_date, "\n")] = 0;

    printf("Introduza a data de fim (YYYY-MM-DD) ou deixe vazio: ");
    if (!fgets(end_date, sizeof(end_date), stdin))
        break;
    end_date[strcspn(end_date, "\n")] = 0;

    query4_execute(
        dataset,
        strlen(begin_date) > 0 ? begin_date : NULL,
        strlen(end_date) > 0 ? end_date : NULL);
    break;}


    case 5:
    {
        int N;
        printf("Introduza o número N: ");
        if (scanf("%d", &N) != 1)
            break;
        getchar();

        query5(
            dataset_get_flights(dataset),
            N,
            stdout);
        break;
    }

    case 6:
    {
        char airport_code[10];
        printf("Introduza o código do aeroporto: ");
        if (!fgets(airport_code, sizeof(airport_code), stdin))
            break;
        airport_code[strcspn(airport_code, "\n")] = 0;

        q6(
            dataset_get_passengers(dataset),
            dataset_get_flights(dataset),
            dataset_get_reservations(dataset),
            airport_code,
            stdout);
        break;
    }

    default:
        printf("Query inválida.\n");
        break;
    }

    // voltar para a pergunta inicial ou terminar o programa
    printf("Deseja continuar? (s/n): ");
    char choice;
    if (scanf(" %c", &choice) == 1 && choice == 's')
    {
        interactive_program(buffer);
    }
    dataset_destroy(dataset);
}

int main(void)
{
    interactive_program(NULL);
    return 0;
}
