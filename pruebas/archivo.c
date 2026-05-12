#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <omp.h>


int main(int argc, char **argv)
{
    double start, end, elapsed;

    double pi = 0;

    int N = atoi(argv[1]);
    int T = atoi(argv[2]);
    start = omp_get_wtime();
    // #pragma omp parallel for reduction(+:pi) schedule(runtime) num_threads(THREADS)
    // for( long long i = 0; i<N; i++)
    // {
    //     pi += pow(-1,i)/(2*i+1);
    // }

    // pi *= 4;

#pragma omp parallel for schedule(runtime) num_threads(T) reduction(+:pi)
    for (long long i = 0; i < N; i++)
    {
        printf("T: %d i: %lld\n",omp_get_thread_num(), i);
    }

    end = omp_get_wtime();

    elapsed = end - start;

    printf("%lf\n", elapsed);
    return 0;
}
