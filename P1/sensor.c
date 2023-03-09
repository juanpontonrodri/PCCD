#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int pidasc;
int piso;

void sighandler(int sig){
	switch(sig){
	case 10:
	sleep(3);
	printf("Se ha activado el sensor del piso %i\n",piso);
	kill(pidasc,14);
	break;
	}
}

int main(int argc,char* argv[]){

	piso=atoi(argv[1]);
	printf("%d",getpid());
	printf("introduce el pid del ascensor:");
	scanf("%i",&pidasc);
	struct sigaction sigAction;
	sigAction.sa_handler=sighandler;
	sigaction(10, &sigAction,NULL);
	while(1){
	pause();
	}	
	return 0;
	

}
