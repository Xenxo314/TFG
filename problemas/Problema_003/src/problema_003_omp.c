#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

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
    if (argc < 3)
    {
        printf("Uso: %s N T\n", argv[0]);
        return 1;
    }

    omp_sched_t sched;
    int chunky = 0;
    long long N = atoll(argv[1]);
    int T = atoi(argv[2]);

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

    printf("TIME: %lf\n", end - start);

    // Liberar memoria
    free_matrix(A, N);
    free_matrix(B, N);
    free_matrix(C, N);

    return 0;
}