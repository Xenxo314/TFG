#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <unistd.h>

#define N_SCHEDS 2
/**
 * En este código comprobaremos los numeros primos que existe en el rango [A,A+B]
 */

int main(int argc, char *argv[])
{

    long long A, B;           // Parametros para primos
    int T;                    // Numero de hilos
    long long num_primos_thr[N_SCHEDS] = {0,0}; // Numero de primos entre A y A+B
    long long num_primos = 0;
    volatile char winner = 'X';                  // S -> Static, D -> Dynamic y X -> Por decidir
    const volatile char *ptr_winner = &(winner); // Usamos un puntero porque si usáramos la variable winner, cada for "cachearía" su valor y no serviría de flag (Por lo menos en mi ordenador)

    if (argc < 4)
    {
        printf("ERROR: Uso ./%s <A> <B> <T>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parseo de Argumentos
    A = atoll(argv[1]);
    B = atoll(argv[2]);
    T = atoi(argv[3]);

    omp_set_num_threads(T); // Numero total de hilos disponibles
    omp_set_nested(1);      // Activamos el paralelismo Aninado
    if (A < 2 || B < 1 || T < 1)
    {
        printf("Argumentos inconsistentes\n");
        exit(EXIT_FAILURE);
    }

    double start = omp_get_wtime();
#pragma omp parallel sections
    {
#pragma omp section
        {
            printf("STATIC\n");
            omp_set_num_threads(T / 2);
#pragma omp parallel for schedule(static) reduction(+ : num_primos_thr[0]) num_threads(T/2)
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
                    num_primos_thr[0]++;
                }
            }
            #pragma omp critical
            {
                num_primos = num_primos_thr[0];
                if(*(ptr_winner) == 'X')
                {
                    winner = 'S';
                }
            }
        }
#pragma omp section
        {
            printf("DYNAMIC\n");
            omp_set_num_threads(T / 2);
#pragma omp parallel for schedule(dynamic) reduction(+ : num_primos_thr[1]) num_threads(T/2)
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
                    num_primos_thr[1]++;
                }
            }
            #pragma omp critical
            {
                num_primos = num_primos_thr[1];
                if(*(ptr_winner) == 'X')
                {
                    winner = 'D';
                }
            }
        }
    }

    double end = omp_get_wtime();

    printf("Num de primos encontrados: %lld\n", num_primos);
    printf("TIME: %lf\n", end - start);
    printf("WINNER: %c\n", winner);

    return 0;
}