#include <stdio.h>
#include <stdbool.h>
#include <ctype.h> // for the function to check if its alphanumeric
#include <string.h>

// validate the date in the format yyyy-mm-dd
bool validate_date(const char *date){
    int y,m,d;
    if (!date || strlen (date) != 10) return false; // making sure the length is 10 (yyyy-mm-dd)
    if (sscanf(date,"%4d-%2d-%2d",&y,&m,&d)!=3) return false; // making sure it reads the year,month,day 
    if (m<1 || m>12 || d <1 || d>31) return false; // range of possible days and months
    if (y > 2025 || (y == 2025 && m > 9)) return false; // it can't be more recent than 2025/09/30
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            if (date[i] != '-') return false; // making sure '-' it's used
        } else {
            if (date[i] < '0' || date[i] > '9') return false; // making sure the numbers are between 0 and 9
        }
    };
    return true;
}

// validate the datetime in the format aaaa-mm-dd hh:mm
bool validate_datetime(const char *datetime){
    int y, m, d, h, min; 
    if (!datetime || strlen(datetime) < 16) return false; // making sure the length is 16 (aaaa-mm-dd hh:mm)
    if (sscanf(datetime, "%4d-%2d-%2d %2d:%2d", &y, &m, &d, &h, &min) != 5) return false; // making sure it reads the year,month,day, hours and minutes
    if (m< 1 || m> 12 || d < 1 || d > 31) return false; // range of possible days and months
    if (h< 0 || h> 23 || min < 0 || min > 59) return false; // range for minutes and hours
    if (y > 2025 || (y == 2025 && m > 9)) return false; // it can't be more recent than 2025/09/30
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            if (datetime[i] != '-') return false; // making sure '-' it's used
        if (i ==12){
            if (datetime[i] != ':') return false; // making sure ':' it's used
        }
        } else {
            if (datetime[i] < '0' || datetime[i] > '9') return false; // making sure the numbers are between 0 and 9
        }
    };
    return true;
}

// validate the email int the format username@domain ( username@<lstring>.<rstring>)
bool validate_email(const char *email) {

    // username and @
    if (!email) return false; 
    bool has_arroba = false;    
    int arroba_pos = -1, i = 0;            
    while (email[i] != '\0') {
        char c = email[i]; 
        if (!(isalnum(c) || c == '.' || c == '@')) return false;
        if (c == '@') {
            if (has_arroba) return false;
            has_arroba = true; arroba_pos = i; 
            if (i == 0) return false; // it cant start with @
        }
        i++;}
    if (!has_arroba) return false; // make sure it has a @

    // Domain part before the dot
    const char *domain = email + arroba_pos + 1;
    if (*domain == '\0') return false; // it means there is nothing after '@'
    const char *dot = NULL;
    for (int j = 0; domain[j] != '\0'; j++) {
        if (domain[j] == '.') 
        dot = domain + j; // keep the position where "." it's located
    }
    if (!dot) return false; // must have a "."
    if ( dot == domain || *(dot + 1) == '\0') return false; // it can't be the first or last character of domain

    //Domain part after the dot (ex: .com)
    int finalprt_length = 0;
    const char *finalprt = dot + 1;

    while (*finalprt != '\0') {
        if (!isalpha(*finalprt)) return false; // must be only letters
        finalprt_length++; 
        finalprt++;
    }
    if (finalprt_length < 2 || finalprt_length > 3) return false; // Must be 2–3 letters 
    return true; // if it passes
}

// validate de IATA code 
bool validate_airport_code( const char *code){
    if (!code || strlen(code) != 3) return false; // IATA code must be 3 letters
    for (int i = 0; i < 3; i++){
        if (code[i] < 'A' || code[i] > 'Z') return false; // letters must be from A to Z
    }
    return true;
}

// validate the gender of the passenger (M,F or O)
bool validate_gender(const char *gender){
    if (!gender) return false;
    return (strcmp(gender, "M") == 0 || strcmp(gender, "F") == 0 || strcmp(gender, "O") == 0); 
    // why strcmp? it compares with the letter to check, if it's the same it returns a 0
    // that's why we use the comparison ==0;
}

// validates the latitude and longitude 
bool validate_latitude_longitude(const char *latitude,const char *longitude){
    double la, lo; // to save the latitude and longitude
    if (sscanf(latitude, "%lf", &la) != 1 || sscanf(longitude, "%lf", &lo) != 1) return false; // it checks if its read the values
    return (la >= -90 && la <= 90 && lo >= -180 && lo <= 180); // it checks the range of the latitude and longitude
}

// validates the flight id that has the form ccddddd
bool validate_flight_id(const char *id){
    if (!id || strlen(id) != 7) return false; // it must have 7 digits
    return isupper(id[0]) && isupper(id[1]) &&  // the first 2 digits must be letters
    isdigit(id[2]) && isdigit(id[3]) && isdigit(id[4]) && isdigit(id[5]) && isdigit(id[6]); // the rest must be numbers
}

// validates the reservation id that has the form Rnnnnnnnnn
bool validate_reservation_id(const char *id){
    if (!id) return false;
    if (!(id[0] == 'R')) return false;
    for (int i = 1; i < 10; i++) 
    if (!isdigit(id[i])) return false; 
    return true;
}

// validates the document number that has the form nnnnnnnnn
bool validate_document_number(const char *doc_num){
    if (!doc_num || strlen (doc_num)!= 9) return false;
    for (int i = 0; i < 9; i++) 
    if (!isdigit(doc_num[i])) return false; 
    return true; 
}

// validates the airport type 
bool validate_airport_type(const char *type) {
    const char *valid[] = {"small_airport", "medium_airport", "large_airport", "heliport", "seaplane_base"};
    // array with valid airport types
    for (int i = 0; i < 5; i++)
        if (strcmp(type, valid[i]) == 0) return true; // compares the type with the ones in the valid array
    return false;
}

// validates the csv lists: starts with '[' and ends with ']'
bool validate_lista_csv(const char *list) {
    if (!list) return false;
    int length = strlen(list);
    if (length < 2) return false; // Must be at least 2 because of "[]"
    if (list[0] != '[' || list[length - 1] != ']') return false; // Must start with '[' and end with ']'
    return true;
}
