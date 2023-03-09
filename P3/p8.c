#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char *argv[]){
	int yo = atoi(argv[1]);
    int otro;
    int shmid; // Identificador del segmento de memoria compartida
    int *ptr; // Puntero al segmento de memoria compartida

    int clave=ftok("/home/juan",33);
    shmid = shmget(clave,2*sizeof(int), IPC_CREAT | 0666);
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

		if(yo == 0) otro = 1;
		else otro = 0;
		ptr[0] = 0;//Activamos los pulsadores
		ptr[1] = 0;

    while(1){
        printf( "Caminando por mi habitación");
        if(getchar() == '\n'){
            printf("Dentro del pasillo\n");
        }
        ptr[yo]=1;//quiero
        if(getchar() == '\n'){
 printf("He accionado el pulsador\n");        
 }
       

if(getchar() == '\n'){
            printf("Intentando acceder a SC\n");
        }
       // while(quiere1); es:
       while (ptr[otro] != 0) { // mientras la puerta esté cerrada
        printf("Puerta cerrada. Saliendo del Pasillo\n");
        if(getchar() == '\n'){
            printf("He accionado el pulsador\n");
                ptr[yo]=0;

        }
        printf("He salido del pasillo\n");

        continue;
    }
        if(ptr[otro]==0){
           
            printf("Dentro de mi Sección Crítica\n");

            
            if(getchar() == '\n') {
                printf("He salido de mi sección crítica\n");
            }
            
             if(getchar() == '\n') {
                ptr[yo]=0;
                printf("He accionado el pulsador\n");
            }

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
