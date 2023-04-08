#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <math.h> // para fmax()
// seamforos para proteger vairabless

int cola(int id);
void *proc_receptor(void *);

struct Testigo
{
    long mtype;
    int IDNodoOrigen;
    int atendidas_testigo[100];
};

struct PeticionTestigo
{
    long mtype;
    int IDNodoOrigen;
    int numero_peticion;
};

int id_cola;
int mi_peticion = 0;
int mi_id;
int vector_peticiones[100] = {0}, vector_atendidas[100] = {0}, dentro = 0, testigo = 1; // ->0 los otros nodos

sem_t sem_vector_peticiones, sem_vector_atendidas, sem_dentro, sem_testigo;

int main(int argc, char *argv[])
{
    int num_nodos = 2;

    sem_init(&sem_vector_peticiones, 0, 1); // los valores de aquí no son finales
    sem_init(&sem_vector_atendidas, 0, 1);  // los valores de aquí no son finales
    sem_init(&sem_dentro, 0, 1);            // los valores de aquí no son finales
    sem_init(&sem_testigo, 0, 1);           // los valores de aquí no son finales

    struct Testigo Testigo;
    struct PeticionTestigo PeticionTestigo;

    pthread_t receptor;

    int id_nodo_sig = 0, opcion, estado;
    char cadena[10];

    mi_id = atoi(argv[1]);
    sem_wait(&sem_testigo);
    testigo = atoi(argv[2]);
    sem_post(&sem_testigo);

    id_cola = cola(mi_id);

    pthread_create(&receptor, NULL, proc_receptor, NULL);
    while (1)
    {
        while (testigo)
        {
            sleep(1);
        }
        printf("\n1.Pedir testigo");
        printf("\n2.Salir.\n");
        fgets(cadena, 10, stdin);
        opcion = atoi(cadena);
        switch (opcion)
        {
        case 1:
            printf("Pidiendo testigo\n");
            if (!testigo)
            {

                mi_peticion = mi_peticion + 1;

                for (int i = 0; i < num_nodos; i++)
                {
                    if (i == mi_id)
                        continue;

                    int id_cola_otro = cola(3);

                    PeticionTestigo.mtype = 0;
                    PeticionTestigo.IDNodoOrigen = mi_id;
                    PeticionTestigo.numero_peticion = mi_peticion;

                    sem_wait(&sem_vector_peticiones);
                    estado = msgsnd(id_cola_otro, &PeticionTestigo, sizeof(struct PeticionTestigo) - sizeof(long), 0);
                    sem_post(&sem_vector_peticiones);

                    if (estado == -1)
                    {
                        perror("msgsnd buscanod testigo");
                        exit(EXIT_FAILURE);
                    }
                    printf("Peticion enviada: %d\n", i);
                }

                estado = msgrcv(id_cola, &Testigo, sizeof(struct Testigo) - sizeof(long), 1, 0); // con el ultimo 0 es modo bloqueante si en vez del 0 pones reciebe no wait no se queda suspendido si no hay mensaje

                if (estado == -1)
                {
                    perror("msgrcv");
                    exit(EXIT_FAILURE);
                }
                printf("He recibido el testigo \n");

                sem_wait(&sem_testigo);
                testigo = 1;
                sem_post(&sem_testigo);
            }
            sem_wait(&sem_dentro);
            dentro = 1;
            sem_post(&sem_dentro);
            printf("Entrando en la SC\n");
            sleep(2); // SC
            printf("Saliendo de la SC\n");
            sem_wait(&sem_vector_atendidas);
            vector_atendidas[mi_id] = mi_peticion;
            sem_post(&sem_vector_atendidas);

            sem_wait(&sem_dentro);
            dentro = 0;

            sem_post(&sem_dentro);

            if (id_nodo_sig != 0)

                if (vector_peticiones[id_nodo_sig] > vector_atendidas[id_nodo_sig])
                {

                    Testigo.mtype = 1;
                    Testigo.IDNodoOrigen = mi_id;
                    Testigo.atendidas_testigo[mi_id] = mi_peticion;
                    int id_cola_sig = cola(id_nodo_sig);
                    Testigo.mtype = 1;
                    Testigo.atendidas_testigo[mi_id] = mi_peticion;
                    Testigo.IDNodoOrigen = mi_id;
                    estado = msgsnd(id_cola_sig, &Testigo, sizeof(struct Testigo) - sizeof(long), 0);
                    if (estado == -1)
                    {
                        perror("msgsnd");
                        exit(EXIT_FAILURE);
                    }
                    printf("Testigo enviado: %d\n", id_nodo_sig);
                }

            break;
        case 2:
            exit(0);
            break;
        default:
            printf("\nOpción no válida\n");
            break;
        }
    }
}

void *proc_receptor(void *)
{
    int id_nodo_origen = 0, num_peticion_origen = 0, estado;
    struct Testigo Testigo;
    struct PeticionTestigo PeticionTestigo;
    while (1)
    {

        estado = msgrcv(id_cola, &PeticionTestigo, sizeof(struct PeticionTestigo) - sizeof(long), 0, 0); // con el ultimo 0 es modo bloqueante si en vez del 0 pones reciebe no wait no se queda suspendido si no hay mensaje

        if (estado == -1)
        {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        id_nodo_origen = PeticionTestigo.IDNodoOrigen;
        num_peticion_origen = PeticionTestigo.numero_peticion;

        sem_wait(&sem_vector_peticiones);
        vector_peticiones[id_nodo_origen] = fmax(vector_peticiones[id_nodo_origen], num_peticion_origen);
        sem_post(&sem_vector_peticiones);

        if (testigo && (!dentro) && (vector_peticiones[id_nodo_origen] > vector_atendidas[id_nodo_origen]))
        {
            int id_cola_otro = cola(id_nodo_origen);
            Testigo.mtype = 1;
            Testigo.atendidas_testigo[mi_id] = mi_peticion;
            Testigo.IDNodoOrigen = mi_id;
            estado = msgsnd(id_cola_otro, &Testigo, sizeof(struct Testigo) - sizeof(long), 0);
            if (estado == -1)
            {
                perror("msgsnd");
                exit(EXIT_FAILURE);
            }
            printf("Testigo enviado: %d\n", id_nodo_origen);
        }
    }
}

int cola(int id)
{
    key_t clave;
    clave = ftok("./", id);
    if (clave == (key_t)-1)
    {
        printf("Error al obtener la clave.\n");
        exit(0);
    }
    int id_cola = msgget(clave, 0777 | IPC_CREAT);
    if (id_cola == -1)
    {
        perror("msgget");
        exit(-1);
    }
    return id_cola;
}