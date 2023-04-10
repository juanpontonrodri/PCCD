#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TABACO 1
#define PAPEL 2
#define FOSFOROS 3

struct mensaje {
    long tipo;
    int dato;
};

int main(int argc, char *argv[]) {
    int id_cola_tabaco, id_cola_papel, id_cola_fosforos;
    struct mensaje mensaje;

    key_t key_tabaco = 22;
    key_t key_papel = 23;
    key_t key_fosforos = 24;
    srand(time(NULL));
    id_cola_tabaco = msgget(key_tabaco, 0600 | IPC_CREAT);
    id_cola_papel = msgget(key_papel, 0600 | IPC_CREAT);
    id_cola_fosforos = msgget(key_fosforos, 0600 | IPC_CREAT);

    //Tabaco=1, papel=2, fósforos=3
    while (1) {
        //FALTA SINCRONIZACION PARA QUE SOLO ENVIA DESPUES DE QUE ALGUIEN FUME
        // Y PARA Q LUEGO LOS RESETEE CUNAOD ALGUIEN FUME
        int ingredientes = 2;

        printf("[Proveedor]-> El proveedor elige los ingredientes para fumar\n");

        //Seleccionar aleatoriamente dos ingredientes
        int ing1 = rand() % 3 + 1;
        int ing2 = rand() % 3 + 1;
        while (ing2 == ing1) {
            ing2 = rand() % 3 + 1;
        }

        //Enviar los ingredientes a las colas correspondientes
        if (ing1 == TABACO) {
            mensaje.tipo = TABACO;
            mensaje.dato = 0;
            msgsnd(id_cola_tabaco, &mensaje, sizeof(struct mensaje) - sizeof(long), 0);
        } else if (ing1 == PAPEL) {
            mensaje.tipo = PAPEL;
            mensaje.dato = 0;
            msgsnd(id_cola_papel, &mensaje, sizeof(struct mensaje) - sizeof(long), 0);
        } else {
            mensaje.tipo = FOSFOROS;
            mensaje.dato = 0;
            msgsnd(id_cola_fosforos, &mensaje, sizeof(struct mensaje) - sizeof(long), 0);
        }
        if (ing2 == TABACO) {
            mensaje.tipo = TABACO;
            mensaje.dato = 0;
            msgsnd(id_cola_tabaco, &mensaje, sizeof(struct mensaje) - sizeof(long), 0);
        } else if (ing2 == PAPEL) {
            mensaje.tipo = PAPEL;
            mensaje.dato = 0;
            msgsnd(id_cola_papel, &mensaje, sizeof(struct mensaje) - sizeof(long), 0);
        } else {
            mensaje.tipo = FOSFOROS;
            mensaje.dato = 0;
            msgsnd(id_cola_fosforos, &mensaje, sizeof(struct mensaje) - sizeof(long), 0);
        }
    }
}
