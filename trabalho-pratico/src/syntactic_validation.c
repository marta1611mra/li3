#include <stdio.h>
#include <stdbool.h>

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

// validate the email int the format username@domínio

bool validate_email();
bool validate_airport_code();
bool validate_gender();
bool validate_latitude_longitude();
bool validate_flight_id();
bool validate_reservation_id();
bool validate_document_number();
bool validate_airport_type();
bool validate_csv_lists();
