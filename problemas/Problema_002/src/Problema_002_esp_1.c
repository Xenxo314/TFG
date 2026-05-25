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

    // METRICAS DE TIEMPO
    double time_tot;   // tiempo total de ejecución (Sin reserva de memoria)
    double time_dec;   // Tiempo de decisión
    double start, end; // Marcadores del tiempo total
    double end_d;      // Marcador del tiempo de decisión

    // ARGUMENTOS
    long long N;    // Tamaño del problema
    int T;          // Numero de hilos
    double p;       // Porcentaje de la decisión
    long chunk = 1; // Distancia entre iteraciones [OPT]

    // MEMORIA
    struct rusage usage; // Tamaño de RSS

    // OTRAS variables
    long long N_d;                               // Tamaño de la decisión
    volatile char winner = 'X';                  // S -> Static, D -> Dynamic y X -> Por decidir
    const volatile char *ptr_winner = &(winner); // Usamos un puntero porque si usáramos la variable winner, cada for "cachearía" su valor y no serviría de flag (Por lo menos en mi ordenador)

    // Variables de cómputo
    long long num_primos_S = 0; // Computo Static
    long long num_primos_D = 0; // Computo Dinamic
    long long num_primos = 0;   // Cómputo Final

    // Comprobación de Parámetros
    if (argc < 4)
    {
        perror("ERROR: No hay suficientes paramámetros introducidos\n");
        exit(EXIT_FAILURE);
    }

    N = atoll(argv[1]);
    T = atoi(argv[2]);
    p = atoi(argv[3]);
    if (argc > 4)
        chunk = atoi(argv[4]);

    if (p > 100)
    {
        printf("No se admiten porcentajes mayores a 100\n");
        abort();
    }
    N_d = N / 100.0f * p;

    omp_set_num_threads(T); // Numero total de hilos disponibles
    omp_set_nested(1);      // Activamos el paralelismo Aninado
    if (N < 2 || T < 1)
    {
        printf("Argumentos inconsistentes\n");
        exit(EXIT_FAILURE);
    }

    start = omp_get_wtime();
#pragma omp parallel sections
    {
#pragma omp section
        {
            omp_set_num_threads(T / 2);
#pragma omp parallel for schedule(static, chunk) reduction(+ : num_primos_S) num_threads(T / 2)
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
                    num_primos_S++;
                }
            }
#pragma omp critical
            {
                num_primos = num_primos_S;
                if (*(ptr_winner) == 'X')
                {
                    winner = 'S';
                    omp_set_schedule(omp_sched_static, chunk);
                }
            }
        }
#pragma omp section
        {
            omp_set_num_threads(T / 2);
#pragma omp parallel for schedule(dynamic, chunk) reduction(+ : num_primos_D) num_threads(T / 2)
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
                    num_primos_D++;
                }
            }
#pragma omp critical
            {
                num_primos = num_primos_D;
                if (*(ptr_winner) == 'X')
                {
                    winner = 'D';
                    omp_set_schedule(omp_sched_dynamic, chunk);
                }
            }
        }
    }
    end_d = omp_get_wtime();

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

    end = omp_get_wtime();

    time_dec = end_d - start;
    time_tot = end - start;
    getrusage(RUSAGE_SELF, &usage);

    printf("TIME_TOT = %lf\n", time_tot);
    printf("TIME_DEC = %lf\n", time_dec);
    printf("WINNER = %c\n", winner);
    printf("RSS = %ld\n", usage.ru_maxrss); // Medido en KB
    // printf("pi(x) = %ld\n", num_primos);

    return 0;
}