#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <math.h> // para fmax()
#include <semaphore.h>
#include <time.h>

// tipos de mensajes:11 21 31 41 51 testigo
//  15 25 35 45 55 pedir testigo en ese proceso

int cola_entrada(int id);
int cola_distribucion(int id);
void *proc_receptor(void *);
void *proceso_main(int tipo_proceso);

struct Control
{
    long mtype;
};

struct Testigo
{
    long mtype;
    int IDNodoOrigen;
    int atendidas_testigo[10][5];
};

struct PeticionTestigo
{
    long mtype;
    int IDNodoOrigen;
    int IDProcesoOrigen;
    int numero_peticion;
};

sem_t sem_testigo, sem_dentro, sem_vector_pet, sem_vector_aten, sem_flag_cola, sem_id_nodo_sig, sem_id_proceso_sig;

int id_cola_distribucion, id_cola_entrada;
int mi_id;
int id_nodo_sig = 0, id_proceso_sig = 0, flag_cola = 0, num_nodos;
int vector_peticiones[100][5] = {0}, vector_atendidas[100][5] = {0}, dentro = 0, testigo = 0;

int main(int argc, char *argv[]) // argv[1] es el id del nodo argv[2] es el numero de nodos
{

    sem_init(&sem_testigo, 0, 1);
    sem_init(&sem_dentro, 0, 1);
    sem_init(&sem_vector_pet, 0, 1);
    sem_init(&sem_vector_aten, 0, 1);
    sem_init(&sem_flag_cola, 0, 1);
    sem_init(&sem_id_nodo_sig, 0, 1);
    sem_init(&sem_id_proceso_sig, 0, 1);

    num_nodos = atoi(argv[2]);

    struct Testigo Testigo;
    struct Control Control;

    pthread_t receptor;

    mi_id = atoi(argv[1]);

    id_cola_entrada = cola_entrada(mi_id);
    id_cola_distribucion = cola_distribucion(mi_id);

    pthread_create(&receptor, NULL, proc_receptor, NULL);

    for (int i = 1; i < 6; i++)
    {
        pthread_create(&receptor, NULL, (void *(*)(void *))proceso_main, (void *)i);
    }

    int opcion, estado;
    char cadena[10];

    while (1)
    {

        printf("\n1.Pedir testigo");
        printf("\n2.Salir.\n");
        fflush(stdout);

        fgets(cadena, 10, stdin);
        opcion = atoi(cadena);
        switch (opcion)
        {

        case 1:
            printf("\nSeleccione proceso para pedir testigo: ");
            fgets(cadena, 10, stdin);
            opcion = atoi(cadena);
            switch (opcion)
            {
            case 1:
                Control.mtype = 15;
                estado = msgsnd(id_cola_distribucion, &Control, sizeof(struct Control) - sizeof(long), 0);
                if (estado == -1)
                {
                    printf("msgsnd enviando orden control %d\n", opcion);
                    exit(EXIT_FAILURE);
                }
                break;
            case 2:
                Control.mtype = 25;
                estado = msgsnd(id_cola_distribucion, &Control, sizeof(struct Control) - sizeof(long), 0);
                if (estado == -1)
                {
                    printf("msgsnd enviando orden control %d\n", opcion);
                    exit(EXIT_FAILURE);
                }
                break;
            case 3:
                Control.mtype = 35;
                estado = msgsnd(id_cola_distribucion, &Control, sizeof(struct Control) - sizeof(long), 0);
                if (estado == -1)
                {
                    printf("msgsnd enviando orden control %d\n", opcion);
                    exit(EXIT_FAILURE);
                }
                break;
            case 4:
                Control.mtype = 45;
                estado = msgsnd(id_cola_distribucion, &Control, sizeof(struct Control) - sizeof(long), 0);
                if (estado == -1)
                {
                    printf("msgsnd enviando orden control %d\n", opcion);
                    exit(EXIT_FAILURE);
                }
                break;
            case 5:

                Control.mtype = 55;
                estado = msgsnd(id_cola_distribucion, &Control, sizeof(struct Control) - sizeof(long), 0);
                if (estado == -1)
                {
                    printf("msgsnd enviando orden control %d\n", opcion);
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                printf("\nOpción no válida\n");
                break;
            }

            break;

        case 2:
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
    int id_nodo_origen = 0, num_peticion_origen = 0, estado, id_proceso_origen;
    struct Testigo Testigo;
    struct PeticionTestigo PeticionTestigo;
    while (1)
    {
        printf("\nR: Esperando petición en la cola %d \n", id_cola_entrada);

        estado = msgrcv(id_cola_entrada, &PeticionTestigo, sizeof(struct PeticionTestigo) - sizeof(long), 2, 0); // con el ultimo 0 es modo bloqueante si en vez del 0 pones reciebe no wait no se queda suspendido si no hay mensaje
        time_t now = time(NULL);
        // printf("R:  Tiempo actual: %ld.%03ld segundos\n", now, (clock() * 1000 / CLOCKS_PER_SEC) % 1000);

        if (estado == -1)
        {
            printf("R:  error msgrcv peticion testigo\n");
            exit(EXIT_FAILURE);
        }

        id_nodo_origen = PeticionTestigo.IDNodoOrigen;
        num_peticion_origen = PeticionTestigo.numero_peticion;
        id_proceso_origen = PeticionTestigo.IDProcesoOrigen;

        printf("R: He recibido una peticion del nodo %d del proceso %d con nº: %d \n", id_nodo_origen, id_proceso_origen, num_peticion_origen);
        fflush(stdout);
        vector_peticiones[id_nodo_origen][id_proceso_origen] = fmax(vector_peticiones[id_nodo_origen][id_proceso_origen], num_peticion_origen);

        // sem_wait(&sem_vector_aten);
        // sem_wait(&sem_vector_pet);
        // printf("\nR: ||valoro testigo:%d dentro:%d nodo_orig:%d proceso_orig:%d nºpeti:%d nºatend%d\n", testigo, dentro, id_nodo_origen, id_proceso_origen, vector_peticiones[id_nodo_origen][id_proceso_origen], vector_atendidas[id_nodo_origen][id_proceso_origen]);
        if ((testigo != 0) && (!dentro) && (vector_peticiones[id_nodo_origen][id_proceso_origen] > vector_atendidas[id_nodo_origen][id_proceso_origen]))
        {
            // printf("\nR: ||entro en el if\n");
            sem_wait(&sem_flag_cola);
            flag_cola = 0;
            sem_post(&sem_flag_cola);
            int id_cola_otro = cola_distribucion(id_nodo_origen);
            Testigo.mtype = id_proceso_origen * 10 + 1;

            for (int i = 0; i < 5; i++)
            {
                Testigo.atendidas_testigo[mi_id][i] = vector_atendidas[mi_id][i];
            }

            Testigo.IDNodoOrigen = mi_id;

            estado = msgsnd(id_cola_otro, &Testigo, sizeof(struct Testigo) - sizeof(long), 0);
            time_t now = time(NULL);
            // printf("R: Tiempo actual: %ld.%03ld segundos\n", now, (clock() * 1000 / CLOCKS_PER_SEC) % 1000);

            if (estado == -1)
            {
                perror("R: msgsnd testigo del hilo receptor");
                exit(EXIT_FAILURE);
            }

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
                sem_wait(&sem_flag_cola);
                flag_cola = 1;
                sem_post(&sem_flag_cola);
                sem_wait(&sem_id_nodo_sig);
                id_nodo_sig = id_nodo_origen;
                sem_post(&sem_id_nodo_sig);
                sem_wait(&sem_id_proceso_sig);
                id_proceso_sig = id_proceso_origen;
                sem_post(&sem_id_proceso_sig);
                // printf("R: //id_nodo_sig: %d id_proceso_sig: %d\n", id_nodo_sig, id_proceso_sig);
            }
        }
        sem_post(&sem_vector_aten);
        sem_post(&sem_vector_pet);
    }
}

void *proceso_main(int tipo_proceso)
{

    int mi_peticion = 0;
    int estado, testigo_proc = 0;
    struct Testigo Testigo;
    struct Control Control;
    struct PeticionTestigo PeticionTestigo;
    int ntype_testigo, ntype_control;

    switch (tipo_proceso)
    {
    case 1:
        if (mi_id == 0)
        {
            sem_wait(&sem_testigo);
            testigo = 1;
            sem_post(&sem_testigo);
        }
        ntype_testigo = 11;
        ntype_control = 15;
        break;
    case 2:
        ntype_testigo = 21;
        ntype_control = 25;
        break;
    case 3:
        ntype_testigo = 31;
        ntype_control = 35;
        break;
    case 4:
        ntype_testigo = 41;
        ntype_control = 45;
        break;
    case 5:
        ntype_testigo = 51;
        ntype_control = 55;
        break;
    default:
        printf("Error en el tipo de proceso");
        break;
    }

    // printf("P%d : y mi testigo es %d \n", tipo_proceso, testigo);

    while (1)
    {
        /* while (testigo==1)
        {
            sleep(5);
            printf("Soy el proceso %d y mi testigo es %d \n",tipo_proceso,testigo);
        } */

        estado = msgrcv(id_cola_distribucion, &Control, sizeof(struct Control) - sizeof(long), ntype_control, 0);
        if (estado == -1)
        {
            printf("msgrcv recibiendo la orden de pedir testigo %d\n", tipo_proceso);
            exit(EXIT_FAILURE);
        }
        printf("P%d: Pidiendo testigo\n", tipo_proceso);

        // sem_wait(&sem_testigo);
        // printf("Soy el proceso %d y mi testigo es %d \n", tipo_proceso, testigo);
        if (testigo != tipo_proceso)
        {
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
                PeticionTestigo.IDProcesoOrigen = tipo_proceso;
                estado = msgsnd(id_cola_otro, &PeticionTestigo, sizeof(struct PeticionTestigo) - sizeof(long), 0);
                if (estado == -1)
                {
                    printf("msgsnd buscando testigo en proceso %d\n", tipo_proceso);
                    exit(EXIT_FAILURE);
                }
                printf("P%d :Peticion enviada al nodo: %d con nº: %d desde el proceso %d \n", tipo_proceso, i, mi_peticion, tipo_proceso);
            }
            printf("P%d: Esperando testigo\n", tipo_proceso);
            estado = msgrcv(id_cola_distribucion, &Testigo, sizeof(struct Testigo) - sizeof(long), ntype_testigo, 0);

            if (estado == -1)
            {
                printf("msgrcv buscando testigo en proceso %d\n", tipo_proceso);
                exit(EXIT_FAILURE);
            }
            printf("P%d: #He recibido el testigo\n", tipo_proceso);
            sem_wait(&sem_testigo);
            testigo = tipo_proceso;
            sem_post(&sem_testigo);
        }
        // sem_post(&sem_testigo);
        sem_wait(&sem_dentro);
        dentro = 1;
        printf("P%d: #Entrando en la SC\n", tipo_proceso);
        sleep(5); // Simulamos la sección crítica
        printf("P%d: #Saliendo de la SC\n", tipo_proceso);
        sem_wait(&sem_vector_aten);
        vector_atendidas[mi_id][tipo_proceso] = mi_peticion;
        sem_post(&sem_vector_aten);
        dentro = 0;
        sem_post(&sem_dentro);
        printf("P%d: #Fuera de la SC\n", tipo_proceso);
        sem_wait(&sem_flag_cola);
        // printf("P%d: flag_cola: %d\n", tipo_proceso, flag_cola);
        if (flag_cola != 0)
        {
            if (vector_peticiones[id_nodo_sig][id_proceso_sig] > vector_atendidas[id_nodo_sig][id_proceso_sig])
            {
                printf("P%d: atiendo a la peticion en cola del nodo: %d, proceso: %d\n", tipo_proceso, id_nodo_sig, id_proceso_sig);
                int id_cola_sig = cola_distribucion(id_nodo_sig);
                int tipo_proceso_sig = id_proceso_sig * 10 + 1;
                Testigo.mtype = tipo_proceso_sig;
                Testigo.atendidas_testigo[mi_id][tipo_proceso] = mi_peticion;
                Testigo.IDNodoOrigen = mi_id;
                estado = msgsnd(id_cola_sig, &Testigo, sizeof(struct Testigo) - sizeof(long), 0);
                time_t now = time(NULL);
                // printf("P%d: Tiempo actual: %ld.%03ld segundos\n", tipo_proceso, now, (clock() * 1000 / CLOCKS_PER_SEC) % 1000);

                if (estado == -1)
                {
                    printf("msgsnd enviando testigo en proceso %d\n", tipo_proceso);
                    exit(EXIT_FAILURE);
                }
                sem_wait(&sem_testigo);
                testigo = 0;
                sem_post(&sem_testigo);
                printf("P%d: #Testigo enviado por el proceso principal desde el nodo: %d desde el proceso %d al nodo: %d  proceso %d\n", tipo_proceso, mi_id, tipo_proceso, id_nodo_sig, id_proceso_sig);
                flag_cola = 0;
            }
        }
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
    clave = ftok("./testigo_n.c", id);
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