#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char *argv[]) {
    int num = atoi(argv[1]); // Se convierte el argumento a entero
    int shmid; // Identificador del segmento de memoria compartida
    int *ptr; // Puntero al segmento de memoria compartida

    int clave=ftok("/home/juan/PCCD",33);
    shmid = shmget(clave,sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    printf("Zona de memoria: %d\n",shmid);
    // Se une el proceso al segmento de memoria compartida
    ptr = (int *) shmat(shmid, NULL, 0);
    if (ptr == (int *) -1) {
        perror("shmat");
        exit(1);
    }

    // Se escribe el número recibido como argumento en el segmento de memoria compartida
    *ptr = num;

    // Se desune el proceso del segmento de memoria compartida
    if (shmdt(ptr) == -1) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}
