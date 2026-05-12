#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

/**
 * En este código comprobaremos los numeros primos que existe en el rango [A,A+B]
 */

int main(int argc, char *argv[])
{

    long long A, B;           // Parametros para primos
    int T;                    // Numero de hilos
    long long num_primos = 0; // Numero de primos entre A y A+B
    if (argc < 4)
    {
        printf("ERROR: Uso ./%s <A> <B> <T>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parseo de Argumentos
    A = atoll(argv[1]);
    B = atoll(argv[2]);
    T = atoi(argv[3]);

    if (A < 2 || B < 1 || T < 1)
    {
        printf("Argumentos inconsistentes\n");
        exit(EXIT_FAILURE);
    }

    double start = omp_get_wtime();

#pragma omp parallel for schedule(runtime) reduction(+ : num_primos) num_threads(T)
    for (long long i = A; i <= A + B; i++)
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

    //printf("Num de primos encontrados: %lld\n", num_primos);
    printf("TIME: %lf\n", end - start);

    return 0;
}