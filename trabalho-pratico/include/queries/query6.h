#ifndef QUERY6_H
#define QUERY6_H

#include <stdio.h>
#include "dataset.h"
#include "managers/passengers_manager.h"
#include "managers/flights_manager.h"
#include "managers/reservations_manager.h"


void q6(Dataset d,
        const char *nationality,
        FILE *out);

#endif
