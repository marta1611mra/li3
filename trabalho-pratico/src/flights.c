#include <stdio.h>

typedef enum {
    OnTime,
    Delayded,
    Cancelled
} flight_status;

struct flight{
    
    char flight_id[8];
    char departure[19];
    char actual_departure[19];
    char arrival[19];
    char actual_arrival[19];
    char gate[3];
    flight_status status;
    char origin[4];
    char destiny[4];
    char aircraft_id[15];
    char airline[40];
    char tracking_url[100];
};
