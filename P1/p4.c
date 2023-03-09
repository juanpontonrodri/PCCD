#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

int n1=0;
int n2=0;
int term=0;

void sighandler(int sig){
	int c;
	switch(sig){
	
	case 10:
		n1++;
		break;
	case 12:
		n2++;
		break;
	case 15:
		term=1;

		printf("SIGUSR1 recibidas: %i\n",n1);
		printf("SIGUSR2 recibidas: %i\n",n2);
		
		return;
	
}}

int main(){
	

	struct sigaction sigAction;
	sigAction.sa_handler=sighandler;

	sigaction(10, &sigAction, NULL);
	sigaction(12, &sigAction, NULL);
	sigaction(15, &sigAction, NULL);
	while(!term){
		 pause();
	}
	return 0;
}




