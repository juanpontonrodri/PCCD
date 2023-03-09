#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

int sigHecho[64];
int term=0;

void sighandler(int sig){
	int c;
	switch(sig){
	
	case 15:
		term=1;
		sigHecho[sig]=1;

		printf("vector sig recibidas:");
		for(c=1;c<64;c++){
			printf("%i\n",sigHecho[c]);
		}
		
		return;
	default: sigHecho[sig]=1;
		 break;
}}

int main(){
	int a;
	for(a=0;a<64;a++){
	sigHecho[a]=0;
}

struct sigaction sigAction;
	sigAction.sa_handler=sighandler;
	int b;
	for(b=0;b<64;b++){
		sigaction(b,&sigAction,NULL);
	}
	while(!term){
		 pause();
	}
	return 0;
}




