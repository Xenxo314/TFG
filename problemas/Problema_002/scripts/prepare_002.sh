#!/bin/bash

BASE_DIR=$1

gcc "$BASE_DIR/problemas/Problema_002/src/Problema_002_sec.c" -o "$BASE_DIR/problemas/Problema_002/execs/P_002_seq" -lm -O0
gcc "$BASE_DIR/problemas/Problema_002/src/Problema_002_omp.c" -o "$BASE_DIR/problemas/Problema_002/execs/P_002_omp" -lm -fopenmp -O0
gcc "$BASE_DIR/problemas/Problema_002/src/Problema_002_esp_1.c" -o "$BASE_DIR/problemas/Problema_002/execs/P_002_esp_1" -lm -fopenmp -O0
gcc "$BASE_DIR/problemas/Problema_002/src/Problema_002_esp_2.c" -o "$BASE_DIR/problemas/Problema_002/execs/P_002_esp_2" -lm -O0 -fopenmp


# Inicializamos variables de entorno
export OMP_CANCELLATION=true