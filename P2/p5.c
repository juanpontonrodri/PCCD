#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{

    /*
        Si pongo a Ramos u otro cualquierda en el padre estoy frenando la banda izquierda puesto que es la unica que tiene una restriccion individual
        Si pongo 4 forks estoy usando demasiados procesos (el otor metodo es suficiente con 3)
        y ademas al comprobar en el padre estoy frenando a la banda izquierda ya que al comprobar el proceso tiene que esperar a la comporbacion
        en el padre y no tendria pq esperar para continuar con thiago y silva. Sin embargo si pongo en un mismo proceso jordi alba thiago y silva busquets tendria q esperar por toda la rama

        En el medio campo, una vez tengo la defensa completa es un orden secuencial hasta aspas
        una vez alli compruebo las dos bandas

        */
    int pidar[3], next = 1;

    printf("De Gea ");

    switch (pidar[0] = fork())
    {
    case -1:
        printf("fork error ");
        exit(-1);

    case 0:
        printf("Carvajal ");
        exit(0);
    }

    switch (pidar[1] = fork())
    {
    case -1:
        printf("fork error ");
        exit(-1);

    case 0:
        printf("Ramos ");
        exit(0);
    }

    switch (pidar[2] = fork())
    {
    case -1:
        printf("fork error ");
        exit(-1);

    case 0:
        printf("Piqué ");
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
        printf("Thiago ");
        printf("Silva ");
        exit(0);
    }

    int contador = 0, pidhijo, wstatus, bandaizquierda = 0;
    int k;
    for (k = 0; k < 3;)
    {
        pidhijo = wait(&wstatus);
        if (pidhijo == pidar[0] || pidhijo == pidar[1] || pidhijo == pidar[2])
        {
            k++;
        }
        if (pidhijo == pidizquierda)
        {
            bandaizquierda == 1;
        }
    }

    printf("Busquets ");
    printf("Isco ");
    printf("Aspas ");

    if (bandaizquierda == 0) // para recoger el proceso the la banda izquirda
    {
        int pid2 = wait(&wstatus);
        if (pid2 = pidizquierda)
        {
            printf("Morata \n");
        }
    }
    else
    {
        printf("Morata \n");
    }

    return 0;
}
