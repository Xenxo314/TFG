#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <sys/resource.h>

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
            M[i][j] = rand() % 10;
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
                M[i][j] = rand() % 10;
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

int main(int argc, char *argv[])
{
    double time_tot;
    struct rusage usage; // Tamaño de RSS
    long long N = atoll(argv[1]);
    int T = atoi(argv[2]);

    if (argc < 3)
    {
        printf("Uso: %s N T\n", argv[0]);
        return 1;
    }

    srand(time(NULL));

    double **A = alloc_matrix(N);
    double **B = alloc_matrix(N);
    double **C = alloc_matrix(N);

    init_lower_triangular(A, N);
    init_random(B, N);

    // Inicializar C a 0
    for (long long i = 0; i < N; i++)
        for (long long j = 0; j < N; j++)
            C[i][j] = 0.0;

    double start = omp_get_wtime();

// Multiplicación de matrices
#pragma omp parallel for schedule(runtime) num_threads(T)
    for (long long i = 0; i < N; i++)
    {
        for (long long j = 0; j < N; j++)
        {
            for (long long k = 0; k <= i; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    double end = omp_get_wtime();

    time_tot = end - start;

    getrusage(RUSAGE_SELF, &usage);

    printf("TIME_TOT = %lf\n", time_tot);
    printf("RSS = %ld\n", usage.ru_maxrss);

    // Liberar memoria
    free_matrix(A, N);
    free_matrix(B, N);
    free_matrix(C, N);

    return 0;
}