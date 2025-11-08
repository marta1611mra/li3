CC = gcc
CFLAGS = -Wall -g -std=gnu11 \
         -Iinclude -Iinclude/entities -Iinclude/managers -Iinclude/queries \
         $(shell pkg-config --cflags glib-2.0)
LIBS = $(shell pkg-config --libs glib-2.0)

SRC_COMMON = src/dataset.c src/parser.c src/logical_validation.c src/syntactic_validation.c \
             src/entities/aircrafts.c src/entities/airports.c src/entities/flights.c \
             src/entities/passengers.c src/entities/reservations.c \
             src/managers/aircrafts_manager.c src/managers/airports_manager.c \
             src/managers/flights_manager.c src/managers/passengers_manager.c \
             src/managers/reservations_manager.c \
             src/queries/query2.c \
             src/queries/query3.c

all: programa-principal programa-testes

programa-principal: $(SRC_COMMON) src/programa-principal.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

programa-testes: $(SRC_COMMON) src/programa-testes.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f programa-principal programa-testes src/*.o src/entities/*.o src/managers/*.o
