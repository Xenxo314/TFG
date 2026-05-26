#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <sys/resource.h>

#define TOLERANCIA 1
#define EPSILON 1e-7

// FUNCIONES AUXILIARES PARA MATRICES
double **alloc_matrix(long long N)
{
    double **M = (double **)malloc(N * sizeof(double *));
    for (long long i = 0; i < N; i++)
    {
        M[i] = (double *)malloc(N * sizeof(double));
    }
    return M;
}

void free_matrix(double **M, long long N)
{
    for (long long i = 0; i < N; i++)
    {
        free(M[i]);
    }
    free(M);
}

void init_random(double **M, long long N)
{
    for (long long i = 0; i < N; i++)
    {
        for (long long j = 0; j < N; j++)
        {
            M[i][j] = rand() % 2 + 1;
        }
    }
}

// Inicializa matriz triangular inferior
void init_lower_triangular(double **M, long long N)
{
    for (long long i = 0; i < N; i++)
    {
        for (long long j = 0; j < N; j++)
        {
            if (j <= i)
                M[i][j] = rand() % 2 + 1;
        }
    }
}

void print_matrix(double **M, long long N)
{
    for (long long i = 0; i < N; i++)
    {
        for (long long j = 0; j < N; j++)
        {
            printf("%8.4f ", M[i][j]);
        }
        printf("\n");
    }
}

double calc_std_dev(double *data, long long N, int *T, double *std_perc)
{
    double sum = 0;
#pragma omp parallel for reduction(+ : sum) schedule(static) num_threads(*T)
    for (long long i = 0; i < N; i++)
    {
        sum += data[i];
    }

    double media = sum / N;
    double values = 0;

#pragma omp parallel for reduction(+ : values) schedule(static) num_threads(*T)
    for (long long i = 0; i < N; i++)
    {
        values += (data[i] - media) * (data[i] - media);
    }

    double std_dev = sqrt(values / N);

    *std_perc = std_dev / media * 100;

    return std_dev;
}

int main(int argc, char **argv)
{
    // METRICAS DE TIEMPO
    double time_tot;             // tiempo total de ejecución (Sin reserva de memoria)
    double time_dec;             // Tiempo de decisión
    double start, end;           // Marcadores del tiempo total
    double end_d;                // Marcador del tiempo de decisión
    double start_i, end_i;       // Marcadores de inicio de un bloque de iteraciones
    double std_dev, desbalanceo; // Desviación típica y coeficiente de variación

    // ARGUMENTOS
    long long N;        // Tamaño del problema
    int T;              // Numero de hilos
    double p;           // Porcentaje de la decisión
    double umbral = 25; // Umbral para decidir si planificación estática o dinámica [OPT]
    long chunk = 1;     // Distancia entre iteraciones (De esta manera evitamos errores de medición) [OPT]

    // MEMORIA
    struct rusage usage; // Tamaño de RSS

    // OTRAS variables
    long long N_d;            // Tamaño de la decisión
    char winner = 'X';        // Indica la planificación ganadora
    long long init_value = 0; // Valor inicial en el bucle

    // Variables de cómputo
    double **A, **B, **C;

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
        chunk = atol(argv[5]);
    }

    N_d = N * p / 100.0f;

    // RESERVA de memoria
    double *times = malloc(ceil((double)(N_d - init_value) / (double) chunk) * sizeof(double));
    A = alloc_matrix(N);
    B = alloc_matrix(N);
    C = alloc_matrix(N);

    init_lower_triangular(A, N);
    init_random(B, N);

    // Inicializar C a 0
    for (long long i = 0; i < N; i++)
        for (long long j = 0; j < N; j++)
            C[i][j] = 0.0;

    // COMIENZA EL TIEMPO DE DECISIÓN
    start = omp_get_wtime();

    for (long long i = init_value; i < N_d; i++)
    {
        if ((i - init_value) % chunk == 0)
        {
            start_i = omp_get_wtime();
        }

        int contador = 1;
    repeat:
        if (contador == 1)
        {
            for (long long j = 0; j < N; j++)
            {
                for (long long k = 0; k <= i; k++)
                {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
        else
        {
            double a = 0;
            for (long long j = 0; j < N; j++)
            {
                for (long long k = 0; k <= i; k++)
                {
                    a += A[i][k] * B[k][j];
                }
            }
        }
        if (chunk == 1 || (((i - init_value)% chunk) == (chunk - 1))|| i == (N_d - 1))
        {
            end_i = omp_get_wtime();
            if (end_i - start_i > EPSILON)
            {
                times[(i - init_value) / chunk] = (end_i - start_i) / contador;
            }
            else
            {
                contador++;
                goto repeat;
            }
        }
    }

    // CALCULO MÉTRICAS
    std_dev = calc_std_dev(times, ceil((double)(N_d - init_value) /(double) chunk), &T, &desbalanceo);

    // Ajustamos el scheduler
    (desbalanceo < umbral) ? (omp_set_schedule(omp_sched_static, chunk), winner = 'S') : (omp_set_schedule(omp_sched_dynamic, chunk), winner = 'D');

    // FIN TIEMPO DE DECISIÓN
    end_d = omp_get_wtime();

    // COMPUTO PARALELO
#pragma omp parallel for schedule(runtime) num_threads(T)
    for (long long i = N_d; i < N; i++)
    {
        for (long long j = 0; j < N; j++)
        {
            for (long long k = 0; k <= i; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    // FIN TIEMPO TOTAL
    end = omp_get_wtime();

    time_dec = end_d - start;
    time_tot = end - start;

    // Medir memoria
    getrusage(RUSAGE_SELF, &usage);

    // PRINTS
    printf("TIME_TOT = %lf\n", time_tot);
    printf("TIME_DEC = %lf\n", time_dec);
    printf("STD_DEV = %.15lf\n", std_dev);
    printf("DESBALANCEO = %.15lf\n", desbalanceo);
    printf("WINNER = %c\n", winner);
    printf("RSS = %ld\n", usage.ru_maxrss); // Está en KB

    // FREE
    free_matrix(A, N);
    free_matrix(B, N);
    free_matrix(C, N);
    free(times);

    return 0;
}