#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>

// ATENCIÓN: Si ves que el programa está yendo más lento de lo esperado, recuerda que tienes que activar la cancelación
// export $OMP_CANCELLATION=true, sino no se mejora el tiempo de ejecución.
int main(int argc, char **argv)
{

    long long N_d, N; // Tamaño del Problema y numero de chunks
    int T;            // Numero de hilos que se usaran
    int p;            // Porcentaje del tamaño del problema que se ejecutará

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

    double estimated_pi_S = 0.0f; // PI static
    double estimated_pi_D = 0.0f; // PI dynamic
    double estimated_pi = 0.0f;   // Valor Final

    volatile char winner = 'X';                  // S -> Static, D -> Dynamic y X -> Por decidir
    //const volatile char *ptr_winner = &(winner); // Usamos un puntero porque si usáramos la variable winner, cada for "cachearía" su valor y no serviría de flag (Por lo menos en mi ordenador)
    // HOLA xenxo del futuro al habla, EL SIMPLE HECHO DE DESCOMENTAR LA LINEA DE ARRIBA AFECTA AL CÓDIGO QUÉ DIABLOS
    double start, end_d, end; // Medidores de tiempo

    start = omp_get_wtime();
#pragma omp parallel sections shared(winner)
    {
#pragma omp section
        {
            // Ajustamos la cantidad de hilos de Esta sección
            omp_set_num_threads(T / 2);

// Cómputo Paralelizable de PI
#pragma omp parallel for schedule(static) reduction(+ : estimated_pi_S)
            for (long i = 1; i < N_d; i++)
            {
                if (winner != 'X')
                {
                    printf("Estatico\n");
#pragma omp cancel for
                }
                estimated_pi_S += 1 / (i * (double)i);
            }

// Si somos la primera planificación en acabar
#pragma omp critical
            if (winner == 'X')
            {
                winner = 'S';
                estimated_pi = estimated_pi_S;
                omp_set_schedule(omp_sched_static, 0);
            }
        }

#pragma omp section
        {
            omp_set_num_threads(T / 2);
#pragma omp parallel for schedule(dynamic) reduction(+ : estimated_pi_D)
            for (long i = 1; i < N_d; i++)
            {
                if (winner != 'X')
                {
                    printf("DINAMICO\n");
#pragma omp cancel for
                }
                estimated_pi_D += 1 / (i * (double)i);
            }

#pragma omp critical
            if (winner == 'X')
            {
                winner = 'D';
                estimated_pi = estimated_pi_D;
                omp_set_schedule(omp_sched_dynamic, 0);
            }
        }
    }

    end_d = omp_get_wtime();

    #pragma omp parallel for schedule(runtime) num_threads(T)
    for (int i = N_d; i < N; i++)
    {
        estimated_pi += 1 / (i * (double)i);
    }

    estimated_pi *= 6;
    estimated_pi = sqrt(estimated_pi_D);
    end = omp_get_wtime();
    printf("WINNER: %c\n", winner);
    printf("DECISSION_TIME: %f\n", end_d - start);
    printf("TOTAL_TIME: %f\n", end - start);
    return 0;
}
