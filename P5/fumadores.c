#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


struct mensaje {
    long tipo;
    int dato;
};

int main(int argc, char *argv[]) {
    const char *fumadorString[] = {"Papel", "Tabaco", "Fósforos"};
    const char *necesidades[] = {"el tabaco y los fósforos", "el papel y los fósforos", "el papel y el tabaco"};
    int fumador, i, id_cola[3];
    struct mensaje mensaje;

    if (argc != 2) {
        printf("La llamada correcta al proceso es: %s tipo\n", argv[0]);
        exit(-1);
    }

    fumador = atoi(argv[1]);
    mensaje.tipo = 1;
    mensaje.dato = 1;

    for (i = 0; i < 3; i++) {
        key_t key = 22 + ((i + fumador) % 3);  // Se utiliza la aritmética modular para obtener la clave de la cola.
        id_cola[i] = msgget(key, 0600 | IPC_CREAT);
    }

    while (1) {
        printf("[Fumador %s]-> Intentando fumar...\n", fumadorString[fumador]);
        msgrcv(id_cola[fumador], &mensaje, sizeof(int), 1, 0);
        printf("[Fumador %s]-> He cogido %s que ha puesto el proveedor y estoy fumando\n", fumadorString[fumador], necesidades[fumador]);
        getchar();
        printf("[Fumador %s]-> He dejado de fumar\n", fumadorString[fumador]);

        // Comprobamos si los ingredientes son los necesarios.
        if ((fumador == 0 && mensaje.dato != 3) || (fumador == 1 && mensaje.dato != 5) || (fumador == 2 && mensaje.dato != 6)) {
            printf("[Fumador %s]-> Los ingredientes no son los necesarios. Volviendo a poner los ingredientes en la cola...\n", fumadorString[fumador]);
            msgsnd(id_cola[(fumador + 2) % 3], &mensaje, sizeof(int), 0);  // Se utiliza la aritmética modular para enviar el mensaje a la cola correspondiente.
        } else {
            msgsnd(id_cola[(fumador + 2) % 3], &mensaje, sizeof(int), 0);  // Se utiliza la aritmética modular para enviar el mensaje a la cola correspondiente.
        }
    }

    return 0;
}
