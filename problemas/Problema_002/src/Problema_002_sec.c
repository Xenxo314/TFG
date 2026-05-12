#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main(int argc, char *argv[])
{

    long long A, B;
    int num_primos = 0;
    if (argc < 3)
    {
        printf("ERROR: Uso ./%s <A> <B>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    A = atoll(argv[1]);
    B = atoll(argv[2]);
    
    if (A < 2 || B < 1)
    {
        printf("Argumentos inconsistentes\n");
        exit(EXIT_FAILURE);
    }

    
    
    clock_t inicio = clock();

    for (long long i = A; i <= A + B; i++)
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

    clock_t fin = clock();

    printf("Num de primos encontrados: %d\n", num_primos);
    printf("TIME: %f\n", (double)(fin - inicio) / CLOCKS_PER_SEC);

    return 0;
}