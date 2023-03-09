#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

// Adresses de los semaforos
sem_t global; // este determina el numero maximo de lectores simulatenos

sem_t leer[5];
sem_t salir[5];

void *start_routine(int *i)
{
    while (1)
    {
        printf("[Lector %i] -> Esperando a intentar leer...\n", (int)i);
        sem_wait(&leer[(int)i]);

        printf("[Lector %i] -> Intentando leer...\n", (int)i);
        // esperamos a que hay sitio
        sem_wait(&global);

        printf("[Lector %i] -> Leyendo...\n", (int)i);
        sem_wait(&salir[(int)i]);

        printf("[Lector %i] -> Fin lectura\n", (int)i);
        // hacemos sitio de nuevo
        sem_post(&global);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{

    int N1 = atoi(argv[1]);
    int N2 = atoi(argv[2]);
    // creao los adresses de los hilos para cada lector
    pthread_t hilo[N1];
    // arranco el semaforo global con valor maximo el numeor de lectores simultaneos
    sem_init(&global, 0, N2);
    // inciializo los hilos
    for (int i = 0; i < N1; i++)
    {
        if (pthread_create(&hilo[i], NULL, (void *)start_routine, (void *)i + 1) != 0)
        {
            printf("error");
            exit(-3);
        }
        else
        {
            // inicio semaforos con valor inical 0( y el segunod argumento signiifca q se comparte entre hilos)
            sem_init(&leer[i], 0, 0);
            sem_init(&salir[i], 0, 0);
        }
    }

    while (1)
    {
        printf("introduzca 1 2 o 3:\n");
        printf("Intentar leer\n");
        printf("Finalizar leer\n");
        printf("Salir\n");

        int opcion, numero_lector;
        scanf("%i", &opcion);
        switch (opcion)
        {
        case 1:
            printf("Introduzca el numero del lector (del 1 al %i)\n", N1);
            scanf("%i", &numero_lector);
            // incremetno el semaforo correspondiente al numero del lector
            sem_post(&leer[numero_lector]);
            break;
        case 2:
            printf("Introduzca el numero del lector (del 1 al %i)\n", N1);
            scanf("%i", &numero_lector);
            sem_post(&salir[numero_lector]);
            break;
        case 3:
            return 0;
        }
    }
}
