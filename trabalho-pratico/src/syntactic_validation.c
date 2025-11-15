#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h> // for the function to check if its alphanumeric
#include <string.h>

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
            if (!isdigit((unsigned char)date[i])) return false; // making sure the numbers are between 0 and 9
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
            if (datetime[i] != ' ') return false; // making sure ' ' it's used
        } else if (i == 13) {
            if (datetime[i] != ':') return false; // making sure ' ' it's used
        } else if (!isdigit((unsigned char)datetime[i])) return false; // making sure the numbers are between 0 and 9
    }
    return true;
}

// validate the year
bool validate_year(const char *year) {
    if (!year || strlen(year) != 4) return false;
    for (int i = 0; i < 4; i++)
        if (!isdigit((unsigned char)year[i])) return false;
    int y = atoi(year);
    if(y > 2025) return false;
    return true;
}

// validate the email int the format username@domain
bool validate_email(const char *email) {
    if (!email) return false;
    const char *at = strchr(email, '@');
    if (!at) return false;
    if (at == email) return false; // username empty

    // username part
    for (const char *p = email; p < at; p++) {
        char c = *p;
        if (!(c == '.' || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')))
            return false;
    }

    //domain part
    const char *domain = at + 1;
    const char *dot = strchr(domain, '.');
    if (!dot) return false;

    if (dot == domain) return false; // empty lstring

    int rlen = strlen(dot + 1);
    if (rlen != 2 && rlen != 3) return false;

    for (const char *p = domain; p < dot; p++)
        if (*p < 'a' || *p > 'z') return false;

    for (const char *p = dot + 1; *p; p++)
        if (*p < 'a' || *p > 'z') return false;

    return true;
}

// validate the IATA code 
bool validate_airport_code(const char *code) {
    return code && strlen(code) == 3
        && code[0] >= 'A' && code[0] <= 'Z'
        && code[1] >= 'A' && code[1] <= 'Z'
        && code[2] >= 'A' && code[2] <= 'Z';
}

// validate the gender of the passenger (M,F or O)
bool validate_gender(const char *gender){
    return gender && (strcmp(gender, "M") == 0 || strcmp(gender, "F") == 0 || strcmp(gender, "O") == 0);
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
    if (!id || strlen(id) != 7) return false;
    return (id[0] >= 'A' && id[0] <= 'Z') &&
           (id[1] >= 'A' && id[1] <= 'Z') &&
           isdigit(id[2]) && isdigit(id[3]) && isdigit(id[4]) &&
           isdigit(id[5]) && isdigit(id[6]);
}

// validates the reservation id that has the form Rnnnnnnnnn
bool validate_reservation_id(const char *id){
    if (!id || strlen(id) != 10) return false;
    if ((id[0] != 'R')) return false;
    for (int i = 1; i < 10; i++) 
        if (!isdigit((unsigned char)id[i])) return false; 
    return true;
}

// validates the document number that has the form nnnnnnnnn
bool validate_document_number(const char *doc_num){
    if (!doc_num || strlen (doc_num)!= 9) return false;
    for (int i = 0; i < 9; i++) 
        if (!isdigit((unsigned char)doc_num[i])) return false; 
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
bool validate_csv_lists(const char *s) {
    size_t n = strlen(s);
    return n >= 2 && s[0]=='[' && s[n-1]==']';
}

