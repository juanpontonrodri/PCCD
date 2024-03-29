#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <math.h> // para fmax()
#include <semaphore.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

#define MAX_SIZE 100 // Tamaño máximo de la cola
#define MAX_PROCESOS 10
#define MAX_NODOS 2
// tipos de mensajes:11 21 31 41 51 testigo
//  15 25 35 45 55 pedir testigo en ese proceso

int cola_entrada(int id);
int cola_distribucion(int id);
void *proc_receptor(void *);
void *proceso_main(int id_proceso);

struct Testigo
{
    long mtype;
    int IDProcesoDestino;
    int IDNodoOrigen; // sobra
    int atendidas_testigo[MAX_NODOS][MAX_PROCESOS];
};

#define MAX_SIZE 100 // Tamaño máximo de la cola

typedef struct PeticionTestigo
{
    long mtype;
    int IDNodoOrigen;
    int IDProcesoOrigen;
    int numero_peticion;
} PeticionTestigo;

typedef struct
{
    PeticionTestigo data[MAX_SIZE];
    int front;
    int rear;
} Queue;

// Función para inicializar la cola
void initQueue(Queue *queue)
{
    queue->front = 0;
    queue->rear = -1;
}

// Función para verificar si la cola está vacía
int isEmpty(Queue *queue)
{
    return (queue->rear < queue->front);
}

// Función para verificar si la cola está llena
int isFull(Queue *queue)
{
    return (queue->rear == MAX_SIZE - 1);
}

// Función para agregar un elemento a la cola
void enqueue(Queue *queue, PeticionTestigo value)
{
    if (isFull(queue))
    {
        printf("La cola está llena.\n");
        return;
    }
    queue->rear++;
    queue->data[queue->rear] = value;
}

// Función para eliminar un elemento de la cola
PeticionTestigo dequeue(Queue *queue)
{
    if (isEmpty(queue))
    {
        printf("La cola está vacía.\n");
        PeticionTestigo empty;
        empty.mtype = 0;
        return empty;
    }
    PeticionTestigo value = queue->data[queue->front];
    queue->front++;
    return value;
}

Queue cola_peticiones;

sem_t sem_testigo, sem_dentro, sem_vector_pet, sem_vector_aten, sem_flag_cola, sem_id_nodo_sig, sem_id_proceso_sig;
sem_t sem_inicial;
sem_t sem_procesos[MAX_PROCESOS];
int coger_testigo = 0;

int id_cola_distribucion, id_cola_entrada;
int mi_id;
int id_nodo_sig = 0, id_proceso_sig = 0, flag_cola = 0, num_nodos;
int vector_peticiones[MAX_NODOS][MAX_PROCESOS] = {0}, vector_atendidas[MAX_NODOS][MAX_PROCESOS] = {0}, dentro = 0, testigo = 0;

int main(int argc, char *argv[]) // argv[1] es el id del nodo argv[2] es el numero de nodos y argv[3] es el numero de procesos
{

    sem_init(&sem_testigo, 0, 1);
    sem_init(&sem_dentro, 0, 1);
    sem_init(&sem_vector_pet, 0, 1);
    sem_init(&sem_vector_aten, 0, 1);
    sem_init(&sem_flag_cola, 0, 1);
    sem_init(&sem_id_nodo_sig, 0, 1);
    sem_init(&sem_id_proceso_sig, 0, 1);
    sem_init(&sem_inicial, 0, 1);
    for (int i = 0; i < MAX_PROCESOS; i++)
    {
        sem_init(&sem_procesos[i], 0, 0);
    }
    num_nodos = atoi(argv[2]);
    int n_procesos = atoi(argv[3]);

    mi_id = atoi(argv[1]);

    id_cola_entrada = cola_entrada(mi_id);
    id_cola_distribucion = cola_distribucion(mi_id);

    pthread_t receptor;
    pthread_create(&receptor, NULL, proc_receptor, NULL);

    pthread_t pid_procesos;
    for (int i = 1; i < n_procesos + 1; i++)
    {
        printf("Creando proceso %d \n", i);
        pthread_create(&pid_procesos, NULL, (void *)proceso_main, (void *)i);
        printf("Proceso %d creado  \n", i);
    }

    int opcion;
    char cadena[10];

    while (1)
    {

        printf("\n99.Salir y eliminar colas.\n");
        fflush(stdout);

        fgets(cadena, 10, stdin);
        opcion = atoi(cadena);
        switch (opcion)
        {
        case 99:
            printf("Saliendo...\n");
            if (msgctl(id_cola_distribucion, IPC_RMID, NULL) == -1)
            {
                perror("Error eliminando la cola");
            }
            if (msgctl(id_cola_entrada, IPC_RMID, NULL) == -1)
            {
                perror("Error eliminando la cola");
            }

            exit(EXIT_SUCCESS);
            break;
        default:
            printf("\nOpción no válida\n");
            break;
        }
    }
}

void *proc_receptor(void *)
{
    int id_nodo_origen = 0, num_peticion_origen = 0, id_proceso_origen;
    struct Testigo Testigo, TestigoRecibido;
    struct PeticionTestigo PeticionTestigo;
    while (isEmpty(&cola_peticiones) != 1)
    {
        dequeue(&cola_peticiones);
    }
    while (1)
    {
        int estado = 0;
        printf("\nR: Esperando petición en la cola %d \n", id_cola_entrada);

        estado = msgrcv(id_cola_entrada, &PeticionTestigo, sizeof(struct PeticionTestigo) - sizeof(long), 2, 1); // con el ultimo 0 es modo bloqueante si en vez del 0 pones reciebe no wait no se queda suspendido si no hay mensaje
        // time_t now = time(NULL);
        //  printf("R:  Tiempo actual: %ld.%03ld segundos\n", now, (clock() * 1000 / CLOCKS_PER_SEC) % 1000);

        if (estado == -1)
        {
            printf("R:  error msgrcv peticion testigo\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            id_nodo_origen = PeticionTestigo.IDNodoOrigen;
            num_peticion_origen = PeticionTestigo.numero_peticion;
            id_proceso_origen = PeticionTestigo.IDProcesoOrigen;
            sem_wait(&sem_vector_pet);
            printf("R: He recibido una peticion del nodo %d del proceso %d con nº: %d \n", id_nodo_origen, id_proceso_origen, num_peticion_origen);
            vector_peticiones[id_nodo_origen][id_proceso_origen] = fmax(vector_peticiones[id_nodo_origen][id_proceso_origen], num_peticion_origen);
            sem_post(&sem_vector_pet);
            printf("\nR: ||valoro testigo:%d dentro:%d nodo_orig:%d proceso_orig:%d nºpeti:%d nºatend%d\n", testigo, dentro, id_nodo_origen, id_proceso_origen, vector_peticiones[id_nodo_origen][id_proceso_origen], vector_atendidas[id_nodo_origen][id_proceso_origen]);
            sem_wait(&sem_flag_cola);
            if ((testigo != 0) && (!dentro) && (vector_peticiones[id_nodo_origen][id_proceso_origen] > vector_atendidas[id_nodo_origen][id_proceso_origen]))
            {
                printf("R: check point 3\n");
                int id_cola_otro = cola_distribucion(id_nodo_origen);
                Testigo.mtype = 10;
                Testigo.IDProcesoDestino = id_proceso_origen;

                Testigo.atendidas_testigo[id_nodo_origen][id_proceso_origen] = num_peticion_origen;
                printf("P%d: actualizado la peticion atendida del nodo: %d, proceso: %d , con peticion nº:%d\n", id_proceso_sig, id_nodo_sig, id_proceso_sig, num_peticion_origen);

                Testigo.IDNodoOrigen = mi_id;

                printf("mtype: %ld \n", Testigo.mtype);
                printf("IDNodoOrigen: %d \n", Testigo.IDNodoOrigen);
                sem_wait(&sem_vector_aten);

                for (int l = 0; l < MAX_NODOS; l++)
                {
                    for (int k = 0; k < MAX_PROCESOS; k++)
                    {
                        Testigo.atendidas_testigo[l][k] = vector_atendidas[l][k];
                    }
                }
                sem_post(&sem_vector_aten);

                printf("atendidas_testigo: %d \n", Testigo.atendidas_testigo[id_nodo_origen][id_proceso_origen]);
                printf("IDProcesoDestino: %d \n", Testigo.IDProcesoDestino);
                printf("cola destino: %d \n", id_cola_otro);

                estado = msgsnd(id_cola_otro, &Testigo, sizeof(struct Testigo) - sizeof(long), 0);
                printf("R: TESTIGO  mtype: %ld, IDNodoOrigen: %d, IDProcesoDestino: %d Vector atendidas: %d tamaño vector: %ld\n", Testigo.mtype, Testigo.IDNodoOrigen, Testigo.IDProcesoDestino, Testigo.atendidas_testigo[id_nodo_sig][id_proceso_sig], sizeof(Testigo.atendidas_testigo));
                printf("tamaño struct: %ld \n", sizeof(struct Testigo));
                printf("tamaño vector_atendidas: %ld \n", sizeof(vector_atendidas));
                if (estado == -1)
                {
                    perror("R: msgsnd testigo del hilo receptor");
                    exit(EXIT_FAILURE);
                }
                // time_t now = time(NULL);
                //  printf("R: Tiempo actual: %ld.%03ld segundos\n", now, (clock() * 1000 / CLOCKS_PER_SEC) % 1000);
                sem_wait(&sem_testigo);
                testigo = 0;
                sem_post(&sem_testigo);
                printf("R: #Testigo enviado por el receptor desde nodo: %d, al nodo: %d con proceso %d  \n", mi_id, id_nodo_origen, id_proceso_origen);
                fflush(stdout);
            }
            else
            {
                if ((testigo != 0) && (dentro) && (vector_peticiones[id_nodo_origen][id_proceso_origen] > vector_atendidas[id_nodo_origen][id_proceso_origen]))
                {

                    enqueue(&cola_peticiones, PeticionTestigo);
                    printf("R: //Peticion encolada en el nodo %d del proceso %d con nº: %d \n", id_nodo_origen, id_proceso_origen, num_peticion_origen);
                    // PeticionTestigo = dequeue(&cola_peticiones);
                    // printf("R: //Peticion desencolada en el nodo %d del proceso %d con nº: %d \n", PeticionTestigo.IDNodoOrigen, PeticionTestigo.IDProcesoOrigen, PeticionTestigo.numero_peticion);
                    // printf("Is empty: %d\n", isEmpty(&cola_peticiones));

                    // printf("R: //id_nodo_sig: %d id_proceso_sig: %d\n", id_nodo_sig, id_proceso_sig);
                }
                else
                {
                    printf("R: //No se encola la peticion\n");
                }
            }
            sem_post(&sem_flag_cola);
        }

        estado = msgrcv(id_cola_entrada, &TestigoRecibido, sizeof(struct Testigo) - sizeof(long), 10, 1); // con el ultimo 0 es modo bloqueante si en vez del 0 pones reciebe no wait no se queda suspendido si no hay mensaje
        if (estado == -1)
        {
            printf("R:  ERROR RECIBIENDO TESTIGO\n");
        }
        else
        {
            printf("R:  TESTIGO RECIBIDO\n");
            int id_proceso_destino = TestigoRecibido.IDProcesoDestino;
            int id_nodo_origen = TestigoRecibido.IDNodoOrigen;
            for (int l = 0; l < MAX_NODOS; l++)
            {
                for (int k = 0; k < MAX_PROCESOS; k++)
                {

                    printf("#Mi testigo debe de ser mi id %d \n", testigo);
                    printf("atendidas_testigo[%d][%d]: %d \n", l, k, TestigoRecibido.atendidas_testigo[l][k]);
                    vector_atendidas[l][k] = Testigo.atendidas_testigo[l][k];
                }
            }
            sem_post(&sem_procesos[id_proceso_destino]);
        }
    }
}

void *proceso_main(int id_proceso)
{

    int mi_peticion = 0;
    int estado;
    struct Testigo Testigo;
    struct PeticionTestigo PeticionTestigo;
    sem_wait(&sem_inicial);
    if (mi_id == 0 && id_proceso == 1)
    {

        sem_wait(&sem_testigo);
        testigo = id_proceso;
        sem_post(&sem_testigo);
        printf("P%d: He cogido el testigo inicial \n", id_proceso);
    }
    sem_post(&sem_inicial);
    while (1)
    {
        printf("P%d: mi testigo es %d \n", id_proceso, testigo);

        /* while (testigo==1)
        {
            sleep(5);
            printf("Soy el proceso %d y mi testigo es %d \n",tipo_proceso,testigo);

        } */

        srand(time(NULL));
        int tiempo = rand() % 1 + 5; // Genera un número aleatorio entre
        sleep(tiempo);

        // sem_wait(&sem_testigo);
        printf("Soy el proceso %d y mi testigo es %d \n", id_proceso, testigo);
        if (testigo != id_proceso)
        {
            printf("P%d: Pido testigo\n", id_proceso);
            // sem_post(&sem_testigo);
            mi_peticion = mi_peticion + 1;

            for (int i = 0; i < num_nodos; i++)
            {
                // if (i == mi_id)
                //     continue;

                int id_cola_otro = cola_entrada(i);

                PeticionTestigo.mtype = 2;
                PeticionTestigo.IDNodoOrigen = mi_id;
                PeticionTestigo.numero_peticion = mi_peticion;
                PeticionTestigo.IDProcesoOrigen = id_proceso;
                estado = msgsnd(id_cola_otro, &PeticionTestigo, sizeof(struct PeticionTestigo) - sizeof(long), 0);
                if (estado == -1)
                {
                    printf("msgsnd buscando testigo en proceso %d\n", id_proceso);
                    exit(EXIT_FAILURE);
                }
                printf("P%d :Peticion enviada al nodo: %d con nº: %d \n", id_proceso, i, mi_peticion);
            }
            sem_wait(&sem_procesos[id_proceso]);
            printf("P%d: #He recibido el testigo\n", id_proceso);

            sem_wait(&sem_testigo);
            testigo = id_proceso;
            sem_post(&sem_testigo);
        }
        // sem_post(&sem_testigo);
        printf("P%d: #Mi testigo debe de ser mi id %d \n", id_proceso, testigo);

        sem_wait(&sem_dentro);
        dentro = 1;

        printf("P%d: #Entrando en la SC\n", id_proceso);
        sleep(4); // Simulamos la sección crítica

        printf("P%d: #Saliendo de la SC\n", id_proceso);

        dentro = 0;

        sem_post(&sem_dentro);
        printf("P%d: #Fuera de la SC\n", id_proceso);
        sem_wait(&sem_flag_cola);
        // printf("P%d: flag_cola: %d\n", pid, flag_cola);
        if (isEmpty(&cola_peticiones) == 0)
        {
            printf("P%d: Desencolando peticion\n", id_proceso);
            PeticionTestigo = dequeue(&cola_peticiones);
            printf("P%d: Peticion desencolada\n", id_proceso);

            int id_nodo_sig = PeticionTestigo.IDNodoOrigen;
            int id_proceso_sig = PeticionTestigo.IDProcesoOrigen;
            int numero_peticion_sig = PeticionTestigo.numero_peticion;
            if (vector_peticiones[id_nodo_sig][id_proceso_sig] > vector_atendidas[id_nodo_sig][id_proceso_sig])
            {
                printf("P%d: atiendo a la peticion en cola del nodo: %d, proceso: %d , con peticion nº:%d\n", id_proceso, id_nodo_sig, id_proceso_sig, numero_peticion_sig);
                int id_cola_sig = cola_distribucion(id_nodo_sig);
                Testigo.IDProcesoDestino = id_proceso_sig;
                Testigo.mtype = 10;
                sem_wait(&sem_vector_aten);
                for (int l = 0; l < MAX_NODOS; l++)
                {
                    for (int k = 0; k < MAX_PROCESOS; k++)
                    {
                        Testigo.atendidas_testigo[l][k] = vector_atendidas[l][k];
                    }
                }
                sem_post(&sem_vector_aten);
                Testigo.atendidas_testigo[id_nodo_sig][id_proceso_sig] = PeticionTestigo.numero_peticion;
                printf("P%d: ||Actualizado la peticion atendida del nodo: %d, proceso: %d , con peticion nº:%d\n", id_proceso, id_nodo_sig, id_proceso_sig, Testigo.atendidas_testigo[id_nodo_sig][id_proceso_sig]);
                Testigo.IDNodoOrigen = mi_id;
                printf("P%d: TESTIGO  mtype: %ld, IDNodoOrigen: %d, IDProcesoDestino: %d Vector atendidas: %d tamaño vector: %ld\n", id_proceso, Testigo.mtype, Testigo.IDNodoOrigen, Testigo.IDProcesoDestino, Testigo.atendidas_testigo[id_nodo_sig][id_proceso_sig], sizeof(Testigo.atendidas_testigo));
                printf("tamaño struct: %ld \n", sizeof(struct Testigo));
                printf("tamaño vector_atendidas: %ld \n", sizeof(vector_atendidas));
                estado = msgsnd(id_cola_sig, &Testigo, sizeof(struct Testigo) - sizeof(long), 0);
                // time_t now = time(NULL);
                //  printf("P%d: Tiempo actual: %ld.%03ld segundos\n", pid, now, (clock() * 1000 / CLOCKS_PER_SEC) % 1000);

                if (estado == -1)
                {
                    printf("msgsnd enviando testigo en proceso %d\n", id_proceso);
                    exit(EXIT_FAILURE);
                }
                for (int m = 0; m < MAX_NODOS; m++)
                {
                    for (int n = 0; n < MAX_PROCESOS; n++)
                    {
                        printf("P%d: atendidas_testigo[%d][%d]: %d \n", id_proceso, m, n, Testigo.atendidas_testigo[m][n]);
                    }
                }
                sem_wait(&sem_testigo);
                testigo = 0;
                sem_post(&sem_testigo);
                printf("P%d: #Testigo enviado peticion n: %d por el proceso principal desde el nodo: %d desde el proceso %d al nodo: %d  proceso %d\n", id_proceso, numero_peticion_sig, mi_id, id_proceso, id_nodo_sig, id_proceso_sig);
            }
            else
            {
                printf("P%d: No atiendo a la peticion en cola del nodo: %d, proceso: %d , con peticion nº:%d\n", id_proceso, id_nodo_sig, id_proceso_sig, numero_peticion_sig);
            }
        }
        else
        {
            printf("P%d: No hay peticiones en cola\n", id_proceso);
        }
        printf("P%d: #Mi testigo debe de ser mi id %d \n", id_proceso, testigo);

        sem_post(&sem_flag_cola);
    }
}

int cola_entrada(int id)
{
    key_t clave;
    clave = ftok("./", id);
    if (clave == (key_t)-1)
    {
        printf("Error al obtener la clave.\n");
        exit(0);
    }
    int id_cola_get = msgget(clave, 0777 | IPC_CREAT);
    if (id_cola_get == -1)
    {
        perror("msgget de la funcion cola");
        exit(-1);
    }
    printf("{{COLA entrada; id cola: %d, id nodo: %d, clave: %d}}\n\n", id_cola_get, id, clave);
    fflush(stdout);
    return id_cola_get;
}

int cola_distribucion(int id)
{
    key_t clave;
    clave = ftok("./", id);
    if (clave == (key_t)-1)
    {
        printf("Error al obtener la clave.\n");
        exit(0);
    }
    int id_cola_get = msgget(clave, 0777 | IPC_CREAT);
    if (id_cola_get == -1)
    {
        perror("msgget de la funcion cola");
        exit(-1);
    }
    printf("{{COLA distribucion; id cola: %d, id nodo: %d, clave: %d}}\n\n", id_cola_get, id, clave);

    fflush(stdout);
    return id_cola_get;
}
