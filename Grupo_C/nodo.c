#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <signal.h>


//Cuando salen todos los lectores, se pide el testigo con la máxima prioridad para actualizar que no hay nadie dentro de la SC.
#define SALIDA_SC_LECTORES 4

//Prioridades de los procesos
#define PAGO_ANULACION 3
#define PRERRESERVA 2
#define GRADA_EVENTO 1

//Nodos que van a participar en el sistema.
#define CANTIDAD_NODOS 2

typedef struct lista {
    int PID; //PID del proceso de la lista
    struct lista *siguiente; //Puntero al siguiente proceso de la lista
} Lista;

typedef struct mensaje {
    long tipoProceso; //Tipo de proceso
    int PID; //PID del proceso
    int ES; //Proceso de entrada () o de salida ()
} Mensaje;

//Mensaje usado para pedir el testigo al nodo que lo tenga
typedef struct testigo {
    long IDNodoDestino; //ID del nodo al que vamos a enviar el mensaje (todos, porque no sabemos dónde está)
    int IDNodoOrigen; //ID del nodo del que solicita el testigo
    int prio; //Prioridad del proceso que quiere entrar
    int contadorPeticiones; //Número de peticiones que ha hecho el nodo
} Testigo;

//Estructura usada para enviar el testigo junto con los parametros correspondientes (vectores leyendo y peticiones atendidas)
typedef struct paramTestigo{
    long mtype;
    long IDNodoDestino;
    int vectorLeyendo[CANTIDAD_NODOS+1];
    int vectorAtendidas[CANTIDAD_NODOS+1];
} ParamTestigo;

//En las listas se van guardando las peticiones pendientes. El 3 viene de los 3 DEFINE que hay arriba con los distintos tipo de procesos.
Lista *encabezado[3]; //Referencia al primer elemento de la lista
Lista *lista[3]; //Lista como tal.

int contadorPeticiones = 0; //Número de peticiones que ha realizado el nodo.
int lectores, escribiendo; //numero de lectores y escritores que hay en la SC.

//Número de peticiones de cada tipo de proceso que están pendientes de entrar a la SC. Tenemos 3 tipos de proceso, el 4º () se utiliza para avisar de que ya no hay lectores (máx prio). quiere[0] no se utiliza, el resto coinciden con las etiquetas que están definidas arriba (1, 2, 3, 4).
int quiere[5]={0,0,0,0,0};

//Vectores del testigo informan de la prioridad de los nodos y del estado de la lectura en cada nodo(1 activada, 0 desactivada)
int vectorPeticiones[CANTIDAD_NODOS+1],vectorLeyendo[CANTIDAD_NODOS+1],vectorAtendidas[CANTIDAD_NODOS+1],vectorPrioridades[CANTIDAD_NODOS+1];

//Semáforos utilizados.
sem_t semaforoEspera;


void *nodeManagement();
void *recepcion();
void askToken();
void pendingMessages();
void addList(int, Mensaje, int);
void asignarTestigo();
void sendToken(int);

int myID, IDColaIntranodo, IDColaInternodo, IDColaTestigo, testigo, IDColaTestigoAux;

int main (int argc, char *argv[]){
    int i, leer;
    pthread_t hiloIntranodo, hiloRecepcionTestigo;
    Testigo testigoSt;

    key_t Clave;

    if (argc != 2){
        printf("Por favor, introduzca el ID del nodo como primer argumento.\n");
        exit(0);
    }

    myID = atoi(argv[1]);

    //Le damos el testigo al primer nodo, por ejemplo
    if (myID == 1) testigo = 1;
    else testigo = 0;

    //Crea buzón para los mensajes intranodo
    Clave = ftok("/bin/ls", myID);
    if (Clave == (key_t) -1){
        printf("Error al obtener la clave.\n");
        exit(0);
    } else {
        IDColaIntranodo = msgget (Clave, 0666 | IPC_CREAT);

        if(IDColaIntranodo == -1){
            printf("La invocación a 'msgget()' ha fallado.\n");
            exit(0);
        } else {
            printf("Se ha creado el buzón de mensajes (Intranodo) correctamente, con el ID %i.\n",IDColaIntranodo);
        }
    }

    //Buzón internodo
    Clave = ftok("/bin/ls", 'e');
    if (Clave == (key_t) -1){
        printf("Error al obtener la clave.\n");
        exit(0);
    } else {
        IDColaInternodo = msgget (Clave, 0666 | IPC_CREAT);

        if(IDColaInternodo == -1){
            printf("La invocación a 'msgget()' ha fallado.\n");
            exit(0);
        } else {
            printf("Se ha creado el buzón de mensajes (Internodo) correctamente, con el ID %i.\n",IDColaInternodo);
        }
    }

    //Buzón mensajes del testigo
    Clave = ftok("/bin/ls", 't');
    if (Clave == (key_t) -1){
        printf("Error al obtener la clave.\n");
        exit(0);
    } else {
        IDColaTestigo = msgget (Clave, 0666 | IPC_CREAT);
        IDColaTestigoAux = IDColaTestigo;
        if(IDColaTestigo == -1){
            printf("La invocación a 'msgget()' ha fallado.\n");
            exit(0);
        } else {
            printf("Se ha creado el buzón de mensajes (Testigo) correctamente, con el ID %i.\n",IDColaTestigo);
        }
    }
sem_init(&semaforoEspera, 0, 1);
    //Hilo de recepción de mensajes intranodo y recepción de testigo (tiene que realizarse en paralelo al funcionamiento del programa)
    pthread_create(&hiloIntranodo, NULL, nodeManagement, NULL);
    pthread_create(&hiloRecepcionTestigo, NULL, recepcion, NULL);

	

    //Esperando a recibir una petición del testigo en la cola internodo
    while(1){
        printf("           TESTIGO = %i\n",testigo);
        while(testigo == 0);
        printf("Esperando a recibir una petición en la cola internodo con ID '%i'.\n", IDColaInternodo);
        if (msgrcv(IDColaInternodo, &testigoSt, (sizeof(Testigo) - sizeof(long)), 0, 0) == -1){
          printf("Hubo un error recibiendo los mensajes en el nodo (Nodo = %i) en la cola internodo.\n", myID);
          exit(0);
        } else {
          printf("Se ha recibido una petición de testigo en la cola internodo con ID '%i'.\n", IDColaInternodo);
   /************************************************************
    *********************************************************
    **************SE QUEDA AQUI PARADO*******************************************
    ***********************************************************
    *******************************************************/
        sem_wait(&semaforoEspera);
          printf("  Prioridad de la peticion de testigo = %i\n",testigoSt.prio);
          //Si el ticket de petición entrante es mayor que la última que teníamos
          if(vectorPeticiones[testigoSt.IDNodoOrigen] < testigoSt.contadorPeticiones) {
            //nos quedamos con la petición
            vectorPrioridades[testigoSt.IDNodoOrigen] = testigoSt.prio;
            vectorPeticiones[testigoSt.IDNodoOrigen] = testigoSt.contadorPeticiones;
            if(testigo == 1){
              switch(testigoSt.prio){
                case GRADA_EVENTO:
                  leer = 1;
                  printf(" ---- entra en el case grada-evento\n");
                  //Si la petición es un lector y no hay ninguna petición más prioritaria o alguien escribiendo pasamos testigo
                  for(i = 1; i < CANTIDAD_NODOS+1; i++){
                    if(((vectorPeticiones[i] > vectorAtendidas[i]) && (GRADA_EVENTO < vectorPrioridades[i]) && (vectorPrioridades[i] < SALIDA_SC_LECTORES)) || (escribiendo == 1)){
                      leer = 0;
                      printf(" --------- se entra en leer = 0");
                      break;
                    }
                  }
                  if(leer == 1){
                      printf("Nos ha llegado una peticion de lectura de otro nodo, le enviamos el testigo (Nodo destino = %i)\n",testigoSt.IDNodoOrigen);
                    sendToken(testigoSt.IDNodoOrigen);
                  }
                  break;

                case PAGO_ANULACION:
                case PRERRESERVA:
                    leer = 0;
                    //Si la petición es un escritor y no hay ningún nodo leyendo
                    for(i = 1; i < CANTIDAD_NODOS+1; i++){
                      if(vectorLeyendo[i] == 1){
                        leer = 1; // HEMOS MODIFICADO ESTO *****************************************************************
                        break;
                      }
                    }
                    //y ningún proceso escribiendo en el nodo y el ticket es superior pasamos el testigo
                    if(vectorPeticiones[testigoSt.IDNodoOrigen] > vectorAtendidas[testigoSt.IDNodoOrigen]){
                        if(escribiendo == 0 && leer == 0){
                          sendToken(testigoSt.IDNodoOrigen);
                        }
                    }
                    break;

                  case SALIDA_SC_LECTORES:
                      printf("   Case SALIDA_SC_LECTORES\n\n");
                    //Si la petición es de salida de lectores, el ticket es superior y no hay nadie escribiendo pasamos el testigo
                    if(vectorPeticiones[testigoSt.IDNodoOrigen] > vectorAtendidas[testigoSt.IDNodoOrigen]){
                        if(escribiendo != 1){
                          sendToken(testigoSt.IDNodoOrigen);
                        }
                    }
                    break;
              }
            }
          }
        }
        sem_post(&semaforoEspera);
    }
    return 0;
}

void *nodeManagement(){

    Mensaje msg;
    int i, k, alguienLeyendo=0, longitud=(sizeof(Mensaje) - sizeof(long));
    while(1){
    printf("           TESTIGO = %i\n",testigo);
    printf("Esperando a recibir un mensaje en la cola intranodo IDColaIntranodo %i\n",IDColaIntranodo);
    if(msgrcv(IDColaIntranodo, &msg, longitud, 0, 0) == -1){
        printf("Error en la recepción del mensaje intranodo (nodeManagement()).\n");
        exit(0);
    }

    printf("Ha entrado un mensaje intranodo.\n");

    //Procedemos a bloquear para que no se altere la ejecución de la comprobación
    sem_wait(&semaforoEspera);
    int tipoPeticion = msg.tipoProceso;
	printf("Tipo de mensaje intranodo (Entrada/Salida) -> %i\n   - tipoPeticion = %i\n",msg.ES, tipoPeticion);
    if (msg.ES == 1) { //SE COMPRUEBA SI ES UN MENSAJE DE PETICION DE ENTRADA EN SC
        int peticionSuperior=0, prioridadesSuperiores;

        if(tipoPeticion>=2) {// los procesos escritores del 4 al 3

            printf("Solicitud de entrada a la SC por un proceso escritor (PAGO, ANULACIÓN o PRERRESERVA).\n");
            printf("           TESTIGO = %i\n",testigo);
            if(testigo == 1){//si tienes el testigo no competir por la contienda global porque es el mas prioritario y tienes el testigo
                //asegurarse de que no hay lectores en ningun nodo ni escritores escribiendo (en tu mismo nodo porque tienes el testigo)
                //en ese caso despertar al escritor
                //en caso de haber escritores o lectores se encola el nuevo pago

                if(quiere[tipoPeticion] == 0 ){
                    alguienLeyendo=0;
                    for(k=1; k<CANTIDAD_NODOS+1; k++){
                        if(vectorLeyendo[k]==1){
                            alguienLeyendo=1;
                            break;
                        }
                    }

                    if((escribiendo == 1) || (alguienLeyendo == 1)){
                        addList(tipoPeticion,msg,1);
                        //Se comprueba que procesos con prioridades superiores no quieran entrar en la SC
                        for (prioridadesSuperiores = 3; prioridadesSuperiores == tipoPeticion; prioridadesSuperiores--) {
                            if(quiere[prioridadesSuperiores]==1){
                                peticionSuperior = 1;
                                break;
                            }
                        }

                        if(alguienLeyendo == 1 && peticionSuperior==0){
                            if(testigo == 1){ //**********************************************************
                                quiere[tipoPeticion]++;
                            }else{
                                quiere[tipoPeticion]++;
                                askToken(tipoPeticion);
                            }
                        }else quiere[tipoPeticion]++;
                    }else{
                        escribiendo=1;
                        kill(msg.PID, SIGUSR1);
                    }
                }else{
                    addList(tipoPeticion,msg,2);
                    quiere[tipoPeticion]++;
                }
            }else{ //testigo = 0

                for (prioridadesSuperiores = 3; prioridadesSuperiores >= tipoPeticion; prioridadesSuperiores--){
                    if(quiere[prioridadesSuperiores]==1){
                        peticionSuperior = 1;
                        break;
                    }
                }

                if (quiere[tipoPeticion] == 0 && peticionSuperior == 0) {	//SE AGREGA A LA LISTA DE PAGOS
                    addList(tipoPeticion,msg,1);
                    askToken(tipoPeticion); //El testigo solo lo pide el primer pago de la lista (los demas no ganarian la contienda) (se mantiene el testigo hasta atender a todos los pagos)
			printf("No tenemos el testigo, lo pedimos\n");               
		 } else {
                    addList(tipoPeticion,msg,2);
			printf("No tenemos el testigo, ya se ha pedido\n");               
                }
                quiere[tipoPeticion]++;
            }
        }else if (tipoPeticion == GRADA_EVENTO) { //procesos lectores
            printf("Solicitud de entrada a la SC por un proceso lector (EVENTOS o GRADAS)\n");
            if(testigo == 1){
                if(quiere[tipoPeticion] == 0 ){ //primer lector
                    i=0;
                    int quiere_escribir = 0;
                    for(i=0; i<CANTIDAD_NODOS+1 ;i++){
                        if(vectorPeticiones[i] > vectorAtendidas[i] && vectorPrioridades[i] > 1){
                            quiere_escribir = 1;
                            break;
                        }
                    }
                    for (prioridadesSuperiores = 3; prioridadesSuperiores > 1 ; prioridadesSuperiores--) {
                        if(quiere[prioridadesSuperiores]>0){
                            peticionSuperior = 1;
                            break;
                        }
                    }
                    if(escribiendo == 1 || peticionSuperior==1 || quiere_escribir == 1){
                        addList(GRADA_EVENTO,msg,1);
                        quiere[GRADA_EVENTO]++;
                    }else{
                        // despertamos al proceso lector
                        lectores++; //variable que lleva la cuenta dentro de un nodo de los lectores en la seccion critica
                        vectorLeyendo[myID]=1;
                        kill(msg.PID, SIGUSR1);
                    }

                }else{//quiere_lectores!=0
                    addList(GRADA_EVENTO,msg,2);
                    quiere[GRADA_EVENTO]++;
                }
            }else{ //testigo = 0
                if(quiere[GRADA_EVENTO] == 0){
                    for (prioridadesSuperiores = 3; prioridadesSuperiores == 2; prioridadesSuperiores--) {
                        if(quiere[prioridadesSuperiores]==1){
                            peticionSuperior = 1;
                            break;
                        }
                    }
                    if (peticionSuperior==0){
                        printf("No hay peticiones de escritores pendientes de ser atendidas, somos lectores y pedimos el testigo\n");
                        askToken(GRADA_EVENTO); //El testigo solo lo pide el primer reserva de la lista (los demas no ganarian la contienda) (se mantiene el testigo hasta atender a todos los pagos)
                    } 	//SE AGREGA A LA LISTA DE PAGOS
                    addList(GRADA_EVENTO,msg,1);
                } else {
                    addList(GRADA_EVENTO,msg,2);
                }
                quiere[GRADA_EVENTO]++;
            }
        }
    } else{
        printf("           TESTIGO = %i\n",testigo);
        if(tipoPeticion>=2){
            printf("Ha salido un proceso escritor tipo %i.\n", tipoPeticion);
            escribiendo = 0;
            asignarTestigo();
        }else{
            printf("Ha salido un proceso lector tipo %i.\n", tipoPeticion);
            lectores--;
            printf("Total de procesos lectores -> %i\n",lectores);

            if (lectores == 0) {
                vectorLeyendo[myID]=0; //indicamos en el vector del testigo que no hay nadie en ese nodo leyendo

                i=0, alguienLeyendo=0; //variable que indica si hay alguien leyendo en algun nodo

                for(i=1 ; i<CANTIDAD_NODOS+1 ; i++){
                    if(vectorLeyendo[i]==1){
                        alguienLeyendo=1;
                        break;
                    }
                }

                if(alguienLeyendo == 0){
                    if(testigo==1) asignarTestigo();
                    else askToken(SALIDA_SC_LECTORES);
                }else{
                    if(testigo==0) askToken(SALIDA_SC_LECTORES);
                }
            }else{// ***********************************************************************
                if(testigo == 1){
                    quiere[SALIDA_SC_LECTORES]=1;
                }else{
                    quiere[SALIDA_SC_LECTORES]=1;
                    askToken(SALIDA_SC_LECTORES);//pedir el testigo para actualizar el vector de lectores, con la maxima prioridad   
 
                }
            }
        }
    }

        sem_post(&semaforoEspera);
}
    return NULL;

}

void addList(int prioridad, Mensaje msg, int primero){

    //Prioridad: nos indica si es de PAGO_ANULACION, PRERRESERVA o GRADA_EVENTO
    //msg: es el mensaje a añadir en la lista
    //primero: nos indica si es el primer proceso de la lista (1) o si no lo es(0)

    if(primero == 1){//primer nodo de la lista enlazada
        encabezado[prioridad] = (Lista *) malloc(sizeof(Lista));
        lista[prioridad] = encabezado[prioridad];
        lista[prioridad] -> PID = msg.PID;
        lista[prioridad] -> siguiente = NULL;
    } else { //No es el primer nodo de la lista enlazada
        lista[prioridad] -> siguiente = (Lista *) malloc(sizeof(Lista));
        lista[prioridad] = lista[prioridad] -> siguiente;
        lista[prioridad] -> PID = msg.PID;
        lista[prioridad] -> siguiente = NULL;
    }

    return;
}

void askToken(int prioridadNodo){
    Testigo peticionTestigo;
    int i = 0;
    // Aumentamos el contador de peticiones del nodo
    contadorPeticiones++;
    // Rellenamos la peticion con los valores correspondientes
    peticionTestigo.IDNodoOrigen = myID;
    peticionTestigo.prio = prioridadNodo;
    peticionTestigo.contadorPeticiones = contadorPeticiones;
    // Enviamos peticion a todos los nodos
    for(i = 1; i < CANTIDAD_NODOS + 1; i++){
	if(i!=myID){
            printf("Solicitamos el testigo al resto de nodos\n");
		peticionTestigo.IDNodoDestino = i;
		if(msgsnd(IDColaInternodo, &peticionTestigo, (sizeof(Testigo) - sizeof(long)), 0) == -1){
		    printf("Error al pedir testigo\n");
		    exit(1);
		}
	}
    }
    return;
}

void pendingMessages(){
    int i = 0;
    // Comprobamos que hay procesos que piden entrar a la seccion critica (por orden de prioridad)
    for(i = PAGO_ANULACION; i >= GRADA_EVENTO ; i--){
        // En cuanto nos encontremos un proceso que quiere entrar, enviamos el testigo al resto de nodos y dejamos de comprobar
        if(encabezado[i] != NULL){ 
            askToken(i);
            // Dejamos de buscar procesos en espera y salimos del bucle
            break;
        }      
    }
    return;
}

void asignarTestigo(){

    int PID, i, k, j, aux=0;
    //Se comprueba si hay alguna nueva peticion de salida de lectores de la seccion critica en algun nodo. Si la hay se le envia directamente porque no puede haber escritores en la SC
    for(k=1; k<CANTIDAD_NODOS+1; k++){
        if((vectorPeticiones[k] > vectorAtendidas[k]) && (vectorPrioridades[k] == SALIDA_SC_LECTORES)){
            sendToken(k);
            printf("Enviamos el testigo al nodo -> %i\n",k);
            return;
        }
    }
    //Si no hay peticiones de salida de lectores de la SC, procedemos a comprobar, por orden de prioridad, si hay peticiones de los demás tipos.
    for(k=PAGO_ANULACION; k>=GRADA_EVENTO; k--){

        if(quiere[k] != 0){ //Si hay peticiones del tipo de proceso que ejecuta el FOR
            PID = encabezado[k]->PID; //Sacamos el PID del primer proceso de la lista que está esperando por la SC
            if(k != GRADA_EVENTO){ //Si no es un proceso lector, comprobamos que no haya lectores en la SC, pues para que un escritor entre, deben estar todos fuera.
                aux=0;
                for(j=1; j<CANTIDAD_NODOS+1; j++){
                    if(vectorLeyendo[j] == 1){ //Si hay un lector, abortamos; tenemos que esperar a que terminen los lectores todos.
                        aux=1;
                        break;
                    }
                }

                //Si no hay lectores, procedemos a asignar el testigo a quien corresponda
                if(aux == 0){
                    quiere[k]--; //Decrementamos el vector, pues se habrá atendido una petición
                    escribiendo=1; //Activamos el flag de escritores
                    kill(PID,SIGUSR1); //Enviamos una señal (conforme puede entrar a la SC)
                    encabezado[k] = encabezado[k] -> siguiente; //Actualizamos la lista.
                }
            } else {
                //Si es un proceso lector el de mayor prioridad que solicita la SC, le dejo entrar
                while(encabezado[k] != NULL){
                    lectores++;
                    vectorLeyendo[myID] = 1;
                    kill(encabezado[k]->PID,SIGUSR1);
                    encabezado[k]=encabezado[k]->siguiente;
                    quiere[k]--;
                }

                //Si hay en algún nodo un proceso lector que quiera entrar, le pasamos el Token para que pueda hacerlo.
                for(j=1; j<CANTIDAD_NODOS+1; j++){
                    if((vectorPeticiones[j]>vectorAtendidas[j]) && (vectorPrioridades[j] == GRADA_EVENTO)){
                        sendToken(j);
                        break;
                    }
                }
            }
            return;

        } else { //En este nodo no hay PAGOS_ANULACIONES ni PRERRESERVAS, por lo que comprobamos si los hay en otros nodos.
            for(j=1; j<CANTIDAD_NODOS+1; j++){

                if((vectorPeticiones[j] > vectorAtendidas[j]) && (vectorPrioridades[j] == k)){
                    aux=0;
                    //Si hay alguien leyendo, no hago nada
                    for(i=1; i<CANTIDAD_NODOS+1; i++){
                        if(vectorLeyendo[i] == 1){
                            aux=1;
                            break;
                        }
                    }

                    if(aux == 0 || k == GRADA_EVENTO){
                        sendToken(j);

                        for(i=PAGO_ANULACION; i>=GRADA_EVENTO; i--){
                            if(quiere[i]>0){
                                askToken(i);
                                break;
                            }
                        }
                    }
                    return;
                }
            }
        }
    }
    return;
}

// Envía el testigo actualizando los vectores de cada nodo
void sendToken(int IDnodoDestino){
    // Ya no tenemos el testigo, por lo que ponemos el valor a 0
    testigo = 0;
    ParamTestigo envioTestigo;
    envioTestigo.IDNodoDestino = IDnodoDestino;
    envioTestigo.mtype = 1;
    int i = 0;
    // Al nodo que va a recibir el testigo le mandamos la informacion necesaria (vectores leyendo y peticiones atendidas)
    for(i = 1; i < CANTIDAD_NODOS + 1; i++){
        envioTestigo.vectorLeyendo[i] = vectorLeyendo[i];
        envioTestigo.vectorAtendidas[i] = vectorAtendidas[i];
    }
    printf("Enviamos el testigo al nodo %i\n",IDnodoDestino);
    int mss = msgsnd(IDColaTestigoAux, &envioTestigo, (sizeof(ParamTestigo) - sizeof(long)), 0);
	if(mss == -1){
	        printf("Error al enviar el testigo\n");
	        exit(1);
    	}
    // Despues de pedir el testigo, comprobamos si tenemos procesos por atender en nuestro nodo (pediremos el testigo en el caso de que los haya)
    pendingMessages();
}

void *recepcion(){

    ParamTestigo s_testigo;
    int i=0;

    while (1) {
        
       if(testigo ==1) printf("Tengo el testigo, no espero por el\n");
        while(testigo==1);

        printf("Esperando a recibir el testigo en la cola con ID '%i'.\n", IDColaTestigo);
       int msss = msgrcv(IDColaTestigoAux, &s_testigo, (sizeof(ParamTestigo) - sizeof(long)), 0, 0);
        if (msss == -1){	//Miramos si hay mensajes en la cola (intranodo)
            printf("Error en la recepcion del mensaje intranodo en la función 'recepcion()'.\n");
            exit(1);
        }
        printf("Se ha recibido el testigo en el nodo con el ID '%i'.\n", myID);

        sem_wait(&semaforoEspera);
        testigo=1;

        //Actualizamos el vector de peticiones atendidas con lo que nos llega en el token. 
        //Véase que esto lo hacemos nada más recibimos el testigo, para tener en cuenta la posiblidad de que lleguen 
        //peticiones más prioritarias en el momento en el que el testigo 'no es de nadie' (en cuyo caso, se produciría un error).
        for(i=0;i<CANTIDAD_NODOS+1;i++){
            vectorAtendidas[i] = s_testigo.vectorAtendidas[i];
            vectorLeyendo[i] = s_testigo.vectorLeyendo[i];
        }
        vectorAtendidas[myID] = contadorPeticiones; //Cuando tengo el testigo, me acaban de atender -> actualizo el vector de peticiones atendidas.


        for(i = SALIDA_SC_LECTORES ; i >= GRADA_EVENTO ; i--){

		//Cuando el último lector acaba, usa el testigo para decir que acabó y lo suelta inmediatamente
                if((quiere[SALIDA_SC_LECTORES] > 0) && (i == SALIDA_SC_LECTORES)){
                    vectorLeyendo[myID] = 0;
                    quiere[SALIDA_SC_LECTORES]=0;
                    asignarTestigo();
                    break;

		//Si no, comprobamos si hay procesos lectores que quieran acceder a la SC
                }else if ((quiere[GRADA_EVENTO] > 0) && (i == GRADA_EVENTO)){
			//Si los hay, los atendemos a todos.
                    while(encabezado[GRADA_EVENTO] != NULL){
                        quiere[GRADA_EVENTO]--;
                        lectores++;
                        vectorLeyendo[myID] = 1;
                        kill(encabezado[GRADA_EVENTO] -> PID, SIGUSR1);
                        encabezado[GRADA_EVENTO] = encabezado[GRADA_EVENTO] -> siguiente;

                    }
			//Una vez atendidos, reenviamos el testigo.
                    asignarTestigo();

		
                }else if ((quiere[i] > 0) && (i != GRADA_EVENTO) && (i != SALIDA_SC_LECTORES)){

                    int j;
                    int leyendo = 0;
			
			//Comprobamos en todos los nodos si hay o no algún lector
                    for(j=1;j<CANTIDAD_NODOS+1;j++){
                        if(vectorLeyendo[j] == 1){
                            leyendo = 1;
                            break;
                        }
                    }

			//Si no hay lectores, despertamos al proceso más prioritario correspondiente y lo atendemos
                    if(leyendo == 0){
                        escribiendo=1;
                        kill(encabezado[i] -> PID, SIGUSR1);
                        encabezado[i] = encabezado[i]->siguiente;
                        quiere[i]--;
                        break;
                    }
                }
        }
        sem_post(&semaforoEspera);
    }
}
