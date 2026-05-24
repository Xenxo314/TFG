#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <unistd.h>
#include <sys/resource.h>


#define N_SCHEDS 2
/**
 * En este código comprobaremos los numeros primos que existe en el rango [A,A+B]
 */

int main(int argc, char *argv[])
{

    long long num_primos_thr[N_SCHEDS] = {0, 0}; // Numero de primos entre A y A+B
    long long num_primos = 0;
    volatile char winner = 'X';                  // S -> Static, D -> Dynamic y X -> Por decidir
    const volatile char *ptr_winner = &(winner); // Usamos un puntero porque si usáramos la variable winner, cada for "cachearía" su valor y no serviría de flag (Por lo menos en mi ordenador)
    long long N_d, N;                            // Tamaño del Problema y numero de chunks
    int T;                                       // Numero de hilos que se usaran
    int p;                                       // Porcentaje del tamaño del problema que se ejecutará

    struct rusage usage; // Tamaño de RSS


    // Comprobación de Parámetros
    if (argc < 4)
    {
        perror("ERROR: No hay suficientes paramámetros introducidos\n");
        exit(EXIT_FAILURE);
    }

    p = atoi(argv[3]);
    if (p > 100)
    {
        printf("No se admiten porcentajes mayores a 100\n");
        abort();
    }

    N_d = atoll(argv[1]) * p / 100.0; // Numero de iteraciones para decidir la planificación
    N = atoll(argv[1]);               // Tamaño real del problema
    T = atoi(argv[2]);

    omp_set_num_threads(T); // Numero total de hilos disponibles
    omp_set_nested(1);      // Activamos el paralelismo Aninado
    if (N < 2 || T < 1)
    {
        printf("Argumentos inconsistentes\n");
        exit(EXIT_FAILURE);
    }

    double start = omp_get_wtime();
#pragma omp parallel sections
    {
#pragma omp section
        {
            omp_set_num_threads(T / 2);
#pragma omp parallel for schedule(static) reduction(+ : num_primos_thr[0]) num_threads(T / 2)
            for (long long i = 2; i < N_d; i++)
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
                    num_primos_thr[0]++;
                }
            }
#pragma omp critical
            {
                num_primos = num_primos_thr[0];
                if (*(ptr_winner) == 'X')
                {
                    winner = 'S';
                    omp_set_schedule(omp_sched_static, 0);
                }
            }
        }
#pragma omp section
        {
            omp_set_num_threads(T / 2);
#pragma omp parallel for schedule(dynamic) reduction(+ : num_primos_thr[1]) num_threads(T / 2)
            for (long long i = 2; i < N_d; i++)
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
                    num_primos_thr[1]++;
                }
            }
#pragma omp critical
            {
                num_primos = num_primos_thr[1];
                if (*(ptr_winner) == 'X')
                {
                    winner = 'D';
                    omp_set_schedule(omp_sched_dynamic, 0);
                }
            }
        }
    }
    double end_d = omp_get_wtime();

#pragma omp parallel for schedule(runtime) reduction(+ : num_primos) num_threads(T)
    for (long long i = N_d; i <= N; i++)
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

    printf("TIME_TOT: %lf\n", end - start);
    printf("TIME_DEC: %lf\n", end_d - start);
    printf("WINNER = %c\n", winner);
    printf("RSS = %ld\n", usage.ru_maxrss); // Está en KB

    return 0;
}