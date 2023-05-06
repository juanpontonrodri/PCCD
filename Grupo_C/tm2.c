#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_STR_LEN 100
#define MAX_NUM_REGS 10000

char* generar_registro(char* pid, int tipo_proceso, struct timeval tiempo1, struct timeval tiempo2) {
    // Crear un array para almacenar la cadena de registro
    char registro_str[MAX_STR_LEN * 4];

    // Convertir los valores de tiempo a estructuras de tiempo
    struct tm tiempo1_tm = *localtime(&tiempo1.tv_sec);
    struct tm tiempo2_tm = *localtime(&tiempo2.tv_sec);

    // Crear cadenas de caracteres formateadas para las horas
    char hora1_str[MAX_STR_LEN];
    char hora2_str[MAX_STR_LEN];
    strftime(hora1_str, MAX_STR_LEN, "%Y-%m-%d %H:%M:%S", &tiempo1_tm);
    strftime(hora2_str, MAX_STR_LEN, "%Y-%m-%d %H:%M:%S", &tiempo2_tm);

    // Crear cadena de registro con formato "tipo de proceso;pid;hora1;hora2;diferencia"
    char tipo_proceso_str[MAX_STR_LEN];
    switch (tipo_proceso) {
        case 1:
            strcpy(tipo_proceso_str, "ANULACION");
            break;
        case 2:
            strcpy(tipo_proceso_str, "PAGO");
            break;
        case 3:
            strcpy(tipo_proceso_str, "RESERVA/ANULACION");
            break;
        case 4:
            strcpy(tipo_proceso_str, "CONSULTA");
            break;
        default:
            sprintf(tipo_proceso_str, "%d", tipo_proceso);
            break;
    }

    sprintf(registro_str, "%s;%s;%s.%06ld;%s.%06ld;%ld",
        tipo_proceso_str, pid,
        hora1_str, (long) tiempo1.tv_usec,
        hora2_str, (long) tiempo2.tv_usec,
        (long)(tiempo2.tv_sec - tiempo1.tv_sec)*1000000L + (long)(tiempo2.tv_usec - tiempo1.tv_usec));

    // Asignar memoria para la cadena de registro y copiar la cadena generada
    char* registro = malloc(strlen(registro_str) + 1);
    strcpy(registro, registro_str);

    // Devolver la cadena de registro
    return registro;
}



int main() {
    // Crear un array de strings de tamaño MAX_NUM_REGS
    char** registros = malloc(sizeof(char*) * MAX_NUM_REGS);

    // Inicializar valores de ejemplo
    
    
    struct timeval tiempo1, tiempo2;
    

    
    // Generar y almacenar registros adicionales
    for (int i = 0; i < MAX_NUM_REGS; i++) {
        int tipo_proceso = rand() % 4 + 1;
        struct timeval tiempo1, tiempo2;
        gettimeofday(&tiempo1, NULL);
        //usleep(rand() % 200);
        gettimeofday(&tiempo2, NULL);
        pthread_t pid = pthread_self();
        char hilo_str[20];
        snprintf(hilo_str, sizeof(hilo_str), "%ld", (long)pid);
        char* registro = generar_registro(hilo_str, tipo_proceso, tiempo1, tiempo2);
        registros[i] = registro;
    }

     // Escribir todos los registros en el archivo de log
    FILE* log_file = fopen("registros.log", "w");
    if (log_file == NULL) {
        printf("Error al abrir archivo de log\n");
        exit(1);
    }

    for (int i = 0; i < MAX_NUM_REGS; i++) {
        fprintf(log_file, "%s\n", registros[i]);
    }

    fclose(log_file);

    // Liberar memoria asignada para cada cadena de registro y para el array de registros
    for (int i = 0; i < MAX_NUM_REGS; i++) {
        free(registros[i]);
    }
    free(registros);

    return 0;
}