#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <math.h> // para fmax()
// seamforos para proteger vairabless

int cola(int id);
void *proc_receptor(void *);

struct Testigo
{
    long mtype;
    int IDNodoOrigen;
    int atendidas_testigo[10];
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
int id_nodo_sig = 0, flag_cola=0;
int vector_peticiones[100] = {0}, vector_atendidas[100] = {0}, dentro = 0, testigo = 1; // ->0 los otros nodos

int main(int argc, char *argv[]) // argv[1] es el id del nodo y argv[2] es el testigo y argv[3] es el numero de nodos
{
    int num_nodos = atoi(argv[3]);

    struct Testigo Testigo;
    struct PeticionTestigo PeticionTestigo;

    pthread_t receptor;

    int opcion, estado;
    char cadena[10];

    mi_id = atoi(argv[1]);
    testigo = atoi(argv[2]);

    id_cola = cola(mi_id);

    pthread_create(&receptor, NULL, proc_receptor, NULL);
    while (1)
    {
        while (testigo==1)
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

                    int id_cola_otro = cola(i);

                    PeticionTestigo.mtype = 2;
                    PeticionTestigo.IDNodoOrigen = mi_id;
                    PeticionTestigo.numero_peticion = mi_peticion;
                    estado = msgsnd(id_cola_otro, &PeticionTestigo, sizeof(struct PeticionTestigo) - sizeof(long), 0);
                    if (estado == -1)
                    {
                        perror("msgsnd buscanod testigo");
                        exit(EXIT_FAILURE);
                    }
                    printf("Peticion enviada al nodo: %d con nº: %d\n", i, mi_peticion);
                }

                estado = msgrcv(id_cola, &Testigo, sizeof(struct Testigo) - sizeof(long), 1, 0); // con el ultimo 0 es modo bloqueante si en vez del 0 pones reciebe no wait no se queda suspendido si no hay mensaje

                if (estado == -1)
                {
                    perror("msgrcv testigo principal");
                    exit(EXIT_FAILURE);
                }
                printf("#He recibido el testigo \n");
                testigo = 1;
            }

            dentro = 1;
            printf("#Entrando en la SC\n");
            sleep(2); // SC
            printf("#Saliendo de la SC\n");
            vector_atendidas[mi_id] = mi_peticion;
            dentro = 0;

            if (flag_cola != 0)
                if (vector_peticiones[id_nodo_sig] > vector_atendidas[id_nodo_sig])
                {
                    
                    int id_cola_sig = cola(id_nodo_sig);
                    Testigo.mtype = 1;
                    Testigo.atendidas_testigo[mi_id] = mi_peticion;
                    Testigo.IDNodoOrigen = mi_id;
                    estado = msgsnd(id_cola_sig, &Testigo, sizeof(struct Testigo) - sizeof(long), 0);
                    if (estado == -1)
                    {
                        perror("msgsnd testigo principal");
                        exit(EXIT_FAILURE);
                    }

                    testigo = 0;

                    printf("#Testigo enviado por el principal desde el nodo: %d, al nodo %d\n",mi_id, id_nodo_sig);
                    flag_cola = 0;
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
        printf("\nEsperando petición en la cola %d\n", id_cola);

        estado = msgrcv(id_cola, &PeticionTestigo, sizeof(struct PeticionTestigo) - sizeof(long), 2, 0); // con el ultimo 0 es modo bloqueante si en vez del 0 pones reciebe no wait no se queda suspendido si no hay mensaje

        if (estado == -1)
        {
            printf(" error msgrcv peticion testigo\n");
            exit(EXIT_FAILURE);
        }

        id_nodo_origen = PeticionTestigo.IDNodoOrigen;
        num_peticion_origen = PeticionTestigo.numero_peticion;

        printf("He recibido una peticion del nodo %d con nº: %d \n", id_nodo_origen, num_peticion_origen);

        vector_peticiones[id_nodo_origen] = fmax(vector_peticiones[id_nodo_origen], num_peticion_origen);

        
        if (testigo && (!dentro) && (vector_peticiones[id_nodo_origen] > vector_atendidas[id_nodo_origen]))
        {
            flag_cola=0;
            int id_cola_otro = cola(id_nodo_origen);

            // int id_cola_otro = cola(99);

            Testigo.mtype = 1;
            Testigo.atendidas_testigo[mi_id] = mi_peticion;
            Testigo.IDNodoOrigen = mi_id;
            estado = msgsnd(id_cola_otro, &Testigo, sizeof(struct Testigo) - sizeof(long), 0);
            if (estado == -1)
            {
                perror("msgsnd testigo del hilo receptor");
                exit(EXIT_FAILURE);
            }

            testigo=0;

            printf("Testigo enviado por el hilo desde nodo: %d, al nodo: %d \n", mi_id, id_nodo_origen);
        }
        else
        {
            if (testigo && (dentro) && (vector_peticiones[id_nodo_origen] > vector_atendidas[id_nodo_origen])){
                flag_cola=1;
                id_nodo_sig=id_nodo_origen;
                printf("id_nodo_sig: %d\n", id_nodo_sig);
            }
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
    //key_t clave = id;
    int id_cola = msgget(clave, 0777 | IPC_CREAT);
    if (id_cola == -1)
    {
        perror("msgget de la funcion cola");
        exit(-1);
    }
    printf("{{COLA: \n");
    printf("id_cola: %d\n", id_cola);
    printf("id_nodo: %d\n", id);
    printf("clave: %d}}\n\n", clave);
    return id_cola;
}
