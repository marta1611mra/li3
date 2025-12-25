#include "parser/parser.h"
#include "dataset.h"
#include "syntactic_validation.h"
#include "logical_validation.h"
#include "passengers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>


// Lê e processa o ficheiro passengers.csv.

void parse_passengers(Dataset d, const char *data_path) {
    char clean_path[512];
    strcpy(clean_path, data_path);
    size_t len = strlen(clean_path);
    while (len > 0 && clean_path[len - 1] == '/') {
    clean_path[len - 1] = '\0';
    len--;
}

    char path[1026];
    snprintf(path, sizeof(path), "%s/passengers.csv", clean_path);

    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return; }
    printf("A ler ficheiro: %s\n", path);

    exist_result();
    FILE *ferror = fopen("resultados/passengers_errors.csv", "w");
    if (!ferror) { perror("resultados/passengers_errors.csv"); fclose(f); return; }

    wcsv_header(f, ferror);

char line[40000];
    while (fgets(line, sizeof(line), f)) {
        char line_copy[40000];
        strcpy(line_copy, line);

        char *ptr = line;
        char *fields[10];
        int i = 0;

        while (i < 10 && (fields[i] = strsep(&ptr, ",")) != NULL) {
            i++;
        }

        if (i < 10) {
            fprintf(ferror, "%s", line_copy);
            continue;
        }

        for (int j = 0; j < 10; j++) {
            remove_quotes(fields[j]);
            remove_spc(fields[j]);
        }

        char *document_id = fields[0];
        char *first_name  = fields[1];
        char *last_name   = fields[2];
        char *dob         = fields[3];
        char *nationality = fields[4];
        char *gender      = fields[5];
        char *email       = fields[6];
        char *phone       = fields[7];
        char *address     = fields[8];
        char *photo       = fields[9];

        // Validação
        if (!validate_email(email) ||
            !validate_document_number(document_id) ||
            !validate_gender(gender) ||
            !validate_date(dob)) {
            fprintf(ferror, "%s", line_copy);
            continue;
        }

        Passenger p = create_passenger(document_id, first_name, last_name, dob,
                                       nationality, gender, email, phone, address, photo);
        
        if (!p) {
            fprintf(ferror, "%s", line_copy);
            continue;
        }

        passengers_manager_add(dataset_get_passengers(d), p);
    }

    fclose(f);
    fclose(ferror);
}
