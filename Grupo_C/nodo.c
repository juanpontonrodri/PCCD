#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INSTR_LENGTH 1000

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s [node_id] [instrucciones] [tiempos]\n", argv[0]);
        return -1;
    }

    int node_id = atoi(argv[1]);
    char* instrucciones = argv[2];
    char* tiempos = argv[3];

    // Array para almacenar las instrucciones como enteros.
    int instr_arr[MAX_INSTR_LENGTH];
    int instr_count = 0;

    // Dividir la cadena de instrucciones en subcadenas separadas por comas y convertirlas en enteros.
    char* token = strtok(instrucciones, ",");
    while (token != NULL && instr_count < MAX_INSTR_LENGTH) {
        int instr = atoi(token);
        instr_arr[instr_count++] = instr;
        token = strtok(NULL, ",");
    }

    // Array para almacenar los tiempos como enteros.
    int tiempos_arr[MAX_INSTR_LENGTH];
    int tiempos_count = 0;

    // Dividir la cadena de tiempos en subcadenas separadas por comas y convertirlas en enteros.
    token = strtok(tiempos, ",");
    while (token != NULL && tiempos_count < MAX_INSTR_LENGTH) {
        int tiempo = atoi(token);
        tiempos_arr[tiempos_count++] = tiempo;
        token = strtok(NULL, ",");
    }

    // Mostrar los resultados.
    printf("ID del nodo: %d\n", node_id);
    printf("Instrucciones: ");
    for (int i = 0; i < instr_count; i++) {
        printf("%d ", instr_arr[i]);
    }
    printf("\nTiempos: ");
    for (int i = 0; i < tiempos_count; i++) {
        printf("%d ", tiempos_arr[i]);
    }
    printf("\n");

    return 0;
}
