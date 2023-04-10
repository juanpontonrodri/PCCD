#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
struct msgbuf {
    long mtype;
    int tenedorIzq;
    int tenedorDer;
};

#define N 5

int main(int argc, char *argv[]) {
  int i = atoi(argv[1]);
  int IDColaMensajes = atoi(argv[2]);

  struct msgbuf msgIzq, msgDer;

  // Filósofo coge el tenedor de su izquierda
  if (msgrcv(IDColaMensajes, &msgIzq, sizeof(struct msgbuf) - sizeof(long), i, IPC_NOWAIT) == -1) {
      printf("[Filósofo %d] -> Error al recibir el tenedor de su izquierda.\n", i);
      exit(EXIT_FAILURE);
  }
  printf("[Filósofo %d] -> Ha cogido el tenedor de su izquierda.\n", i);

  // Filósofo coge el tenedor de su derecha
  if (msgrcv(IDColaMensajes, &msgDer, sizeof(struct msgbuf) - sizeof(long), (i % N) + 1, IPC_NOWAIT) == -1) {
      printf("[Filósofo %d] -> Error al recibir el tenedor de su derecha.\n", i);
      exit(EXIT_FAILURE);
  }
  printf("[Filósofo %d] -> Ha cogido el tenedor de su derecha.\n", i);

  // Filósofo come durante un tiempo aleatorio
  printf("[Filósofo %d] -> COMIENDO...\n", i);
  sleep(5);

  // Filósofo suelta el tenedor de su izquierda
  msgIzq.tenedorIzq = i;
  if (msgsnd(IDColaMensajes, &msgIzq, sizeof(struct msgbuf) - sizeof(long), IPC_NOWAIT) == -1) {
      printf("[Filósofo %d] -> Error al soltar el tenedor de su izquierda.\n", i);
      exit(EXIT_FAILURE);
  }
  printf("[Filósofo %d] -> Ha soltado el tenedor de su izquierda.\n", i);

  // Filósofo suelta el tenedor de su derecha
  msgDer.tenedorDer = (i % N) + 1;
  if (msgsnd(IDColaMensajes, &msgDer, sizeof(struct msgbuf) - sizeof(long), IPC_NOWAIT) == -1) {
      printf("[Filósofo %d] -> Error al soltar el tenedor de su derecha.\n!", i);
  }
}