#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Para fork y execl


#define MAX_LINE_LENGTH 1000
#define MAX_PROCESOS 100

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s [filename]\n", argv[0]);
        return -1;
    }

    char *filename = argv[1];

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file %s\n", filename);
        return -1;
    }

    // Leer la primera línea del archivo para obtener el nodo mínimo y máximo.
    int nodo_minimo, nodo_maximo;
    if (fscanf(fp, "%d %d", &nodo_minimo, &nodo_maximo) != 2) {
        printf("Error: La primera línea del archivo debe contener dos números enteros.\n");
        return -1;
    }

    // Matrices para almacenar información de procesos y tiempos.
    char instr_procesos[MAX_PROCESOS][MAX_LINE_LENGTH];
    char tiempos_procesos[MAX_PROCESOS][MAX_LINE_LENGTH];

    int i = 0;
    while (!feof(fp)) {
        char line[MAX_LINE_LENGTH];
        if (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
            // Saltar líneas vacías.
            if (line[0] == '\n') {
                continue;
            }

            if (i % 2 == 0) {
                // Almacenar la línea de información del proceso.
                if (sscanf(line, "%[^\n]", instr_procesos[i/2]) != 1) {
                    printf("Error: no se pudo leer la información del proceso en la línea %d.\n", i+1);
                    return -1;
                }
            } else {
                // Almacenar la línea de tiempo del proceso.
                if (sscanf(line, "%[^\n]", tiempos_procesos[i/2]) != 1) {
                    printf("Error: no se pudo leer la información de tiempo en la línea %d.\n", i+1);
                    return -1;
                }
            }
            i++;
        }
    }

    fclose(fp);

    // Verificar que se hayan leído la misma cantidad de líneas de información de procesos y tiempos.
    if (i % 2 != 0) {
        printf("Error: número impar de líneas en el archivo.\n");
        return -1;
    }

    printf("nodo_minimo = %d, nodo_maximo = %d\n", nodo_minimo, nodo_maximo);
    printf("Información de procesos:\n");
    for (int j = 0; j < i/2; j++) {
        printf("%s\n", instr_procesos[j]);
    }
    printf("Tiempos de procesos:\n");
    for (int j = 0; j < i/2 ; j++) {
        printf("%s\n", tiempos_procesos[j]);
    }


        // Bucle para crear procesos y ejecutar el programa "nodo".
    for (int j = nodo_minimo; j <= nodo_maximo; j++) {
        pid_t pid = fork();

        if (pid == -1) {
            // Error al crear proceso hijo.
            printf("Error al crear proceso hijo.\n");
            return -1;
        } else if (pid == 0) {
            // Proceso hijo.

            // Convertir el valor de nodo actual a una cadena de caracteres.
            char nodo_str[MAX_LINE_LENGTH];
            snprintf(nodo_str, MAX_LINE_LENGTH, "%d", j);

            // Ejecutar el programa "nodo" utilizando execl().
            execl("./nodo", "nodo", nodo_str, instr_procesos[j - nodo_minimo], tiempos_procesos[j - nodo_minimo], NULL);

            // Si llegamos hasta aquí, significa que hubo un error al ejecutar execl().
            printf("Error al ejecutar el programa \"nodo\" para el nodo %d.\n", j);
            return -1;
        }
    }

    // Código del proceso padre.
    return 0;
}




