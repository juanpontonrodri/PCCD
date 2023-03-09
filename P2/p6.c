#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{

    int pidar[3], next = 1;
    printf("De Gea ");

    switch (pidar[0] = fork())
    {
    case -1:
        printf("fork error ");
        exit(-1);

    case 0:
        execl("./phijo2", "phijo2", "Carvajal ", (char *)0);
        exit(0);
    }

    switch (pidar[1] = fork())
    {
    case -1:
        printf("fork error ");
        exit(-1);

    case 0:
        execl("./phijo2", "phijo2", "Ramos ", (char *)0);
        exit(0);
    }

    switch (pidar[2] = fork())
    {
    case -1:
        printf("fork error ");
        exit(-1);

    case 0:
        execl("./phijo2", "phijo2", "Piqué ", (char *)0);
        exit(0);
    }

    printf("Jordi Alba ");
    int pidizquierda;
    switch (pidizquierda = fork())
    {
    case -1:
        printf("fork error ");
        exit(-1);

    case 0:
        execl("./phijo2", "phijo2", "Thiago Silva ", (char *)0);
        exit(0);
    }

    int contador = 0, pidhijo, wstatus, bandaizquierda = 0;
    while (contador != 3)
    {
        pidhijo = wait(&wstatus);
        if (pidhijo == pidar[0] || pidhijo == pidar[1] || pidhijo == pidar[2])
        {
            contador++;
        }
        if (pidhijo == bandaizquierda) // a veces el wait coge el pid the este proceso y se lia
        {
            bandaizquierda = 1;
        }
    }
    printf("Busquets ");
    printf("Isco ");
    printf("Aspas ");

    if (bandaizquierda == 0) // para recoger el proceso the la banda izquirda
    {
        wait(NULL);
    }
    printf("Morata \n");

    return 0;
}