#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{

    printf("Ej 2 PID: %i\n", getpid());
    int i;
    int pidarray[5];

    for (i = 0; i < 3; i++)
    {
        if (i == 1)
        {
            switch (pidarray[i] = fork())
            {

            case -1:
                perror("error en el fork\n");
                return 1;

            case 0:
                printf("proceso hijo creado con PID = %i\n", getpid());
                sleep(5);
                printf("proceso  = %i finalizó\n", getpid());
                exit(2);
                return 0;
            }
        }
        else
        {
            switch (pidarray[i] = fork())
            {

            case -1:
                perror("error en el fork\n");
                return 1;

            case 0:
                printf("proceso hijo creado con PID = %i\n", getpid());
                sleep(1);
                printf("proceso  = %i finalizó\n", getpid());
                exit(2);
                return 0;
            }
        }
    }
    int pid;
    int wstatus;

    for (int j = 0; j < 3; j++)
    {

        pid = waitpid(pidarray[j], &wstatus, 0);
        printf("codigo en exit del proceso %i es: %i\n", pid, WEXITSTATUS(wstatus));
    }

    return 0;
}
