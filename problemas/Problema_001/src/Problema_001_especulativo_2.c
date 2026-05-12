#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double calc_std_dev(double *data, long N)
{
    double sum = 0;
    for(int i = 0; i < N; i++)
    {
        sum += data[i];
    }

    double media = sum / N;

    double values = 0;

    for (long i = 0; i < N; i++)
    {
        values += (data[i] - media)*(data[i] - media);
    }
    
    double std_dev = sqrt(values/N);

    return std_dev;
}

// Programa que calcula PI como la raiz cuadrada de 6 veces la suma de inversos de los N-esimos primeros naturales.
// sqrt(6*Σ(1/n^2))
int main(int argc, char **argv)
{
    // Variables que miden el tiempo
    clock_t start, end, start_i, end_i;
    double cpu_time_used;

    long N; // Tamaño del Problema
    double estimated_pi = 0;

    // Comprobación de Parámetros
    if (argc < 2)
    {
        perror("ERROR: No hay suficientes paramámetros introducidos\n");
        exit(EXIT_FAILURE);
    }

    N = atol(argv[1]);
    double *times = malloc( N * sizeof(double));


    start = clock();
    // Hacemos la suma de los inversos al cuadrado
    for (long long i = 1; i < N; i++)
    {
        start_i = clock();
        estimated_pi += 1 / (i * (double)i);
        end_i = clock();
        times[i - 1] = ((double)(end_i-start_i)) / CLOCKS_PER_SEC;
    }

    // Multiplicamos y Hacemos la raiz
    estimated_pi *= 6;
    estimated_pi = sqrt(estimated_pi);

    // Paramos Contador
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("TIME: %lf\n", cpu_time_used);

    // Calculamos la desviación estandar de los tiempos de cada iteración.
    double std_dev = calc_std_dev(times,N);
    printf("STD_DEV = %.15lf\n",std_dev);

    return 0;
}