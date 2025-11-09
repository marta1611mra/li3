#include <stdio.h>
#include <glib.h>
#include "dataset.h"
#include "parser.h"
#include "entities/flights.h"
#include "entities/reservations.h"
#include "entities/passengers.h"
#include "entities/aircrafts.h"
#include "entities/airports.h"
#include "managers/flights_manager.h"
#include "managers/reservations_manager.h"
#include "managers/passengers_manager.h"
#include "managers/aircrafts_manager.h"
#include "managers/airports_manager.h"
#include "programa-principal.h"
#include "queries/query2.h"
#include "queries/query3.h"

// 🔧 Processa o ficheiro de comandos
static void process_commands(Dataset d, const char *commands_file) {
    FILE *f = fopen(commands_file, "r");
    if (!f) {
        perror(commands_file);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        int query_id;
        char arg1[128] = "", arg2[128] = "", arg3[128] = "";

        // Exemplo: "2 10 Airbus" ou "3 LPPT 2023-01-01 2023-12-31"
        int n = sscanf(line, "%d %127s %127s %127s", &query_id, arg1, arg2, arg3);

        char out_path[256];
        snprintf(out_path, sizeof(out_path), "resultados/command%d_output.txt", query_id);
        FILE *out = fopen(out_path, "w");
        if (!out) {
            perror(out_path);
            continue;
        }

        switch (query_id) {

            case 1: {
                //query fantasma 
                fprintf(out, "\n");
                break;
            }
            case 2: {
                // 🛩️ Query 2: Top N aircrafts with most flights
                int N = atoi(arg1);
                const char *manufacturer = (n >= 3) ? arg2 : NULL;

                query2_topN_aircrafts(
                    dataset_get_flights(d),
                    dataset_get_aircrafts(d),
                    N,
                    manufacturer,
                    out
                );
                break;
            }

            case 3: {
                // arg1 = data início, arg2 = data fim
            const char *start_date = arg1;
            const char *end_date   = arg2;

            q3(start_date, end_date,
            dataset_get_flights(d),
            dataset_get_airports(d),
            out);
            break;
            }

            default:
                fprintf(stderr, "⚠️ Query desconhecida: %d\n", query_id);
                fprintf(out, "\n"); //cria o ficheiro vazio se query desconhecida
                break;
        }

        fclose(out);
    }

    fclose(f);
}

int run_programa_principal(const char *dataset_path, const char *commands_file) {
    printf("📂 A carregar dataset da pasta: %s\n", dataset_path);

    Dataset d = dataset_create();
    if (!d) {
        fprintf(stderr, "❌ Erro ao criar dataset.\n");
        return 1;
    }

    printf("🧩 A processar datasets...\n");
    parse_airports(d, dataset_path);
    parse_aircrafts(d, dataset_path);
    parse_passengers(d, dataset_path);
    parse_flights(d, dataset_path);
    parse_reservations(d, dataset_path);
    printf("✅ Todos os ficheiros carregados.\n");

    // 👉 Aqui no futuro: processar as queries de input.txt (fase 2)
    printf("📜 A processar comandos de %s (a implementar na fase 2)\n", commands_file);

    printf("\n📊 Resumo do carregamento:\n");
    printf("  ✈️  Voos válidos:         %d\n", flights_manager_count(dataset_get_flights(d)));
    printf("  🧍‍♂️ Passageiros válidos:  %d\n", passengers_manager_count(dataset_get_passengers(d)));
    printf("  🏢 Aeroportos válidos:     %d\n", airports_manager_count(dataset_get_airports(d)));
    printf("  🛩️  Aeronaves válidas:     %d\n", aircrafts_manager_count(dataset_get_aircrafts(d)));
    printf("  🎫 Reservas válidas:       %d\n", reservations_manager_count(dataset_get_reservations(d)));

    printf("\n📁 Ficheiros de erros foram gerados em 'resultados/'\n");

    printf("📜 A processar comandos de %s...\n", commands_file);
    process_commands(d, commands_file);  // <--- adiciona esta linha
    printf("✅ Comandos processados com sucesso.\n");

    dataset_destroy(d);
    printf("\n✅ Memória libertada. Programa terminado com sucesso.\n");
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <pasta_dataset> <ficheiro_comandos>\n", argv[0]);
        return 1;
    }
    return run_programa_principal(argv[1], argv[2]);
}
