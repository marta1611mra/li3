#include "parser/parser.h"
#include "dataset.h"
#include "syntactic_validation.h"
#include "logical_validation.h"
#include "airports.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>

// Lê e processa o ficheiro airports.csv.
void parse_airports(Dataset d, const char *data_path) {
    // Normaliza o path removendo '/' no final
    char clean_path[512];
    strcpy(clean_path, data_path);
    size_t len = strlen(clean_path);
    while (len > 0 && clean_path[len - 1] == '/') {
        clean_path[len - 1] = '\0';
        len--;
    }
    char path[1024];
    snprintf(path, sizeof(path), "%s/airports.csv", clean_path);

    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return; }
    
    // Buffer de I/O 64KB para leitura mais rápida
    setvbuf(f, NULL, _IOFBF, 65536);
    
    printf("A ler ficheiro: %s\n", path);

    exist_result();
    FILE *ferror = fopen("resultados/airports_errors.csv", "w");
    if (!ferror) { perror("resultados/airports_errors.csv"); fclose(f); return; }
    
    // Buffer de I/O 64KB para escrita mais rápida
    setvbuf(ferror, NULL, _IOFBF, 65536);

    wcsv_header(f, ferror);

    // Buffer de linha 16KB para suportar linhas maiores
    char line[16384];
    
    while (fgets(line, sizeof(line), f)) {
        char code[4] = "", name[101] = "", city[101] = "", country[51] = "";
        char lat_str[17] = "", long_str[17] = "", icao[5] = "", type[31] = "";

        int n = sscanf(line,
            "\"%3[^\"]\",\"%100[^\"]\",\"%100[^\"]\",\"%50[^\"]\",\"%16[^\"]\",\"%16[^\"]\",\"%4[^\"]\",\"%30[^\"]\"",
            code, name, city, country, lat_str, long_str, icao, type);

        if (n < 8) { 
            fprintf(ferror, "%s", line); 
            continue; 
        }

        // Limpeza de aspas e espaços
        remove_quotes(code); remove_spc(code);
        remove_quotes(name); remove_spc(name);
        remove_quotes(city); remove_spc(city);
        remove_quotes(country); remove_spc(country);
        remove_quotes(lat_str); remove_spc(lat_str);
        remove_quotes(long_str); remove_spc(long_str);
        remove_quotes(icao); remove_spc(icao);
        remove_quotes(type); remove_spc(type);

        // Validação sintática
        if (!validate_airport_code(code) ||
            !validate_latitude_longitude(lat_str, long_str) || 
            !validate_airport_type(type) ||
            strlen(name) == 0) {
            fprintf(ferror, "%s", line);
            continue;
        }

        // Conversão para double
        double latitude = strtod(lat_str, NULL);
        double longitude = strtod(long_str, NULL);

        // Criação do objeto Airport
        Airport a = create_airport(code, name, city, country, latitude, longitude, icao, type);
        if (!a) {
            fprintf(stderr, "create_airport failed\n"); 
            fprintf(ferror, "%s", line); 
            continue; 
        } 
    
        AirportsManager am = dataset_get_airports(d);
        airports_manager_add(am, a);
    }

    fclose(f);
    fclose(ferror);
}