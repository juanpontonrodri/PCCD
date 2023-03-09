#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

// Adresses de los semaforos
sem_t global; // este determina el numero maximo de lectores simulatenos

sem_t escribir[5];
sem_t salir[5];

void *start_routine(int *i)
{
    while (1)
    {
        printf("[Escritor %i] -> Esperando a intentar escribir...\n", (int)i);
        sem_wait(&escribir[(int)i]);

        printf("[Escritor %i] -> Intentando escribir...\n", (int)i);
        // esperamos a que hay sitio
        sem_wait(&global);

        printf("[Escritor %i] -> Escribiendo...\n", (int)i);
        sem_wait(&salir[(int)i]);

        printf("[Escritor %i] -> Fin escritura\n", (int)i);
        // hacemos sitio de nuevo
        sem_post(&global);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{

    int N3 = atoi(argv[1]);

    // creao los adresses de los hilos para cada lector
    pthread_t hilo[N3];
    // arranco el semaforo global con 1
    sem_init(&global, 0, 1);
    // inciializo los hilos
    for (int i = 0; i < N3; i++)
    {
        if (pthread_create(&hilo[i], NULL, (void *)start_routine, (void *)i + 1) != 0)
        {
            printf("error");
            exit(-3);
        }
        else
        {
            // inicio semaforos con valor inical 0( y el segunod argumento signiifca q se comparte entre hilos)
            sem_init(&escribir[i], 0, 0);
            sem_init(&salir[i], 0, 0);
        }
    }

    while (1)
    {
        printf("introduzca 1 2 o 3:\n");
        printf("Intentar escribir\n");
        printf("Finalizar escritura\n");
        printf("Salir\n");

        int opcion, numero_lector;
        scanf("%i", &opcion);
        switch (opcion)
        {
        case 1:
            printf("Introduzca el numero del lector (del 1 al %i)\n", N3);
            scanf("%i", &numero_lector);
            // incremetno el semaforo correspondiente al numero del lector
            sem_post(&escribir[numero_lector]);
            break;
        case 2:
            printf("Introduzca el numero del lector (del 1 al %i)\n", N3);
            scanf("%i", &numero_lector);
            sem_post(&salir[numero_lector]);
            break;
        case 3:
            return 0;
        }
    }
}
