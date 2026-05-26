#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/resource.h>


// Programa que calcula PI como la raiz cuadrada de 6 veces la suma de inversos de los N-esimos primeros naturales.
// sqrt(6*Σ(1/n^2))
int main(int argc, char **argv)
{
    // Variables que miden el tiempo
    clock_t start, end;
    double time_tot;

    long long N; // Tamaño del Problema
    double estimated_pi = 0;

    struct rusage usage; // Tamaño de RSS

    //Comprobación de Parámetros
    if(argc < 2)
    {
        perror("ERROR: No hay suficientes paramámetros introducidos\n");
        exit(EXIT_FAILURE);
    }

    N = atoll(argv[1]);
    start = clock();
    // Hacemos la suma de los inversos al cuadrado
    for (long long i = 1; i < N; i++)
    {
        estimated_pi += 1 / (double)(i * i);
    }

    // Multiplicamos y Hacemos la raiz
    estimated_pi *= 6;
    estimated_pi = sqrt(estimated_pi);

    // Paramos Contador
    end = clock();
    time_tot = ((double)(end - start)) / CLOCKS_PER_SEC;

    getrusage(RUSAGE_SELF, &usage);

    printf("TIME_TOT = %lf\n", time_tot);
    printf("RSS = %ld\n", usage.ru_maxrss); // Medido en KB
    // printf("PI = %lf\n", estimated_pi);
    return 0;
}