#!/bin/bash

# Compilamos los archivos del código fuente
gcc /home/usc/cursos/curso1285/home/TFG/problemas/Problema_001/sequential/Problema_001_sec.c -o /home/usc/cursos/curso1285/home/TFG/problemas/Problema_001/execs/P_001_seq -lm -O0
gcc /home/usc/cursos/curso1285/home/TFG/problemas/Problema_001/openmp/Problema_001_omp.c -o /home/usc/cursos/curso1285/home/TFG/problemas/Problema_001/execs/P_001_omp -lm -fopenmp -O0
gcc /home/usc/cursos/curso1285/home/TFG/problemas/Problema_001/especulativo/Problema_001_esp_1.c -o /home/usc/cursos/curso1285/home/TFG/problemas/Problema_001/execs/P_001_esp_1 -lm -fopenmp -O0
gcc /home/usc/cursos/curso1285/home/TFG/problemas/Problema_001/especulativo/Problema_001_esp_2.c -o /home/usc/cursos/curso1285/home/TFG/problemas/Problema_001/execs/P_001_esp_2 -lm -O0

# Inicializamos las variables de entorno (CANCELLATION)
export OMP_CANCELLATION=true

