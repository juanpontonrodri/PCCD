#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char caracter;
int contador = 0;

typedef struct
{
    int posicion;
    char *valor;
} estructura;

void *start_routine(estructura *parametros)
{

    printf("Posicion: %i\nValor: %s\n\n", parametros->posicion, parametros->valor);
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char *argv[])
{

    pthread_t hilo[argc];
    int retorno;
    estructura parametros[argc - 1];

    for (int i = 0; i < (argc - 1); i++)
    {

        parametros[i].posicion = i + 1;
        parametros[i].valor = argv[i + 1];

        retorno = pthread_create(&(hilo[i]), NULL, (void *)start_routine, (void *)&parametros[i]);

        if (retorno != 0)
        {
            printf("error");
            exit(-3);
        }
    }

    pthread_exit(NULL);
    return 0;
}
