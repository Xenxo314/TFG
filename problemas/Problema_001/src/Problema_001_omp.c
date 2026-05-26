#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <sys/resource.h>

// Programa que calcula PI como la raiz cuadrada de 6 veces la suma de inversos de los N-esimos primeros naturales.
// sqrt(6*Σ(1/n^2))
int main(int argc, char **argv)
{
    // Variables que miden el tiempo
    double start, end, time_tot;

    struct rusage usage; // Tamaño de RSS

    long long N;
    int T;
    double estimated_pi = 0;

    // Comprobación de Parámetros
    if (argc < 3)
    {
        perror("ERROR: No hay suficientes paramámetros introducidos\n");
        exit(EXIT_FAILURE);
    }

    N = atoll(argv[1]);
    T = atoi(argv[2]);

    // Ajustamos el nº de hilos que ejecutan el programa
    omp_set_num_threads(T);
    start = omp_get_wtime();
// Hacemos la suma de los inversos al cuadrado
#pragma omp parallel for reduction(+ : estimated_pi) schedule(runtime)
    for (long long i = 1; i < N; i++)
    {
        estimated_pi += 1 / (i * (double)i);
    }

    // Multiplicamos y Hacemos la raiz
    estimated_pi *= 6;
    estimated_pi = sqrt(estimated_pi);

    // Paramos Contador
    end = omp_get_wtime();
    time_tot = end - start;
    getrusage(RUSAGE_SELF, &usage);

    printf("TIME_TOT = %lf\n", time_tot);
    printf("RSS = %ld\n", usage.ru_maxrss);

    return 0;
}