#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
            M[i][j] = (double)rand() / RAND_MAX;
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

void rellenar_matriz_manual(double **M, long long N, char nombre)
{
    for (long long i = 0; i < N; i++)
    {
        for (long long j = 0; j < N; j++)
        {
            if ('B' == nombre || i >= j)
                M[i][j] = i * N + j + 1;
            else
                M[i][j] = 0;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Uso: %s N\n", argv[0]);
        return 1;
    }

    clock_t start, end;
    double time_tot;
    long long N = atoll(argv[1]);
    struct rusage usage; // Tamaño de RSS


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

    start = clock();
    // Multiplicación de matrices
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

    end = clock();
    getrusage(RUSAGE_SELF, &usage);
    time_tot = ((double)end - start) / CLOCKS_PER_SEC;

    printf("TIME_TOT = %lf\n", time_tot);
    printf("RSS = %ld\n", usage.ru_maxrss); // Medido en KB
    // Liberar memoria
    free_matrix(A, N);
    free_matrix(B, N);
    free_matrix(C, N);

    return 0;
}