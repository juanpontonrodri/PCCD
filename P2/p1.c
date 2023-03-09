#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

int contador = 0;

void handler(int signal)
{

    printf("SIGNAL SIGCHLD recibida. Ha finalizado un proceso hijo.\n");
    contador++;
}

int main()
{

    int i;

    for (i = 0; i < 3; i++)
    {

        switch (fork())
        {

        case -1:
            perror("fork error\n");
            return 1; // El hijo no se ha creado correctamente. Error.

        case 0:
            printf("Se ha creado un proceso hijo. PID = %i\n", getpid());
            sleep(1 + i);
            printf("El proceso con PID = %i acaba de finalizar.\n", getpid());
            return 0;
        }
    }

    struct sigaction sigAction;
    sigAction.sa_handler = handler;
    // sigAction.sa_flags = SA_NODEFER;

    sigaction(SIGCHLD, &sigAction, NULL);

    while (contador < 3)
    {
        pause();
    }

    return 0;
}
