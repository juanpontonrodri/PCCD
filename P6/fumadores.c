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

void fumador(int tipo_fumador) {
    int tipo_ingrediente1, tipo_ingrediente2, id_cola1, id_cola2;
    struct mensaje mensaje;
    switch (tipo_fumador) {
        case TABACO:
            tipo_ingrediente1 = PAPEL;
            tipo_ingrediente2 = FOSFOROS;
            id_cola1 = msgget(23, 0600 | IPC_CREAT);
            id_cola2 = msgget(24, 0600 | IPC_CREAT);
            printf("[Fumador Tabaco]-> Soy fumador de tabaco\n");
            break;
        case PAPEL:
            tipo_ingrediente1 = FOSFOROS;
            tipo_ingrediente2 = TABACO;
            id_cola1 = msgget(24, 0600 | IPC_CREAT);
            id_cola2 = msgget(22, 0600 | IPC_CREAT);
            printf("[Fumador Papel]-> Soy fumador de papel\n");
            break;
        case FOSFOROS:
            tipo_ingrediente1 = PAPEL;
            tipo_ingrediente2 = TABACO;
            id_cola1 = msgget(23, 0600 | IPC_CREAT);
            id_cola2 = msgget(24, 0600 | IPC_CREAT);

            printf("[Fumador Fósforos]-> Soy fumador de fósforos\n");
            break;
        default:
            printf("Error: tipo de fumador no válido\n");
            exit(1);
    }
    while (1) {
        if (msgrcv(id_cola1, &mensaje, sizeof(struct mensaje) - sizeof(long), tipo_ingrediente1, IPC_NOWAIT) != -1) {
            //Hay ingrediente 1, cogerlo y soltarlo
            msgsnd(id_cola1, &mensaje, sizeof(struct mensaje) - sizeof(long), tipo_ingrediente1);
            printf("[Fumador]-> Devuelto\n");
            if (msgrcv(id_cola2, &mensaje, sizeof(struct mensaje) - sizeof(long), tipo_ingrediente2, 0) != -1) {
                //Tengo ingrediente 1
                printf("[Fumador]-> Tengo ingrediente 1\n");

                if (msgrcv(id_cola2, &mensaje, sizeof(struct mensaje) - sizeof(long), tipo_ingrediente2, 0) != -1) {

                printf("[Fumador]-> Tengo ingrediente 2, fumando...\n");
                sleep(1);
                printf("[Fumador]-> Terminé de fumar\n");
                }
            }
            
        } else {
            //No hay ingrediente 1, esperar
            printf("[Fumador]-> No tengo ingrediente 1, esperando...\n");
        }
    }
    
}


int main(int argc, char *argv[]) {
    int tipo_fumador;
    srand(time(NULL));
    int id_cola = msgget(99, 0600 | IPC_CREAT);

    if (argc != 2) {
        printf("Error: debe especificar el tipo de fumador (1 para tabaco, 2 para papel, 3 para fósforos)\n");
        exit(1);
    }

    tipo_fumador = atoi(argv[1]);

    fumador(tipo_fumador);

    return 0;
}
