#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct mensaje {
    long tipo;
    char datos[100];
};

int main() {
    key_t clave = ftok("/home/juan", 5);
    int id_cola_mensajes = msgget(clave, 0666);

    if (id_cola_mensajes == -1) {
        perror("msgget");
        exit(-1);
    }

    struct mensaje msg;
    if (msgrcv(id_cola_mensajes, &msg, sizeof(struct mensaje) - sizeof(long), 1, IPC_NOWAIT) == -1) {
        printf("No hay mensajes en la cola.\n");
    }

    printf("Mensaje recibido: %s\n", msg.datos);


    return 0;
}
