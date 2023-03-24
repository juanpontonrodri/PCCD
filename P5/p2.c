#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

int main (int argc, char* argv[]){

	int IDColaMensajes;
	IDColaMensajes = atoi(argv[1]);
    if(msgctl(IDColaMensajes, IPC_RMID, NULL) == -1)
		printf("error\n");
	else
		printf("eliminada %i\n",IDColaMensajes);
	

	return 0;
}

