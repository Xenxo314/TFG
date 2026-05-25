#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <sys/resource.h>

// ATENCIÓN: Si ves que el programa está yendo más lento de lo esperado, recuerda que tienes que activar la cancelación
// export $OMP_CANCELLATION=true, sino no se mejora el tiempo de ejecución.
int main(int argc, char **argv)
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
    long chunk = 0; // Distancia entre iteraciones (De esta manera evitamos errores de medición) [OPT]

    // MEMORIA
    struct rusage usage; // Tamaño de RSS

    // OTRAS variables
    long long N_d;                               // Tamaño de la decisión
    volatile char winner = 'X';                  // S -> Static, D -> Dynamic y X -> Por decidir
    const volatile char *ptr_winner = &(winner); // Usamos un puntero porque si usáramos la variable winner, cada for "cachearía" su valor y no serviría de flag (Por lo menos en mi ordenador)

    // Variables de cómputo
    double estimated_pi_S = 0.0f; // PI static
    double estimated_pi_D = 0.0f; // PI dynamic
    double estimated_pi = 0.0f;   // Valor Final

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

    // Configuramos OMP correctamente
    omp_set_num_threads(T); // Numero total de hilos disponibles
    omp_set_nested(1);      // Activamos el paralelismo Aninado

    start = omp_get_wtime();
#pragma omp parallel sections shared(winner)
    {
#pragma omp section
        {

// Cómputo Paralelizable de PI
#pragma omp parallel for schedule(static, chunk) reduction(+ : estimated_pi_S) num_threads(T / 2)
            for (long long i = 1; i < N_d; i++)
            {
                if (*(ptr_winner) != 'X')
                {
#pragma omp cancel for
                }
                estimated_pi_S += 1.0 / (i * (double)i);
            }

#pragma omp critical
            if (winner == 'X')
            {
                winner = 'S';
                estimated_pi = estimated_pi_S;
                omp_set_schedule(omp_sched_static, chunk);
            }
        }

#pragma omp section
        {
#pragma omp parallel for schedule(dynamic, chunk) reduction(+ : estimated_pi_D) num_threads(T / 2)
            for (long long i = 1; i < N_d; i++)
            {
                if (*(ptr_winner) != 'X')
                {
#pragma omp cancel for
                }
                estimated_pi_D += 1.0f / (i * (double)i);
            }

#pragma omp critical
            if (winner == 'X')
            {
                winner = 'D';
                estimated_pi = estimated_pi_D;
                omp_set_schedule(omp_sched_dynamic, chunk);
            }
        }
    }

    // Fin del tiempo de decisión
    end_d = omp_get_wtime();

#pragma omp parallel for schedule(runtime) reduction(+ : estimated_pi) num_threads(T)
    for (long long i = N_d; i < N; i++)
    {
        estimated_pi += 1.0f / (i * (double)i);
    }

    estimated_pi *= 6;
    estimated_pi = sqrt(estimated_pi);
    end = omp_get_wtime();

    time_dec = end_d - start;
    time_tot = end - start;
    getrusage(RUSAGE_SELF, &usage);

    printf("TIME_TOT = %lf\n", time_tot);
    printf("TIME_DEC = %lf\n", time_dec);
    printf("WINNER = %c\n", winner);
    printf("RSS = %ld\n", usage.ru_maxrss); // Medido en KB

    return 0;
}
