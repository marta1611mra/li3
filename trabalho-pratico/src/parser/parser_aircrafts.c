#include "parser/parser.h"
#include "dataset.h"
#include "syntactic_validation.h"
#include "logical_validation.h"
#include "aircrafts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>

// Lê e processa o ficheiro aircrafts.csv.
void parse_aircrafts(Dataset d, const char *data_path) {
    char clean_path[512];
    strcpy(clean_path, data_path);
    size_t len = strlen(clean_path);
    while (len > 0 && clean_path[len - 1] == '/') {
        clean_path[len - 1] = '\0';
        len--;
    }

    char path[1024];
    snprintf(path, sizeof(path), "%s/aircrafts.csv", clean_path);

    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return; }
    
    // Buffer de I/O 64KB
    setvbuf(f, NULL, _IOFBF, 65536);
    
    printf("A ler ficheiro: %s\n", path);

    exist_result();
    FILE *ferror = fopen("resultados/aircrafts_errors.csv", "w");
    if (!ferror) { perror("resultados/aircrafts_errors.csv"); fclose(f); return; }
    
    // Buffer de I/O 64KB para ferror
    setvbuf(ferror, NULL, _IOFBF, 65536);

    wcsv_header(f, ferror);

    // Buffer de linha 16KB
    char line[16384];
    
    while (fgets(line, sizeof(line), f)) {
        char id[16] = "", manufacturer[51] = "", model[51] = "";
        char year_str[8] = "", capacity_str[8] = "", range_str[10] = "";

        int n = sscanf(line,
                "\"%15[^\"]\",\"%50[^\"]\",\"%50[^\"]\",\"%7[^\"]\",\"%7[^\"]\",\"%9[^\"]\"",
                id, manufacturer, model, year_str, capacity_str, range_str);

        if (n < 6) { fprintf(ferror, "%s", line); continue; }

        remove_quotes(id); remove_spc(id);
        remove_quotes(manufacturer); remove_spc(manufacturer);
        remove_quotes(model); remove_spc(model);
        remove_quotes(year_str); remove_spc(year_str);
        remove_quotes(capacity_str); remove_spc(capacity_str);
        remove_quotes(range_str); remove_spc(range_str);

        // Validação sintática
        if (strlen(id) == 0 ||
            !validate_year(year_str) ||
            !is_all_digits(capacity_str) || atoi(capacity_str) <= 0 ||
            !is_all_digits(range_str) || atoi(range_str) <= 0) {
            fprintf(ferror, "%s", line);
            continue;
        }

        // Conversão segura após validação
        int year = atoi(year_str);
        int capacity = atoi(capacity_str);
        int range = atoi(range_str);

        Aircraft a = create_aircraft(id, manufacturer, model, year, capacity, range);
        if (!a) {
            fprintf(stderr, "create_aircraft failed\n"); 
            fprintf(ferror, "%s", line); 
            continue; 
        }

        AircraftsManager am = dataset_get_aircrafts(d);
        aircrafts_manager_add(am, a);
    }

    fclose(f);
    fclose(ferror);
}