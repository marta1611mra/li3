CC = gcc
CFLAGS = -Wall -g -std=gnu11 \
         -Iinclude -Iinclude/entities -Iinclude/managers -Iinclude/queries \
         $(shell pkg-config --cflags glib-2.0)
LIBS = $(shell pkg-config --libs glib-2.0) -lm
PTHREAD_LIBS = -lpthread
NCURSES_LIBS = -lncurses

SRC_COMMON = \
    src/dataset.c \
    src/output_format.c \
    src/parser/parser.c \
    src/parser/parser_aircrafts.c \
    src/parser/parser_airports.c \
    src/parser/parser_flights.c \
    src/parser/parser_passengers.c \
    src/parser/parser_reservations.c \
    src/logical_validation.c \
    src/syntactic_validation.c \
    src/entities/aircrafts.c \
    src/entities/airports.c \
    src/entities/flights.c \
    src/entities/passengers.c \
    src/entities/reservations.c \
    src/managers/aircrafts_manager.c \
    src/managers/airports_manager.c \
    src/managers/flights_manager.c \
    src/managers/passengers_manager.c \
    src/managers/reservations_manager.c \
    src/queries/query1.c \
    src/queries/query2.c \
    src/queries/query3.c \
    src/queries/query4.c \
    src/queries/query5.c \
    src/queries/query6.c \
    src/queries/queries.c

OBJ_COMMON = $(SRC_COMMON:.c=.o)

# Target padrão compila apenas principal e testes (conforme enunciado)
all: programa-principal programa-testes

programa-principal: $(OBJ_COMMON) src/programa-principal.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

programa-testes: $(OBJ_COMMON) src/programa-testes.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Programa interativo com interface NCURSES (terminal)
programa-interativo: src/programa-interativo.c $(OBJ_COMMON)
	$(CC) $(CFLAGS) -o $@ $< $(OBJ_COMMON) $(LIBS) $(NCURSES_LIBS)

# Regra geral para compilar .c em .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f programa-principal programa-testes programa-interativo \
	      src/*.o src/entities/*.o src/managers/*.o src/queries/*.o src/parser/*.o

.PHONY: all clean