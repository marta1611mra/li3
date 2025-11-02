#include <stdbool.h> 

// 5.3. Data Files Validation (.csv)


// flight logic: In addition to syntactic errors, there may be certain logical inconsistencies.
bool validate_destination (Flight f);
static int compare_datetimes(const char *dt1, const char *dt2);
bool validate_arrival(Flight f);
bool validate_aircraft(Flight f, AircraftsManager am);
bool validate_status(Flight f);
bool logical_validate_document_number(Reservation r, Passenger p[], int N);
