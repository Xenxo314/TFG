#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

double calc_std_dev(double *data, long N, int *T, double *std_perc)
{
    double sum = 0;
#pragma omp parallel for reduction(+ : sum) schedule(static) num_threads(*T)
    for (int i = 0; i < N; i++)
    {
        sum += data[i];
    }

    double media = sum / N;

    double values = 0;

#pragma omp parallel for reduction(+ : values) schedule(static) num_threads(*T)
    for (long i = 0; i < N; i++)
    {
        values += (data[i] - media) * (data[i] - media);
    }

    double std_dev = sqrt(values / N);

    *std_perc = std_dev / media * 100;

    return std_dev;
}

// Programa que calcula PI como la raiz cuadrada de 6 veces la suma de inversos de los N-esimos primeros naturales.
// sqrt(6*Σ(1/n^2))

int main(int argc, char **argv)
{
    // METRICAS DE TIEMPO
    double time_tot;             // tiempo total de ejecución (Sin reserva de memoria)
    double time_dec;             // Tiempo de decisión
    double start, end;           // Marcadores del tiempo total
    double end_d;                // Marcador del tiempo de decisión
    double start_i, end_i;       // Marcadores de inicio de un bloque de iteraciones
    double std_dev, desbalanceo; // Métricas de desviación

    // ARGUMENTOS
    long long N;        // Tamaño del problema
    int T;              // Numero de hilos
    double p;           // Porcentaje de la decisión
    double umbral = 25; // Umbral para decidir si planificación estática o dinámica [OPT]
    long ckunk = 1;    // Distancia entre iteraciones (De esta manera evitamos errores de medición) [OPT]

    // OTRAS variables
    long long N_d;     // Tamaño de la decisión
    char winner = 'X'; // Indica la planificación ganadora

    // Variables de cómputo
    double estimated_pi = 0;

    // Comprobación de Parámetros
    if (argc < 4)
    {
        printf("ERROR: Uso ./%s <N> <T> <p> [umbral] [chunk]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // ENTRADA de parametros
    N = atoll(argv[1]);
    T = atoi(argv[2]);
    p = atof(argv[3]);

    if (argc == 6)
    {
        umbral = atoi(argv[4]);
        ckunk = atol(argv[5]);
    }

    N_d = N * p / 100.0f;

    // RESERVA de memoria
    double *times = malloc(N_d / ckunk * sizeof(double));

    // COMIENZA EL TIEMPO DE DECISIÓN
    start = omp_get_wtime();

    for (long long i = 1; i < N_d; i++)
    {
        start_i = omp_get_wtime();
        estimated_pi += 1 / (i * (double)i);
        if (i % ckunk == 0)
        {
            end_i = omp_get_wtime();
            times[i / ckunk - 1] = end_i - start_i;
        }
    }

    // CALCULO MÉTRICAS
    std_dev = calc_std_dev(times, N_d / ckunk, &T, &desbalanceo);

    // Ajustamos el scheduler
    (desbalanceo < umbral) ? (omp_set_schedule(omp_sched_static, ckunk), winner = 'S') : (omp_set_schedule(omp_sched_dynamic, ckunk), winner = 'D');

    // FIN TIEMPO DE DECISIÓN
    end_d = omp_get_wtime();

    // COMPUTO PARALELO
#pragma omp parallel for reduction(+ : estimated_pi) schedule(runtime) num_threads(T)
    for (long long i = N_d; i < N; i++)
    {
        estimated_pi += 1 / (i * (double)i);
    }
    estimated_pi *= 6;
    estimated_pi = sqrt(estimated_pi);

    // FIN TIEMPO TOTAL
    end = omp_get_wtime();

    time_dec = end_d - start;
    time_tot = end - start;

    // PRINTS
    printf("TIME_TOT: %lf\n", time_tot);
    printf("TIME_DEC: %lf\n", time_dec);
    printf("STD_DEV = %.15lf\n", std_dev);
    printf("DESBALANCEO = %.15lf %%\n", desbalanceo);
    printf("WINNER = %c\n", winner);

    // FREE
    free(times);

    return 0;
}