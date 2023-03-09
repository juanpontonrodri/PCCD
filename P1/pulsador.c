#include<stdio.h>
#include<signal.h>
#include<stdlib.h>


int main(int argc, char* argv[]){
	int pid=atoi(argv[1]);
	int opcion;	

	do{
	printf("Introduzca un numero del 1 al 3:\n1. Subir\n2. Bajar\n3.Salir\n");
	scanf("%d",&opcion);
	switch(opcion){
		case 1: kill(pid,10);
			break;
		case 2: kill(pid,12);
			break;
		case 3: kill(pid,3);
			break;
	}
	}while(opcion!=3);
	return 0;

}
