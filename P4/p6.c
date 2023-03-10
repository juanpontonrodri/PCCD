#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

sem_t global, EscStart[20], EscStop[20], LecStart[20], LecStop[20], lim_Esc, lim_Lect, variable, esc_quiere;

int N1, N2, N3;
int lectores = 0;
int esc_pend = 0;

void routine_escritor(int *param)
{

    while (1)
    {

        printf("[Escritor %d] Esperando a intentar escribir...\n", *param + 1);

        sem_wait(&EscStart[*param]);

        esc_pend++;

        printf("[Escritor %d]Intentando escribir... \n", *param + 1);
        sem_wait(&lim_Esc);
        sem_wait(&lim_Lect);

        esc_pend--;

        printf("[Escritor %d]Escribiendo ... \n", *param + 1);
        sem_wait(&EscStop[*param]);
        printf("[Escritor %d]Fin escritura \n", *param + 1);

        sem_post(&lim_Lect);
        sem_post(&lim_Esc);
    }
}

void routine_lector(int *param)
{

    while (1)
    {

        printf("[Lector %d] Esperando a intentar leer...\n", *param + 1);

        sem_wait(&LecStart[*param]);
        printf("[Lector %d]Intentando leer...\n", *param + 1);

        if (esc_pend != 0)
        {
            sem_wait(&lim_Esc);  // comprobamos si hay sitio para un escritor
            sem_wait(&lim_Lect); // si hay sitio para otra persona
            if (esc_pend != 0)
            {                         // si despues de saber que hay sitio sigue habiendo escriotres pendientes
                sem_post(&lim_Esc);   // habilitamos al escritor para q entre
                sem_post(&lim_Lect);  // habilitamos al lector para q entre
                while (esc_pend != 0) // mientras siga habiendo escriotres pendientes
                {
                    sem_post(&lim_Esc);
                    sem_wait(&lim_Esc);
                }
                printf("fuera while\n");
                break;
                printf("fuera while2\n");
            }
            else
            {
                printf("dentro del else\n");

                sem_post(&lim_Esc);
                sem_post(&lim_Lect);
            }
        }

        sem_wait(&lim_Lect);
        printf("[Lector %d]Leyendo ...\n", *param + 1);
        sem_wait(&LecStop[*param]);
        printf("[Lector %d]Fin lectura \n", *param + 1);

        sem_post(&lim_Lect);
    }
}

int main(int argc, char *argv[])
{

    N1 = atoi(argv[1]);
    N2 = atoi(argv[2]);
    N3 = atoi(argv[3]);

    // inicio los semaforos globales que limitan la concurrencia
    sem_init(&lim_Esc, 0, 1);   // limites de escritores
    sem_init(&lim_Lect, 0, N2); // limite de lectores
    sem_init(&variable, 0, 1);
    sem_init(&global, 0, 1);
    sem_init(&esc_quiere, 0, 0); // no se como implementar este, asi que de momento uso la varibale esc_pend

    char cadena[10];
    int opcion;
    pthread_t id_lect[N1];
    pthread_t id_Esc[N3];
    int id_l[N1];
    int id_e[N3];
    int lector, escritor;

    // creo todos los lectores

    for (int i = 0; i < N1; i++)
    {

        if (pthread_create(&id_lect[i], NULL, (void *)routine_lector, (void *)&id_l[i]) != 0)
            printf("ERROR CREANDO HILO\n");
        else
        {
            id_l[i] = i;
            // creo los semaforos de cada uno y luego los pongo a 0
            sem_init(&LecStart[i], 0, 1);
            sem_init(&LecStop[i], 0, 1);
            sem_wait(&LecStop[i]);
            sem_wait(&LecStart[i]);
        }
    }

    // creo los escirotroes
    for (int i = 0; i < N3; i++)
    {
        if ((pthread_create(&id_Esc[i], NULL, (void *)routine_escritor, (void *)&id_e[i]) != 0))
            printf("ERROR CREANDO HILO\n");
        else
        {
            id_e[i] = i;
            sem_init(&EscStart[i], 0, 1);
            sem_init(&EscStop[i], 0, 1);
            sem_wait(&EscStop[i]);
            sem_wait(&EscStart[i]);
        }
    }

    int input;
    while (1)
    {
        printf("\n1.Intentar leer");
        printf("\n2.Finalizar leer");
        printf("\n3.Intentar escribir");
        printf("\n4.Finalizar escribir.");
        printf("\n5.Salir.\n");
        fgets(cadena, 10, stdin);
        opcion = atoi(cadena);
        switch (opcion)
        {
        case 1:
            printf("\nIntroduzca el número del lector de 1 a %d\n", N1);
            fgets(cadena, 10, stdin);
            lector = atoi(cadena);
            sem_post(&LecStart[lector - 1]);
            break;
        case 2:
            printf("\nIntroduzca el número del lector de 1 a %d\n", N1);
            fgets(cadena, 10, stdin);
            lector = atoi(cadena);
            sem_post(&LecStop[lector - 1]);
            break;
        case 3:
            printf("\nIntroduzca el número del escritor de 1 a %d\n", N3);
            fgets(cadena, 10, stdin);
            escritor = atoi(cadena);
            sem_post(&EscStart[escritor - 1]);
            break;
        case 4:
            printf("\nItroduzca el número del escritor de 1 a %d\n", N3);
            fgets(cadena, 10, stdin);
            escritor = atoi(cadena);
            sem_post(&EscStop[escritor - 1]);
            break;
        case 5:
            return 0;
        default:
            break;
        }
    }
    return 0;
}
