#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <sys/resource.h>

/**
 * En este código comprobaremos los numeros primos que existe en el rango [A,A+B]
 */

int main(int argc, char *argv[])
{

    long long N;              // Parametros para primos
    int T;                    // Numero de hilos
    long long num_primos = 0; // Numero de primos entre A y A+B
    double time_tot;

    struct rusage usage; // Tamaño de RSS

    if (argc < 3)
    {
        printf("ERROR: Uso ./%s <N> <T>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parseo de Argumentos
    N = atoll(argv[1]);
    T = atoi(argv[2]);

    if (N < 2 || T < 1)
    {
        printf("Argumentos inconsistentes\n");
        exit(EXIT_FAILURE);
    }

    double start = omp_get_wtime();
#pragma omp parallel for schedule(runtime) reduction(+ : num_primos) num_threads(T)
    for (long long i = 2; i <= N; i++)
    {
        int isprime = 1; // Todo numero es primo hasta que se demuestre lo contrario
        for (long long j = 2; j <= sqrt(i); j++)
        {
            if (i % j == 0)
            {
                isprime = 0;
                break;
            }
        }
        if (isprime)
        {
            num_primos++;
        }
    }
    double end = omp_get_wtime();
    getrusage(RUSAGE_SELF, &usage);

    time_tot = end - start;
    printf("TIME_TOT = %lf\n", time_tot);
    printf("RSS = %ld\n", usage.ru_maxrss);

    return 0;
}