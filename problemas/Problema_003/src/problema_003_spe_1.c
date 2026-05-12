#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>

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

// ATENCIÓN: Si ves que el programa está yendo más lento de lo esperado, recuerda que tienes que activar la cancelación
// export $OMP_CANCELLATION=true, sino no se mejora el tiempo de ejecución.
int main(int argc, char **argv)
{

    long long N; // Tamaño del Problema (numero de columnas a calcular)
    int T;       // Numero de hilos que se usaran
    int p;       // Porcentaje del tamaño del problema que se ejecutará

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

    N = atoll(argv[1]) * p / 100.0; // realizamos el p% de las teraciones en este caso
    T = atoi(argv[2]);

    omp_set_num_threads(T); // Numero total de hilos disponibles
    omp_set_nested(1);      // Activamos el paralelismo Anidado

    double **A, **B, **C_S, **C_D, **C;

    A = alloc_matrix(N);
    B = alloc_matrix(N);
    C_S = alloc_matrix(N);
    C_D = alloc_matrix(N);

    init_lower_triangular(A, N);
    init_random(B, N);

    volatile char winner = 'X';                  // S -> Static, D -> Dynamic y X -> Por decidir
    const volatile char *ptr_winner = &(winner); // Usamos un puntero porque si usáramos la variable winner, cada for "cachearía" su valor y no serviría de flag (Por lo menos en mi ordenador)

    double start, end; // Medidores de tiempo

    start = omp_get_wtime();
#pragma omp parallel sections shared(winner)
    {
#pragma omp section
        {

// Cómputo Paralelizable de PI
#pragma omp parallel for schedule(static) num_threads(T / 2)
            // Multiplicación de matrices
            for (long long i = 0; i < N; i++)
            {
                if (*(ptr_winner) != 'X')
                {
#pragma omp cancel for
                }
                for (long long j = 0; j < N; j++)
                {
                    for (long long k = 0; k <= i; k++)
                    {
                        C_S[i][j] += A[i][k] * B[k][j];
                    }
                }
            }

// Si somos la primera planificación en acabar
#pragma omp critical(asociar_ganador)
            if (winner == 'X')
            {
                winner = 'S';
                //C = C_S;
            }
        }

#pragma omp section
        {

#pragma omp parallel for schedule(dynamic) num_threads(T / 2)
            // Multiplicación de matrices
            for (long long i = 0; i < N; i++)
            {
                if (*(ptr_winner) != 'X')
                {
                    #pragma omp cancel for
                }
                for (long long j = 0; j < N; j++)
                {
                    for (long long k = 0; k <= i; k++)
                    {
                        C_D[i][j] += A[i][k] * B[k][j];
                    }
                }
            }

// Si somos la primera planificación en acabar
#pragma omp critical(asociar_ganador)
            if (winner == 'X')
            {
                winner = 'D';
                //C = C_D;
            }
        }
    }

    end = omp_get_wtime();
    printf("Winner: %c\n", winner);

    printf("TIME: %f seconds\n", end - start);

    free_matrix(A,N);
    free_matrix(B,N);
    free_matrix(C_S,N);
    free_matrix(C_D,N);

    return 0;
}
