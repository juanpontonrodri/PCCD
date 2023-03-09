#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>



int sensores[10]={};
int nSube=0;
int nBaja=0;
int nPisos=0;
int pisoAct=0;
int mov=0;
int term=0;



void sighandler(int sig){
	int c;
	switch(sig){
	
	case 10:
		if(mov==1) return;
		if(pisoAct !=nPisos){
			mov=1;
			nSube++;
			pisoAct++;
			printf("Subiendo un piso\n");
			kill(sensores[pisoAct],10);
		} else {printf("piso maximo\n");
		}
		
		break;
	case 12:
		if(mov==1) return;
		if(pisoAct !=0){
			mov=1;
			nBaja++;
			pisoAct--;
			printf("Bajando un piso\n");
			kill(sensores[pisoAct],10);
		} else {printf("piso minimo\n");
		}
		
		break;
	case 14://sigalarm
		mov=0;
		printf("bienvenido al piso %i\n",pisoAct);
		break;
	case 3:


		printf("veces subidas: %i\n",nSube);
		printf("veces bajadas: %i\n",nBaja);
		term=1;
		
		return;
	
}}

int main(int argc,char* argv[]){
	
	nPisos=atoi(argv[1]);
	printf("pid:%d\n",getpid());
	int p;
	for(p=0;p<=nPisos;p++){
	printf("pid del piso%i?:\n",p);
	scanf("%i",&sensores[p]);
}

	struct sigaction sigAction;
	sigAction.sa_handler=sighandler;

	sigaction(10, &sigAction, NULL);
	sigaction(12, &sigAction, NULL);
	sigaction(14, &sigAction, NULL);
	sigaction(3, &sigAction, NULL);
	while(!term){
		pause();
		if(mov==1){
			printf("imprime algo\n");
			
			}
		}
	}
	return 0;
}




