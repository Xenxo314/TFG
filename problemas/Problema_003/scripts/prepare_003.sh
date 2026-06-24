#!/bin/bash

BASE_DIR=$1

gcc "$BASE_DIR/problemas/Problema_003/src/Problema_003_sec.c" -o "$BASE_DIR/problemas/Problema_003/execs/P_003_seq" -lm -O0
gcc "$BASE_DIR/problemas/Problema_003/src/Problema_003_omp.c" -o "$BASE_DIR/problemas/Problema_003/execs/P_003_omp" -lm -fopenmp -O0
gcc "$BASE_DIR/problemas/Problema_003/src/Problema_003_esp_1.c" -o "$BASE_DIR/problemas/Problema_003/execs/P_003_esp_1" -lm -fopenmp -O0
gcc "$BASE_DIR/problemas/Problema_003/src/Problema_003_esp_2.c" -o "$BASE_DIR/problemas/Problema_003/execs/P_003_esp_2" -lm -O0 -fopenmp


# Inicializamos variables de entorno
export OMP_CANCELLATION=true