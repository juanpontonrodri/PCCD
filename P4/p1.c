#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int contador = 0;
char caracter;

void *start__routine(void *);

int main(int argc, char *argv[])
{
    pthread_t hilo;
    int retorno;

    retorno = pthread_create(&hilo, NULL, start__routine, NULL);

    if (retorno == 0)
    {

        while (caracter != 'q')
        {
            sleep(1);
            printf("contador: %i\n", contador);
        }
    }

    else
        printf("errror");
}

void *start__routine(void *arg)
{

    printf("introducir caracter");

    while (caracter != 'q')
    {
        caracter = getchar();
        if (caracter != '\n')
        {
            contador++;
        }
    }

    pthread_exit(NULL);

    return NULL;
}