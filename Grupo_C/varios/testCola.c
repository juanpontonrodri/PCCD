#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int cola(int id)
{
    key_t clave;
    clave = ftok("./", id);
    if (clave == (key_t)-1)
    {
        printf("Error al obtener la clave.\n");
        exit(0);
    }
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
#define MAX_PROCESOS 10000

#define KEY 0x011000a7
struct Testigo
{
    long mtype;
    int IDNodoOrigen;
    int atendidas_testigo[100][MAX_PROCESOS];
};

int main(int argc, char *argv[])
{
    int msgid = argv[1];
    struct Testigo testigo_send, testigo_recv;

    printf("msgid: %d", msgid);
    // Inicializar la estructura Testigo de envío
    testigo_send.mtype = argv[2];
    testigo_send.IDNodoOrigen = 1;
    testigo_send.atendidas_testigo[0][0] = 1;

    // Enviar la estructura Testigo
    if (msgsnd(msgid, &testigo_send, sizeof(testigo_send) - sizeof(long), 0) == -1)
    {
        perror("msgsnd");
        exit(1);
    }

    printf("Mensaje enviado:\n");
    printf("  mtype: %ld\n", testigo_send.mtype);
    printf("  IDNodoOrigen: %d\n", testigo_send.IDNodoOrigen);
    printf("  atendidas_testigo[0]: %d\n", testigo_send.atendidas_testigo[0]);
    printf("  atendidas_testigo[1]: %d\n", testigo_send.atendidas_testigo[1]);

    /*
        // Recibir la estructura Testigo
        if (msgrcv(msgid, &testigo_recv, sizeof(testigo_recv) - sizeof(long), 1, 0) == -1)
        {
            perror("msgrcv");
            exit(1);
        }

        printf("Mensaje recibido:\n");
        printf("  mtype: %ld\n", testigo_recv.mtype);
        printf("  IDNodoOrigen: %d\n", testigo_recv.IDNodoOrigen);
        printf("  atendidas_testigo[0]: %d\n", testigo_recv.atendidas_testigo[0]);
        printf("  atendidas_testigo[1]: %d\n", testigo_recv.atendidas_testigo[1]);
     */
    return 0;
}
