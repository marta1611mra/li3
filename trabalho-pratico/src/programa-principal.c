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
