#include "parser/parser.h"
#include "dataset.h"
#include "syntactic_validation.h"
#include "logical_validation.h"
#include "parser/parser_passengers.h"
#include "passengers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>


// Lê e processa o ficheiro passengers.csv 
// Remove parsing de campos não usados: phone, address, photo
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
    
    // Buffer de I/O maior
    setvbuf(f, NULL, _IOFBF, 65536);

    exist_result();
    FILE *ferror = fopen("resultados/passengers_errors.csv", "w");
    if (!ferror) { perror("resultados/passengers_errors.csv"); fclose(f); return; }

    wcsv_header(f, ferror);

    // Obter manager uma única vez fora do loop
    PassengersManager pm = dataset_get_passengers(d);

    char line[16384];  // Buffer maior
    
    while (fgets(line, sizeof(line), f)) {
        // Parsing apenas dos campos usados
        char document_id[16] = "";
        char first_name[128] = "";
        char last_name[128] = "";
        char dob[12] = "";
        char nationality[64] = "";
        char gender[4] = "";
        char email[128] = "";
        
        // Campos ignorados (lidos mas descartados)
        char phone[32] = "";
        char address[256] = "";
        char photo[512] = "";

        // Parsing com sscanf
        int n = sscanf(line,
            "\"%15[^\"]\",\"%127[^\"]\",\"%127[^\"]\",\"%11[^\"]\",\"%63[^\"]\","
            "\"%3[^\"]\",\"%127[^\"]\",\"%31[^\"]\",\"%255[^\"]\",\"%511[^\"]\"",
            document_id, first_name, last_name, dob, nationality,
            gender, email, phone, address, photo);

        if (n != 10) {
            fprintf(ferror, "%s", line);
            continue;
        }

        // Limpa apenas os campos usados
        remove_quotes(document_id); remove_spc(document_id);
        remove_quotes(first_name); remove_spc(first_name);
        remove_quotes(last_name); remove_spc(last_name);
        remove_quotes(dob); remove_spc(dob);
        remove_quotes(nationality); remove_spc(nationality);
        remove_quotes(gender); remove_spc(gender);
        remove_quotes(email); remove_spc(email);

        // Remove newlines
        document_id[strcspn(document_id, "\r\n")] = 0;
        first_name[strcspn(first_name, "\r\n")] = 0;
        last_name[strcspn(last_name, "\r\n")] = 0;
        dob[strcspn(dob, "\r\n")] = 0;
        nationality[strcspn(nationality, "\r\n")] = 0;
        gender[strcspn(gender, "\r\n")] = 0;
        email[strcspn(email, "\r\n")] = 0;

        // Validações na ordem mais eficiente
        if (!validate_gender(gender) ||
            !validate_document_number(document_id) ||
            !validate_date(dob) ||
            !validate_email(email)) {
            fprintf(ferror, "%s", line);
            continue;
        }

        // Passa strings vazias para campos que não são usados, poupando memória caso a struct os guarde
        Passenger p = create_passenger(document_id, first_name, last_name, dob,
                                       nationality, gender, email, 
                                       "", "", "");  // phone, address, photo vazios

        if (!p) {
            fprintf(ferror, "%s", line);
            continue;
        }

        passengers_manager_add(pm, p);
    }

    fclose(f);
    fclose(ferror);
}