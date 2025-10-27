#include <stdbool.h> 

// 5.3. Data Files Validation (.csv)


// flight logic: In addition to syntactic errors, there may be certain logical inconsistencies.
bool validate_destination (struct flight f);
bool validate_arrival(struct flight f);
bool validate_aircraft(struct flight f, struct aircraft a[], int N);
bool validate_status(struct flight f);
bool logical_validate_document_number(struct reservation r, struct passenger p[], int N);
