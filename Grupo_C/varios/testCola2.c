#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define KEY 0x011000a7

int cola(int id)
{
    /*  key_t clave;
     clave = ftok("./", id);
     if (clave == (key_t)-1)
     {
         printf("Error al obtener la clave.\n");
         exit(0);
     } */
    key_t clave = id;
    int id_cola = msgget(clave, 0777 | IPC_CREAT);
    if (id_cola == -1)
    {
        perror("msgget");
        exit(-1);
    }
    printf("id_cola: %d\n", id_cola);
    printf("id: %d\n", id);
    printf("clave: %d\n", clave);
    return id_cola;
}

struct PeticionTestigo
{
    long mtype;
    int IDNodoOrigen;
    int numero_peticion;
};

int main()
{
    int msgid = cola(1);
    struct PeticionTestigo peticion_send, peticion_recv;

    // Inicializar la estructura PeticionTestigo de envío
    peticion_send.mtype = 1;
    peticion_send.IDNodoOrigen = 1;
    peticion_send.numero_peticion = 5;

    // Enviar la estructura PeticionTestigo
    if (msgsnd(msgid, &peticion_send, sizeof(peticion_send) - sizeof(long), 0) == -1)
    {
        perror("msgsnd");
        exit(1);
    }

    printf("Mensaje enviado:\n");
    printf("  mtype: %ld\n", peticion_send.mtype);
    printf("  IDNodoOrigen: %d\n", peticion_send.IDNodoOrigen);
    printf("  numero_peticion: %d\n", peticion_send.numero_peticion);

    printf("Presione Enter para continuar...\n");
    while (getchar() != '\n')
        ;
    // Recibir la estructura PeticionTestigo
    if (msgrcv(msgid, &peticion_recv, sizeof(peticion_recv) - sizeof(long), 1, 0) == -1)
    {
        perror("msgrcv");
        exit(1);
    }

    printf("Mensaje recibido:\n");
    printf("  mtype: %ld\n", peticion_recv.mtype);
    printf("  IDNodoOrigen: %d\n", peticion_recv.IDNodoOrigen);
    printf("  numero_peticion: %d\n", peticion_recv.numero_peticion);

    return 0;
}
