#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


//Hago el array de 6 para hacerlo más cómodo, realmente sólo se utilizarán del 1 al 5 (el 0 no).
struct msgbuf{
	long mtype;
	int tenedor;
} tenedor[6];

void inicFilosofoTenedor (int);
void filosofoAComer(int, int);

int main(int argc, char* argv[]){

	int IDColaMensajes;
	key_t Clave;

	if(argc != 2){
		printf("Introduce un número entero como parámetro para crear el ID del buzón de mensajes.\n");
		exit(0);
	}

	Clave = ftok("/bin/ls", atoi(argv[1]));
	if (Clave == (key_t) -1){
		printf("Error al obtener la clave.\n");
		exit(0);
	} else {
		IDColaMensajes = msgget (Clave, 0777 | IPC_CREAT);

		if(IDColaMensajes == -1){
			printf("La invocación a 'msgget()' ha fallado.\n");
			exit(0);
		} else {
			printf("Se ha creado el buzón de mensajes correctamente, con el ID %i.\n",IDColaMensajes);
		}
	}

	int opcion;

	while (1){

		printf("¿Qué desea realizar?\n\n");
		printf("\t1. Introducir tenedores en el buffer.\n");
		printf("\t2. Introducir filósofo a la comida.\n");
		printf("\t3. Ver el estado de la cola de mensajes en el sistema.\n");
		printf("\t4. Salir.\n\n");

		printf("Por favor, introduzca su selección: ");

		scanf("%i", &opcion);

		switch(opcion){

			case 1: printf("\n");
				inicFilosofoTenedor(IDColaMensajes);
				break;

			case 2: printf("\nPor favor, introduzca el filósofo que desee introducir a la comida: ");
				scanf("%i", &opcion);
				filosofoAComer(opcion, IDColaMensajes);
				break;

			case 3: printf("\n");
				system("ipcs -q");
				break;

			case 4: exit(0);
				break;
		}
	}

	inicFilosofoTenedor(IDColaMensajes);
	return 0;
}

void inicFilosofoTenedor(int IDColaMensajes){

	int i;
	printf("Procedemos a meter los tenedores en el buffer...\n");

	for (i=1; i<6; i++){
		tenedor[i].mtype=i;
		tenedor[i].tenedor=i;


		if( (msgsnd(IDColaMensajes, &tenedor[i], sizeof(int), IPC_NOWAIT)) == -1){
			printf("La invocación a 'msgsnd()' ha fallado.\n");
			exit(0);
		} else {
			printf("[Tenedor %i] -> Introducido con éxito en el buffer.\n", i);
		}
	}
	printf("Todos los tenedores se han introducido en el buffer.\n");
}

void filosofoAComer (int i, int IDColaMensajes){

	int d; //Variable que controla el comportamiento del filósofo contiguo.

	printf("[Filósofo %i] -> Tratando de coger el tenedor %i...\n", i, i);
	msgrcv(IDColaMensajes, &tenedor[i], sizeof(int), i, 0);

	printf("[Filósofo %i] -> Se ha retirado el tenedor %i...\n", i, i);

	d=i-1; if(d == 0) d=5; //Se coge siempre primero el tenedor de la izquierda del filósofo.

	printf("[Filósofo %i] -> Tratando de coger el tenedor %i...\n", i, d);
	msgrcv(IDColaMensajes, &tenedor[d], sizeof(int), d, 0);

	printf("[Filósofo %i] -> Se ha retirado el tenedor %i...\n", i, d);

	printf("[Filósofo %i] -> COMIENDO...\n", i);
	sleep(6);

	printf("[Filósofo %i] -> Ha terminado de comer. Devolviendo los tenedores...\n", i);


	tenedor[i].mtype=i;
	tenedor[i].tenedor=i;

	tenedor[d].mtype=d;
	tenedor[d].tenedor=d;

	//Devolvemos los tenedores.
	msgsnd(IDColaMensajes, &tenedor[i], sizeof(int), IPC_NOWAIT);
	msgsnd(IDColaMensajes, &tenedor[d], sizeof(int), IPC_NOWAIT);
}
