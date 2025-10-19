
#include <stdbool.h> 

// 5.3. Data Files Validation (.csv)

// Set of validations to be performed on received CSV format data. 
// The validations to be performed are of two types: syntactic and logical.

// syntax
bool validate_date();
bool validate_datetime();
bool validate_email();
bool validate_airport_code();
bool validate_gender();
bool validate_latitude_longitude();
bool validate_flight_id();
bool validate_reservation_id();
bool validate_document_number();
bool validate_airport_type();
bool validate_csv_lists();

// flight logic: In addition to syntactic errors, there may be certain logical inconsistencies.
bool validate_flight_logic();

