#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <math.h> // para fmax()
#include <semaphore.h>

//tipos de mensajes:11 21 31 41 51 testigo 12 22 32 42 52 peticion testigo
// 15 25 35 45 55 pedir testigo en ese proceso

// numero de peticion comun a los procesos???

int cola(int id);
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
    int IDProcesoOrigen;
    int atendidas_testigo[10][5];
};

struct PeticionTestigo
{
    long mtype;
    int IDNodoOrigen;
    int IDProcesoOrigen;
    int numero_peticion;
};

sem_t sem_testigo, sem_dentro, sem_vector_pet, sem_vector_aten,sem_flag_cola,sem_id_nodo_sig,sem_id_proceso_sig;

int id_cola;
int mi_id;
int id_nodo_sig = 0,id_proceso_sig, flag_cola=0, num_nodos;
int vector_peticiones[100][5] = {0}, vector_atendidas[100][5] = {0}, dentro = 0, testigo = 0; 

int main(int argc, char *argv[]) // argv[1] es el id del nodo argv[2] es el numero de nodos
{

    sem_init(&sem_testigo,0,1);
    sem_init(&sem_dentro,0,1);
    sem_init(&sem_vector_pet,0,1);
    sem_init(&sem_vector_aten,0,1);
    sem_init(&sem_flag_cola,0,1);
    sem_init(&sem_id_nodo_sig,0,1);
    sem_init(&sem_id_proceso_sig,0,1);


    num_nodos = atoi(argv[2]);

    struct Testigo Testigo;
    struct Control Control;

    pthread_t receptor;

    

    mi_id = atoi(argv[1]);

    id_cola = cola(mi_id);

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
                printf("\nProceso 1\n");
                Control.mtype = 15;
                estado = msgsnd(id_cola, &Control, sizeof(struct Control) - sizeof(long), 0);
                    if (estado == -1)
                    {
                        printf("msgsnd enviando orden control %d\n", opcion);
                        exit(EXIT_FAILURE);
                    }
                break;
            case 2:
                printf("\nProceso 2\n");
                Control.mtype = 25;
                estado = msgsnd(id_cola, &Control, sizeof(struct Control) - sizeof(long), 0);
                    if (estado == -1)
                    {
                        printf("msgsnd enviando orden control %d\n", opcion);
                        exit(EXIT_FAILURE);
                    }
                break;
            case 3:
                printf("\nProceso 3\n");
                Control.mtype = 35;
                estado = msgsnd(id_cola, &Control, sizeof(struct Control) - sizeof(long), 0);
                    if (estado == -1)
                    {
                        printf("msgsnd enviando orden control %d\n", opcion);
                        exit(EXIT_FAILURE);
                    }
                break;
            case 4:
                printf("\nProceso 4\n");
                Control.mtype = 45;
                estado = msgsnd(id_cola, &Control, sizeof(struct Control) - sizeof(long), 0);
                    if (estado == -1)
                    {
                        printf("msgsnd enviando orden control %d\n", opcion);
                        exit(EXIT_FAILURE);
                    }
                break;
            case 5:

                printf("\nProceso 5\n");
                Control.mtype = 55;
                estado = msgsnd(id_cola, &Control, sizeof(struct Control) - sizeof(long), 0);
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
            if (msgctl(id_cola, IPC_RMID, NULL) == -1) {
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
        printf("\nEsperando petición en la cola %d \n", id_cola);

        estado = msgrcv(id_cola, &PeticionTestigo, sizeof(struct PeticionTestigo) - sizeof(long), 2, 0); // con el ultimo 0 es modo bloqueante si en vez del 0 pones reciebe no wait no se queda suspendido si no hay mensaje

        if (estado == -1)
        {
            printf(" error msgrcv peticion testigo\n");
            exit(EXIT_FAILURE);
        }

        id_nodo_origen = PeticionTestigo.IDNodoOrigen;
        num_peticion_origen = PeticionTestigo.numero_peticion;
        id_proceso_origen = PeticionTestigo.IDProcesoOrigen;

        printf("He recibido una peticion del nodo %d del proceso %d con nº: %d \n", id_nodo_origen,id_proceso_origen, num_peticion_origen);

        vector_peticiones[id_nodo_origen][id_proceso_origen] = fmax(vector_peticiones[id_nodo_origen][id_proceso_origen], num_peticion_origen);

        sem_wait(&sem_testigo);
        sem_wait(&sem_dentro);
        sem_wait(&sem_vector_aten);
        sem_wait(&sem_vector_pet);
        if (testigo && (!dentro) && (vector_peticiones[id_nodo_origen][id_proceso_origen] > vector_atendidas[id_nodo_origen][id_proceso_origen]))
        {
            sem_wait(&sem_flag_cola);
            flag_cola=0;
            sem_post(&sem_flag_cola);
            int id_cola_otro = cola(id_nodo_origen);

            switch (id_proceso_origen)
            {
            case 1:
                Testigo.mtype = 11;
                break;
            case 2:
                Testigo.mtype = 21;
                break;
            case 3:
                Testigo.mtype = 31;
                break;
            case 4:
                Testigo.mtype = 41;
                break;
            case 5:
                Testigo.mtype = 51;
                break;
            
            default:
                break;
            }

            for (int i = 0; i < 5; i++)
            {
                Testigo.atendidas_testigo[mi_id][i] = vector_atendidas[mi_id][i];
            }

            Testigo.IDNodoOrigen = mi_id;
            estado = msgsnd(id_cola_otro, &Testigo, sizeof(struct Testigo) - sizeof(long), 0);
            if (estado == -1)
            {
                perror("msgsnd testigo del hilo receptor");
                exit(EXIT_FAILURE);
            }

            
            testigo=0;

            printf("Testigo enviado por el hilo desde nodo: %d, al nodo: %d con proceso %d  \n", mi_id, id_nodo_origen, id_proceso_origen);
        }
        else
        {
            if (testigo && (dentro) && (vector_peticiones[id_nodo_origen][id_proceso_origen] > vector_atendidas[id_nodo_origen][id_proceso_origen])){
                sem_wait(&sem_flag_cola);
                flag_cola=1;
                sem_post(&sem_flag_cola);
                id_nodo_sig=id_nodo_origen;
                id_proceso_sig=id_proceso_origen;
                printf("//id_nodo_sig: %d id_proceso_sig: %d\n",id_nodo_sig,id_proceso_sig);
            }
        }
        sem_post(&sem_dentro);
        sem_post(&sem_testigo);
        sem_post(&sem_vector_aten);
        sem_post(&sem_vector_pet);
    }
}

void *proceso_main(int tipo_proceso){

    int mi_peticion=0;
    int estado, testigo_proc=0;
    struct Testigo Testigo;
    struct Control Control;
    struct PeticionTestigo PeticionTestigo;
    int ntype_testigo,ntype_peticion,ntype_control;

    switch (tipo_proceso)
    {
        case 1:
            if(mi_id==0){
                sem_wait(&sem_testigo);
                testigo=1;
                sem_post(&sem_testigo);
            }
            ntype_testigo=11;
            ntype_peticion=12;
            ntype_control=15;
            break;
        case 2:
            ntype_testigo=21;
            ntype_peticion=22;
            ntype_control=25;
            break;
        case 3:
            ntype_testigo=31;
            ntype_peticion=32;
            ntype_control=35;
            break;
        case 4:
            ntype_testigo=41;
            ntype_peticion=42;
            ntype_control=45;
            break;
        case 5:
            ntype_testigo=51;
            ntype_peticion=52;
            ntype_control=55;
            break;
        default:
            break;            
    }

    printf("Soy el proceso %d y mi testigo es %d \n",tipo_proceso,testigo);

    while (1)
    {
        //CAMBIAR ESTO
        while (testigo==1)
        {
            sleep(1);
        }
        
            estado = msgrcv(id_cola, &Control, sizeof(struct Control) - sizeof(long), ntype_control, 0); 
                if (estado == -1)
                {
                    printf("msgrcv recibiendo la orden de pedir testigo %d\n", tipo_proceso);
                    exit(EXIT_FAILURE);
                }
            printf("Pidiendo testigo\n");
            sem_wait(&sem_testigo);
            if (!testigo)
            {
                sem_post(&sem_testigo);
                mi_peticion = mi_peticion + 1;

                for (int i = 0; i < num_nodos; i++)
                {
                    //if (i == mi_id)
                    //    continue;

                    int id_cola_otro = cola(i);

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
                    printf("Peticion enviada al nodo: %d con nº: %d desde el proceso %d \n", i, mi_peticion, tipo_proceso);
                }
                
                estado = msgrcv(id_cola, &Testigo, sizeof(struct Testigo) - sizeof(long), ntype_testigo, 0); // con el ultimo 0 es modo bloqueante si en vez del 0 pones reciebe no wait no se queda suspendido si no hay mensaje

                if (estado == -1)
                {
                    printf("msgrcv buscando testigo en proceso %d\n", tipo_proceso);
                    exit(EXIT_FAILURE);
                }
                printf("#He recibido el testigo en el proceso %d\n", tipo_proceso);
                sem_wait(&sem_testigo);
                testigo = 1;
                sem_post(&sem_testigo);
            }
            sem_wait(&sem_dentro);
            dentro = 1;
            sem_post(&sem_dentro);
            printf("#Entrando en la SC en el proceso %d\n", tipo_proceso);
            sleep(2); // Simulamos la sección crítica
            printf("#Saliendo de la SC en el proceso %d\n", tipo_proceso);
            sem_wait(&sem_vector_aten);
            vector_atendidas[mi_id][tipo_proceso] = mi_peticion;
            sem_post(&sem_vector_aten);
            sem_wait(&sem_dentro);
            dentro = 0;
            sem_post(&sem_dentro);

            sem_wait(&sem_flag_cola);
            if (flag_cola != 0)
                if (vector_peticiones[id_nodo_sig][id_proceso_sig] > vector_atendidas[id_nodo_sig][id_proceso_sig])
                {
                    
                    int id_cola_sig = cola(id_nodo_sig);
                    Testigo.mtype = 1;
                    Testigo.atendidas_testigo[mi_id][tipo_proceso] = mi_peticion;
                    Testigo.IDNodoOrigen = mi_id;
                    estado = msgsnd(id_cola_sig, &Testigo, sizeof(struct Testigo) - sizeof(long), 0);
                    if (estado == -1)
                    {
                        printf("msgsnd enciando testigo en proceso %d\n", tipo_proceso);
                        exit(EXIT_FAILURE);
                    }

                    testigo = 0;

                    printf("#Testigo enviado por el principal desde el nodo: %d, al nodo %d desde el proceso %d\n",mi_id, id_nodo_sig, tipo_proceso);
                    flag_cola = 0;
                }
            sem_post(&sem_flag_cola);
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
    int id_cola_get = msgget(clave, 0777 | IPC_CREAT);
    if (id_cola == -1)
    {
        perror("msgget de la funcion cola");
        exit(-1);
    }
    printf("{{COLA: \n");
    printf("id_cola: %d\n", id_cola_get);
    printf("id_nodo: %d\n", id);
    printf("clave: %d}}\n\n", clave);
    return id_cola_get;
}
