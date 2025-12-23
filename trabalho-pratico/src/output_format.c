#include "output_format.h"

static char output_sep = ';';

void set_output_separator(char sep) {
    output_sep = sep;
}

char get_output_separator(void) {
    return output_sep;
}
