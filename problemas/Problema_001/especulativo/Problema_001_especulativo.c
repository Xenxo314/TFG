#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>

#define N 100000
#define THREADS 6

int main() {
    omp_set_num_threads(THREADS);        // Numero total de hilos disponibles
    omp_set_nested(1);                   // Activamos el paralelismo Aninado

    double estimated_pi_S = 0.0;
    double estimated_pi_D = 0.0;
    double estimated_pi_G = 0.0;

    volatile char winner = 'X';     // S -> Static, D -> Dynamic, G -> Guided
    const volatile char * ptr_winner = &(winner);   // Usamos un puntero porque si usáramos la variable winner, cada for "cachearía" su valor y no serviría de flag (Por lo menos en mi ordenador)
    #pragma omp parallel sections shared(winner)
    {
        #pragma omp section 
        {
            // Ajustamos la cantidad de hilos de Esta sección
            omp_set_num_threads(THREADS/3);

            // Cómputo Paralelizable de PI
            #pragma omp parallel for schedule(static) reduction(+:estimated_pi_S) 
            for (long i = N; i >= 1 ; i--) {
                if(*(ptr_winner) != 'X'){
                    #pragma omp cancel for
                }
                estimated_pi_S += 1/(i * (double)i);
            }

            // Si somos la primera planificación en acabar
            #pragma omp critical
                if(winner == 'X')
                {
                    winner = 'S';
                    // Cómputo de secuecnial de PI
                    estimated_pi_S *= 6;
                    estimated_pi_S = sqrt(estimated_pi_S);
                    printf("STA PI: %.15lf\n", estimated_pi_S);
                }
        }

        #pragma omp section
        {
            omp_set_num_threads(THREADS/3);
            #pragma omp parallel for schedule(dynamic) reduction(+:estimated_pi_D) shared(winner)
            for (long j = N; j >= 1; j--) {
                if(*(ptr_winner) != 'X'){
                    #pragma omp cancel for
                }
                estimated_pi_D += 1/(j * (double)j);
            }

            #pragma omp critical
            if(winner == 'X'){
                winner = 'D';
                estimated_pi_D *= 6;
                estimated_pi_D = sqrt(estimated_pi_D);
                printf("DYN PI: %.15lf\n", estimated_pi_D);
            }    
        }

        #pragma omp section
        {
            omp_set_num_threads(THREADS/3);
            #pragma omp parallel for schedule(guided) reduction(+:estimated_pi_G)
            for (long k = N; k >= 1; k--) 
            {
                if(*(ptr_winner) != 'X'){
                    #pragma omp cancel for
                }
                estimated_pi_G += 1/(k * (double)k);
            }

            #pragma omp critical
            if(winner == 'X'){
                winner = 'G';
                estimated_pi_G *= 6;
                estimated_pi_G = sqrt(estimated_pi_G);
                printf("GUI PI: %.15lf\n", estimated_pi_G);
            }   
        }
    }

    printf("Winner = %c\n", winner);
    return 0;
}
