#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h> // for the function to check if its alphanumeric
#include <string.h>

// validate the date in the format yyyy-mm-dd
bool validate_date(const char *date){
    int y,m,d;
    if (!date || strlen (date) != 10) return false; // making sure the length is 10 (yyyy-mm-dd)
    if (sscanf(date,"%4d-%2d-%2d",&y,&m,&d) != 3) return false; // making sure it reads the year,month,day
    if (m < 1 || m > 12 || d < 1 || d > 31) return false; // range of possible days and months
    if (y > 2025 || (y == 2025 && m > 9)) return false; // it can't be more recent than 2025/09/30
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            if (date[i] != '-') return false; // making sure '-' it's used
        } else {
            if (!isdigit(date[i])) return false; // making sure the numbers are between 0 and 9
        }
    }
    return true;
}

// validate the datetime in the format aaaa-mm-dd hh:mm
bool validate_datetime(const char *datetime){
    int y, m, d, h, min; 
    if (!datetime || strlen(datetime) != 16) return false; // making sure the length is 16 (aaaa-mm-dd hh:mm)
    if (sscanf(datetime, "%4d-%2d-%2d %2d:%2d", &y, &m, &d, &h, &min) != 5) return false; // making sure it reads the year,month,day, hours and minutes
    if (m < 1 || m > 12 || d < 1 || d > 31) return false; // range of possible days and months
    if (h < 0 || h > 23 || min < 0 || min > 59) return false; // range for minutes and hours
    if (y > 2025 || (y == 2025 && m > 9)) return false; // it can't be more recent than 2025/09/30
    for (int i = 0; i < 16; i++) {
        if (i == 4 || i == 7) {
            if (datetime[i] != '-') return false; // making sure '-' it's used
        } else if (i == 10) {
            if (datetime[i] != ' ') return false; // making sure ':' it's used
        } else if (i == 13) {
            if (datetime[i] != ':') return false;
        } else if (!isdigit(datetime[i])) return false; // making sure the numbers are between 0 and 9
    }
    return true;
}

bool validate_year(const char *year) {
    if (!year || strlen(year) != 4) return false;
    for (int i = 0; i < 4; i++)
        if (!isdigit(year[i])) return false;
    int y = atoi(year);
    if(y > 2025) return false;
    return true;
}

// validate the email int the format username@domain ( username@<lstring>.<rstring>)
bool validate_email(const char *email) {
if (!email) return false;
    int len = strlen(email);
    if (len < 6) return false;

    const char *at = strchr(email, '@');
    if (!at || at == email || at == email + len - 1) return false;

    // username part
    for (const char *p = email; p < at; p++)
        if (!(islower(*p) || isdigit(*p) || *p == '.' || *p=='-' || *p=='_')) return false;

    // domain part
    const char *dot = strrchr(at, '.');
    if (!dot || dot <= at + 1 || dot >= email + len - 2) return false;

    for (const char *p = at + 1; p < dot; p++)
        if (!islower(*p)) return false;

    const char *rstring = dot + 1;
    int rlen = strlen(rstring);
    if (rlen < 2 || rlen > 3) return false;
    for (int i = 0; i < rlen; i++)
        if (!islower(rstring[i])) return false;

    return true;
}

// validate de IATA code 
bool validate_airport_code( const char *code){
    if (!code || strlen(code) != 3) return false; // IATA code must be 3 letters
    for (int i = 0; i < 3; i++)
        if (code[i] < 'A' || code[i] > 'Z') return false; // letters must be from A to Z
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
bool validate_latitude(const char *lat) {
    if (!lat || !*lat) return false;
    double val; char extra;
    if (sscanf(lat, "%lf%c", &val, &extra) != 1) return false;
    if (val < -90.0 || val > 90.0) return false;
    return true;
}

bool validate_longitude(const char *lon) {
    if (!lon || !*lon) return false;
    double val; char extra;
    if (sscanf(lon, "%lf%c", &val, &extra) != 1) return false;
    if (val < -180.0 || val > 180.0) return false;
    return true;
}

bool validate_latitude_longitude(const char *latitude,const char *longitude){
    return validate_latitude(latitude) && validate_longitude(longitude);
}

// validates the flight id that has the form ccddddd
bool validate_flight_id(const char *id){
    if (!id || strlen(id) != 7) return false; // it must have 7 digits
    for (int i = 0; i < 2; i++)
        if (id[i] < 'A' || id[i] > 'Z') return false;
    for (int i = 2; i < 7; i++)
        if (!isdigit(id[i])) return false;
    return true;
}

// validates the reservation id that has the form Rnnnnnnnnn
bool validate_reservation_id(const char *id){
    if (!id) return false;
    if ((id[0] != 'R')) return false;
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
   if (!type) return false;
    return (!strcmp(type, "small_airport") ||
            !strcmp(type, "medium_airport") ||
            !strcmp(type, "large_airport") ||
            !strcmp(type, "heliport") ||
            !strcmp(type, "seaplane_base"));
}

// validates the csv lists: starts with '[' and ends with ']'
bool validate_csv_lists(const char *list) {
    if (!list) return false;
    int length = strlen(list);
    if (length < 2) return false; // Must be at least 2 because of "[]"
    if (list[0] != '[' || list[length - 1] != ']') return false; // Must start with '[' and end with ']'
    return true;
}
