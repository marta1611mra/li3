#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "query2.h"
#include "dataset.h"
#include "aircrafts.h"      
#include "output_format.h"  

typedef struct {
    char code[16]; 
    int count;
} Q2Row;

static int cmp_q2(const void *a, const void *b) {
    const Q2Row *x = a;
    const Q2Row *y = b;

    if (y->count != x->count)
        return y->count - x->count;

    return strcmp(x->code, y->code);
}

void q2(Dataset d, int N, const char *manufacturer, FILE *out) {
    if (!d || !out || N <= 0) {
        return;
    }

    const char *key = (manufacturer && *manufacturer) ? manufacturer : "__ALL__";

    /* --- AQUI ESTAVA O ERRO --- */
    /* Antigo (Erro): GHashTable *table = g_hash_table_lookup(d->q2_index, key); */
    
    /* Novo (Correto): Usamos a função getter */
    GHashTable *index_q2 = dataset_get_q2_index(d); 
    if (!index_q2) return;

    GHashTable *table = g_hash_table_lookup(index_q2, key);
    /* -------------------------- */

    if (!table || g_hash_table_size(table) == 0) {
        return;
    }

    int size = g_hash_table_size(table);
    Q2Row *rows = malloc(size * sizeof(Q2Row));
    if (!rows) return;

    int i = 0;
    GHashTableIter iter;
    gpointer k, v;
    g_hash_table_iter_init(&iter, table);

    while (g_hash_table_iter_next(&iter, &k, &v)) {
        strncpy(rows[i].code, (char *)k, 15);
        rows[i].code[15] = '\0';
        rows[i].count = *(int *)v;
        i++;
    }

    qsort(rows, size, sizeof(Q2Row), cmp_q2);

    AircraftsManager am = dataset_get_aircrafts(d);
    char sep = get_output_separator(); 

    for (int j = 0; j < N && j < size; j++) {
        Aircraft a = aircrafts_manager_get(am, rows[j].code);
        
        if (a) {
            fprintf(out, "%s%c%s%c%s%c%d\n",
                    rows[j].code,
                    sep,
                    get_aircraft_manufacturer(a),
                    sep,
                    get_aircraft_model(a),
                    sep,
                    rows[j].count);
        }
    }

    free(rows);
}