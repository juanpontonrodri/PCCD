#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


struct msgbuf {
    long mtype;
    int tenedorIzq;
    int tenedorDer;
};


void inicFilosofoTenedor (int);
void filosofoAComer(int, int);
#define N 5 // Número de filósofos (y tenedores)

int main(int argc, char* argv[]) {
    int IDColaMensajes;
    key_t Clave;

    if (argc != 2) {
        printf("Introduce un número entero como parámetro para crear el ID del buzón de mensajes.\n");
        exit(EXIT_FAILURE);
    }

    Clave = ftok("/bin/ls", atoi(argv[1]));
    if (Clave == (key_t)-1) {
        printf("Error al obtener la clave.\n");
        exit(EXIT_FAILURE);
    } else {
        IDColaMensajes = msgget(Clave, 0777 | IPC_CREAT);

        if (IDColaMensajes == -1) {
            printf("La invocación a 'msgget()' ha fallado.\n");
            exit(EXIT_FAILURE);
        } else {
            printf("Se ha creado el buzón de mensajes correctamente, con el ID %i.\n", IDColaMensajes);
        }
    }


    inicFilosofoTenedor(IDColaMensajes);

    // Crear un proceso filósofo para cada posición en la mesa
    pid_t pid[N];
    for (int i = 1; i <= N; i++) {
        pid[i - 1] = fork();
        if (pid[i - 1] == -1) {
            printf("Error al crear el proceso filósofo %d.\n", i);
            exit(EXIT_FAILURE);
        } else if (pid[i - 1] == 0) {
            // Soy el proceso hijo (filósofo)
            filosofo(i, IDColaMensajes);
            exit(EXIT_SUCCESS);
        }
    }

    // Esperar a que terminen todos los procesos hijos (filósofos)
    for (int i = 0; i < N; i++) {
        waitpid(pid[i], NULL, 0);
    }

    // Eliminar la cola de mensajes
    if (msgctl(IDColaMensajes, IPC_RMID, NULL) == -1) {
        printf("La invocación a 'msgctl()' ha fallado.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Se ha eliminado la cola de mensajes correctamente.\n");
    }

    return 0;
}


void inicFilosofoTenedor(int IDColaMensajes){
    int i;
    printf("Procedemos a meter los tenedores en el buffer...\n");

    struct msgbuf tenedor[N + 1];

    // Se inicializan los tenedores y se envían a la cola de mensajes
    for (i = 1; i <= N; i++) {
        tenedor[i].mtype = i;
        tenedor[i].tenedorIzq = i;
        tenedor[i].tenedorDer = (i % N) + 1;

        if ((msgsnd(IDColaMensajes, &tenedor[i], sizeof(struct msgbuf) - sizeof(long), IPC_NOWAIT)) == -1) {
            printf("La invocación a 'msgsnd()' ha fallado.\n");
            exit(EXIT_FAILURE);
        } else {
            printf("[Tenedor %i] -> Introducido con éxito en el buffer.\n", i);
        }
    }
    printf("Todos los tenedores se han introducido en el buffer.\n");
}


void filosofoAComer(int i, int IDColaMensajes) {
    struct msgbuf msgIzq, msgDer;

    // Filósofo coge el tenedor de su izquierda
    if (msgrcv(IDColaMensajes, &msgIzq, sizeof(struct msgbuf) - sizeof(long), i, 0) == -1) {
        printf("[Filósofo %d] -> Error al recibir el tenedor de su izquierda.\n", i);
        exit(EXIT_FAILURE);
    }
    printf("[Filósofo %d] -> Ha cogido el tenedor de su izquierda.\n", i);

    // Filósofo coge el tenedor de su derecha
    if (msgrcv(IDColaMensajes, &msgDer, sizeof(struct msgbuf) - sizeof(long), (i % N) + 1, 0) == -1) {
        printf("[Filósofo %d] -> Error al recibir el tenedor de su derecha.\n", i);
        exit(EXIT_FAILURE);
    }
    printf("[Filósofo %d] -> Ha cogido el tenedor de su derecha.\n", i);

    // Filósofo come durante un tiempo aleatorio
    printf("[Filósofo %d] -> COMIENDO...\n", i);
    sleep(rand() % 5 + 1);

    // Filósofo suelta el tenedor de su izquierda
    msgIzq.tenedorIzq = i;
    if (msgsnd(IDColaMensajes, &msgIzq, sizeof(struct msgbuf) - sizeof(long), IPC_NOWAIT) == -1) {
        printf("[Filósofo %d] -> Error al soltar el tenedor de su izquierda.\n", i);
        exit(EXIT_FAILURE);
    }
    printf("[Filósofo %d] -> Ha soltado el tenedor de su izquierda.\n", i);

    // Filósofo suelta el tenedor de su derecha
    msgDer.tenedorDer = (i % N) + 1;
    if (msgsnd(IDColaMensajes, &msgDer, sizeof(struct msgbuf) - sizeof(long), IPC_NOWAIT) == -1) {
        printf("[Filósofo %d] -> Error al soltar el tenedor de su derecha.\n", i);
        exit(EXIT_FAILURE);
    }
    printf("[Filósofo %d] -> Ha soltado el tenedor de su derecha.\n", i);
}
