#!/bin/bash

# Compilamos los archivos del código fuente
gcc /home/usc/cursos/curso1285/home/TFG/problemas/Problema_003/src/Problema_003_sec.c -o /home/usc/cursos/curso1285/home/TFG/problemas/Problema_003/execs/P_003_seq -lm -O0
gcc /home/usc/cursos/curso1285/home/TFG/problemas/Problema_003/src/Problema_003_omp.c -o /home/usc/cursos/curso1285/home/TFG/problemas/Problema_003/execs/P_003_omp -lm -fopenmp -O0
gcc /home/usc/cursos/curso1285/home/TFG/problemas/Problema_003/src/Problema_003_esp_1.c -o /home/usc/cursos/curso1285/home/TFG/problemas/Problema_003/execs/P_003_esp_1 -lm -fopenmp -O0
gcc /home/usc/cursos/curso1285/home/TFG/problemas/Problema_003/src/Problema_003_esp_2.c -o /home/usc/cursos/curso1285/home/TFG/problemas/Problema_003/execs/P_003_esp_2 -lm -O0 -fopenmp

# Inicializamos las variables de entorno (CANCELLATION)
export OMP_CANCELLATION=true


