#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


struct mensaje {
    long tipo;
    int dato;
};

int main(int argc, char *argv[]) {
    const char *materiales[] = {"tabaco y fósforos", "papel y fósforos", "papel y tabaco"};
    int i, id_cola[4];
    struct mensaje mensaje;

    srand(time(NULL));
    for (i = 0; i < 4; i++) {
        key_t key = 22 + i;
        id_cola[i] = msgget(key, 0600 | IPC_CREAT);
        msgctl(id_cola[i], IPC_RMID, NULL);
        id_cola[i] = msgget(key, 0600 | IPC_CREAT);
    }

    while (1) {
        int ingredientes = rand() % 3;
        printf("[Proveedor]-> El proveedor elige los ingredientes %s\n", materiales[ingredientes]);

        // Enviamos los ingredientes a los fumadores correspondientes.
        for (i = 0; i < 3; i++) {
            if (i != ingredientes) {
                msgsnd(id_cola[i], &mensaje, sizeof(int), 0);
            }
        }

        // Esperamos a que acaben de fumar los fumadores.
        for (i = 0; i < 3; i++) {
            msgrcv(id_cola[3], &mensaje, sizeof(int), 1, 0);
        }
    }

    return 0;
}
