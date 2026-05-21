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

// Programa para el cómpputo de números primos
int main(int argc, char **argv)
{
    // Variables que miden el tiempo
    clock_t start, end, start_i, end_i;
    double cpu_time_used;

    long N; // Tamaño del Problema
    long num_primos = 0;

    if (argc < 2)
    {
        printf("ERROR: Uso ./%s <N>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    N = atoll(argv[1]);

    
    if (N < 2)
    {
        printf("Argumentos inconsistentes\n");
        exit(EXIT_FAILURE);
    }

    double *times = malloc(N * sizeof(double));

    start = clock();

    for (long long i = 2; i <= N; i++)
    {
        start_i = clock();
        int isprime = 1;
        for (long long j = 2; j <= sqrt(i); j++)
        {
            if (i % j == 0)
            {
                isprime = 0;
                break;

            }
        }
        if (isprime)
        {
            num_primos++;
        }
        times[i - 1] = ((double)(end_i-start_i)) / CLOCKS_PER_SEC;
    }


    // Paramos Contador
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("TIME: %lf\n", cpu_time_used);

    // Calculamos la desviación estandar de los tiempos de cada iteración.
    double std_dev = calc_std_dev(times,N);
    printf("STD_DEV = %.15lf\n",std_dev);
    printf("100_DEV = %.15lf\n",std_dev/cpu_time_used * 100);

    return 0;
}