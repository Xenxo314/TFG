#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Programa que calcula PI como la raiz cuadrada de 6 veces la suma de inversos de los N-esimos primeros naturales.
// sqrt(6*Σ(1/n^2))
int main(int argc, char **argv)
{
    // Variables que miden el tiempo
    clock_t start, end;
    double cpu_time_used;

    long N; // Tamaño del Problema
    double estimated_pi = 0;

    //Comprobación de Parámetros
    if(argc < 2)
    {
        perror("ERROR: No hay suficientes paramámetros introducidos\n");
        exit(EXIT_FAILURE);
    }

    N = atoi(argv[1]);
    start = clock();
    // Hacemos la suma de los inversos al cuadrado
    for (double i = 1; i < N; i++)
    {
        estimated_pi += 1 / (i * i);
    }

    // Multiplicamos y Hacemos la raiz
    estimated_pi *= 6;
    estimated_pi = sqrt(estimated_pi);

    // Paramos Contador
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("TIME: %lf\n", cpu_time_used);

    return 0;
}