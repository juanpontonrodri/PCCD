#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void *Carvajal(void *parametros)
{
    printf("Carvajal ");
    pthread_exit(0);
}

void *Ramos(void *parametros)
{
    printf("Ramos ");
    pthread_exit(0);
}

void *Pique(void *parametros)
{
    printf("Piqué ");
    pthread_exit(0);
}

void *BandaIzq(void *parametros)
{
    printf("Thiago ");
    printf("Silva ");
    pthread_exit(0);
}

int main()
{

    pthread_t hiloCarvajal;
    pthread_t hiloRamos;
    pthread_t hiloPique;
    pthread_t hiloBandIzq;

    printf("De Gea ");

    pthread_create(&hiloCarvajal, NULL, Carvajal, NULL);
    pthread_create(&hiloRamos, NULL, Ramos, NULL);
    pthread_create(&hiloPique, NULL, Pique, NULL);

    printf("Jordi Alba ");

    // pthread_join(hiloCarvajal, NULL);
    // pthread_join(hiloRamos, NULL);
    // pthread_join(hiloPique, NULL);

    // Al final esto anterior no vale pq espera por la defensa entera antes
    // de la banda izquierda y no es correcto

    pthread_create(&hiloBandIzq, NULL, BandaIzq, NULL);

    // ahora que ya iniciamos la banda mientras se ejecuta vamos esperando
    // y luego hacemos la secunecia final
    pthread_join(hiloCarvajal, NULL);
    pthread_join(hiloRamos, NULL);
    pthread_join(hiloPique, NULL);

    printf("Busquets ");
    printf("Isco ");
    printf("Aspas ");

    pthread_join(hiloBandIzq, NULL);
    // se lee la banza izquierda justoa antes de morata supongo pq es cunado muere el hilo
    // q al igual q con los procesos no se vacia el buffer por no poner salto de linea
    printf("Morata");

    pthread_exit(NULL);

    return 0;
}
