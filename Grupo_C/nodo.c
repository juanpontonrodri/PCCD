#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INSTR_LENGTH 1000

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Usage: %s [nodo_minimo] [nodo_maximo] [node_id] [instrucciones] [tiempos]\n", argv[0]);
        return -1;
    }

    int nodo_minimo = atoi(argv[1]);
    int nodo_maximo = atoi(argv[2]);
    int node_id = atoi(argv[3]);
    char* instrucciones = argv[4];
    char* tiempos = argv[5];

    int longitud_instrucciones = 0;
    // Array para almacenar las instrucciones como enteros.
    int instr_arr[MAX_INSTR_LENGTH];
    int instr_count = 0;

    // Dividir la cadena de instrucciones en subcadenas separadas por comas y convertirlas en enteros.
    char* token = strtok(instrucciones, ",");
    while (token != NULL && instr_count < MAX_INSTR_LENGTH) {
        // Omitir los espacios en blanco antes del número.
        while (*token== ' ') {
            token++;
        }
        int instr = atoi(token);
        longitud_instrucciones++;
        instr_arr[instr_count++] = instr;
        token = strtok(NULL, ",");
    }

    // Array para almacenar los tiempos como enteros.
    int tiempos_arr[MAX_INSTR_LENGTH];
    int tiempos_count = 0;

    // Dividir la cadena de tiempos en subcadenas separadas por comas y convertirlas en enteros.
    char* token_tiempo = strtok(tiempos, ",");
    while (token_tiempo != NULL && tiempos_count < MAX_INSTR_LENGTH) {
        // Omitir los espacios en blanco antes del número.
        while (*token_tiempo == ' ') {
            token_tiempo++;
        }
        int tiempo = atoi(token_tiempo);
        tiempos_arr[tiempos_count++] = tiempo;
        token_tiempo = strtok(NULL, ",");
    }


    // Mostrar los resultados.
    printf("ID del nodo: %d\n", node_id);
    printf("Nodo mínimo: %d\n", nodo_minimo);
    printf("Nodo máximo: %d\n", nodo_maximo);
    printf("Instrucciones: ");
    for (int i = 0; i < instr_count; i++) {
        printf("%d ", instr_arr[i]);
    }
    printf("\nTiempos: ");
    for (int i = 0; i < tiempos_count; i++) {
        printf("%d ", tiempos_arr[i]);
    }
    printf("\n");

    
  

    

    for (int j = 0; j < longitud_instrucciones; j += 4) {
        
        printf("j: %d\n", j);

        // Extraer cuatro elementos del array de instrucciones.
        int instr1 = instr_arr[j];
        int instr2 = instr_arr[j+1];
        int instr3 = instr_arr[j+2];
        int instr4 = instr_arr[j+3];

        // Extraer un elemento del array de tiempos.
        int tiempo = tiempos_arr[j/4];

        // Mostrar los resultados.
        printf("Instrucciones: %d %d %d %d, Tiempo: %d\n", instr1, instr2, instr3, instr4, tiempo);
    }

    

    return 0;
}
