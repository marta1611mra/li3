#ifndef AIRCRAFTS_H
#define AIRCRAFTS_H

typedef struct aircraft *Aircraft;

Aircraft create_aircraft(const char *id, const char *manufacturer, const char *model, const char *year, int capacity, int range);

void destroy_aircraft(Aircraft a);

#endif
