#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

int main(int argc, char *argv[]){
	int turno = atoi(argv[1]);
    int shmid; // Identificador del segmento de memoria compartida
    int *ptr; // Puntero al segmento de memoria compartida

    int clave=ftok("/home/juan/PCCD",33);
    shmid = shmget(clave,sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    //printf("Zona de memoria: %d\n",shmid);
    // Se une el proceso al segmento de memoria compartida
    ptr = (int *) shmat(shmid, NULL, 0);
    if (ptr == (int *) -1) {
        perror("shmat");
        exit(1);
    }
    while(1){
        printf( "Caminando por mi habitación");
        if(getchar() == '\n'){
        printf("Intentando entrar en mi Sección Crítica...\n");
        }
        if(turno==*ptr){
            printf("Dentro de mi Sección Crítica.\n");
            if(getchar() == '\n') {
                printf("He salido de mi sección crítica\n");
            }
            getchar();
            if(*ptr == 0){ *ptr = 1;}
                    else{ *ptr = 0;}
                    printf("He accionado el pulsador\n");
                    continue;
            

            
        }else{
                    printf("Puerta cerrada\n");
                    continue;
        }
    }

    // Se desune el proceso del segmento de memoria compartida
    if (shmdt(ptr) == -1) {
        perror("shmdt");
        exit(1);
    }
    return 0;
}
