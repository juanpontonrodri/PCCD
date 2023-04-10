#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

int main (int argc, char* argv[]){

	key_t clave = ftok("/home/juan",5);

	if (clave == (key_t) -1){
		printf("Erro ftok\n");
		exit(0);
	} else {
		int IDColaMensajes = msgget(clave, 0777 | IPC_CREAT);
		if (IDColaMensajes == -1){
			printf("Error identificador\n");
			exit(0);
		} else {
			printf("id %i.\n", IDColaMensajes);
		}
	}

	return 0;


}

