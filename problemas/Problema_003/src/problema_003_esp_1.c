#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
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
    long chunk = 1; // Distancia entre iteraciones (De esta manera evitamos errores de medición) [OPT]

    // MEMORIA
    struct rusage usage; // Tamaño de RSS

    // OTRAS variables
    long long N_d;                               // Tamaño de la decisión
    volatile char winner = 'X';                  // S -> Static, D -> Dynamic y X -> Por decidir
    const volatile char *ptr_winner = &(winner); // Usamos un puntero porque si usáramos la variable winner, cada for "cachearía" su valor y no serviría de flag (Por lo menos en mi ordenador)

    // Variables de cómputo
    double **A, **B, **C_S, **C_D, **C;

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
    omp_set_nested(1);      // Activamos el paralelismo Anidado

    // RESERVA DE MEMORIA
    A = alloc_matrix(N);
    B = alloc_matrix(N);
    C_S = alloc_matrix(N);
    C_D = alloc_matrix(N);

    init_lower_triangular(A, N);
    init_random(B, N);

    start = omp_get_wtime();
#pragma omp parallel sections shared(winner)
    {
#pragma omp section
        {

// Cómputo Paralelizable de PI
#pragma omp parallel for schedule(static) num_threads(T / 2)
            // Multiplicación de matrices
            for (long long i = 0; i < N_d; i++)
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
                C = C_S;
                omp_set_schedule(omp_sched_static, 0);
            }
        }

#pragma omp section
        {

#pragma omp parallel for schedule(dynamic) num_threads(T / 2)
            // Multiplicación de matrices
            for (long long i = 0; i < N_d; i++)
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
                C = C_D;
                omp_set_schedule(omp_sched_dynamic, 0);
            }
        }
    }

    end_d = omp_get_wtime();

// Multiplicación de matrices
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

    end = omp_get_wtime();

    time_dec = end_d - start;
    time_tot = end - start;
    getrusage(RUSAGE_SELF, &usage);

    printf("TIME_TOT = %lf\n", time_tot);
    printf("TIME_DEC = %lf\n", time_dec);
    printf("WINNER = %c\n", winner);
    printf("RSS = %ld\n", usage.ru_maxrss); // Medido en KB

    free_matrix(A, N);
    free_matrix(B, N);
    free_matrix(C_S, N);
    free_matrix(C_D, N);

    return 0;
}
