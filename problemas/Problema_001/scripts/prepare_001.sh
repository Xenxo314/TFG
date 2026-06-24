#!/bin/bash

BASE_DIR=$1

gcc "$BASE_DIR/problemas/Problema_001/src/Problema_001_sec.c" -o "$BASE_DIR/problemas/Problema_001/execs/P_001_seq" -lm -O0
gcc "$BASE_DIR/problemas/Problema_001/src/Problema_001_omp.c" -o "$BASE_DIR/problemas/Problema_001/execs/P_001_omp" -lm -fopenmp -O0
gcc "$BASE_DIR/problemas/Problema_001/src/Problema_001_esp_1.c" -o "$BASE_DIR/problemas/Problema_001/execs/P_001_esp_1" -lm -fopenmp -O0
gcc "$BASE_DIR/problemas/Problema_001/src/Problema_001_esp_2.c" -o "$BASE_DIR/problemas/Problema_001/execs/P_001_esp_2" -lm -O0 -fopenmp


# Inicializamos variables de entorno
export OMP_CANCELLATION=true