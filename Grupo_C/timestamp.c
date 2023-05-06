#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h> // necesario para la función sleep()
#include <string.h>

#define MAX_BUFFER_SIZE 2700000


int get_timestamp(char* buffer, int max_size, char* custom_str) {
    // obtener el timestamp actual con precisión de microsegundos
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t t = tv.tv_sec;
    struct tm* tm_info = localtime(&t);
    char timestamp[256]; // aumentar el tamaño del buffer
    
    // Concatenar la cadena personalizada al timestamp
    strcpy(timestamp, custom_str);
    strcat(timestamp, " ");
    
    strftime(timestamp + strlen(custom_str) + 1, 256 - strlen(custom_str) - 1, "%Y-%m-%d %H:%M:%S", tm_info);
    sprintf(timestamp + strlen(timestamp), ".%06ld", tv.tv_usec);

    // escribir el timestamp en el buffer
    return snprintf(buffer, max_size, "%s\n", timestamp);
}

int main() {
    char buffer[MAX_BUFFER_SIZE];
    int i = 0;

    for (int j = 0; j < 5; j++) {
        int len = get_timestamp(buffer + i, MAX_BUFFER_SIZE - i,"PAGOS ");
        i += len;
        sleep(1);
    }

    FILE *fp = fopen("archivo.log", "w");
    if (fp == NULL) {
        perror("No se pudo abrir el archivo");
        exit(1);
    }

    // escribir el contenido del buffer en el archivo
    fprintf(fp, "%s", buffer);

    // cerrar el archivo
    fclose(fp);

    printf("Información printeada en el archivo.log\n");

    return 0;
}