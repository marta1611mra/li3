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

    char line[2048];
    while (fgets(line, sizeof(line), f)) {
        char document_id[11] = "", first_name[31] = "", last_name[31] = "", dob[12] = "";
        char nationality[21] = "", gender[11] = "", email[51] = "", phone[16] = "", address[51] = "", photo[4096] = "";

        int n = sscanf(line,
                "\"%10[^\"]\",\"%30[^\"]\",\"%30[^\"]\",\"%10[^\"]\",\"%20[^\"]\",\"%10[^\"]\",\"%50[^\"]\",\"%15[^\"]\",\"%50[^\"]\",\"%4095[^\"]\"",
                document_id, first_name, last_name, dob, nationality,
                gender, email, phone, address, photo);


        if (n < 10) { fprintf(ferror, "%s", line); continue; }

        remove_quotes(document_id); remove_spc(document_id);
        remove_quotes(first_name); remove_spc(first_name);
        remove_quotes(last_name); remove_spc(last_name);
        remove_quotes(dob); remove_spc(dob);
        remove_quotes(nationality); remove_spc(nationality);
        remove_quotes(gender); remove_spc(gender);
        remove_quotes(email); remove_spc(email);
        remove_quotes(phone); remove_spc(phone);
        remove_quotes(address); remove_spc(address);
        remove_quotes(photo); remove_spc(photo);

        if (!validate_email(email) ||
            !validate_document_number(document_id) ||
            !validate_gender(gender) ||
            !validate_date(dob)) {
            fprintf(ferror, "%s", line);
            continue;
        }

        Passenger p = create_passenger(document_id, first_name, last_name, dob,
                                       nationality, gender, email, phone, address, photo);
        if (!p) { fprintf(stderr, "create_passenger failed"); fprintf(ferror, "%s", line); continue; }

        PassengersManager pm = dataset_get_passengers(d);
        passengers_manager_add(pm, p);
        }

    fclose(f);
    fclose(ferror);
}
