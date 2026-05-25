#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/resource.h>


int main(int argc, char *argv[])
{

    clock_t start, end;
    double time_tot;
    long long N;
    struct rusage usage; // Tamaño de RSS

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

    start = clock();

    for (long long i = 2; i <= N; i++)
    {
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
    }

    end = clock();

    time_tot = ((double)end - start) / CLOCKS_PER_SEC;
    
    getrusage(RUSAGE_SELF, &usage);

    printf("TIME_TOT = %lf\n", time_tot);
    printf("RSS = %ld\n", usage.ru_maxrss); // Medido en KB
    // printf("Num Primos = %ld\n", num_primos);

    return 0;
}