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

/** @brief Executa o programa interativo do sistema. 
 * Esta função permite ao utilizador carregar um dataset e executar queries interativamente, 
 * lendo inputs do terminal e exibindo os resultados diretamente no stdout.
 * @param path Caminho opcional para o dataset (pode ser NULL)
 * @return void
 */
void interactive_program(char *path);