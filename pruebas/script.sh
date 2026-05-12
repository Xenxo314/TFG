#!/bin/bash

#----------------------------------------------------
# Example OPENMP job script
#----------------------------------------------------
#SBATCH -J TFG_OMP          # Job name
#SBATCH -o TFG_OMP.o        # Name of stdout output file(%j expands to jobId)
#SBATCH -e TFG_OMP.e        # Name of stderr output file(%j expands to jobId)
#SBATCH -c 64               # Cores per task requested
#SBATCH -N 1                # Total # of nodes (must be 1 for OpenMP)
#SBATCH -n 1                # Total # of mpi tasks (should be 1 for OpenMP)
#SBATCH -t 05:59:00         # Run time (hh:mm:ss)
#SBATCH --mem=4G            # Memory 

# ----------------------------------------------------
# Configuración
# ----------------------------------------------------
module load intel
ARCHIVO="/home/usc/cursos/curso1285/home/TFG/pruebas/archivo.c"
EJECUTABLE="./programa"
COMPILADOR="icc"          # cambia a icx o gcc si hace falta
FLAGS="-fopenmp -O3 -lm"
M=1
OMP_THREADS=64

# Argumentos del programa, si los necesita
ARGS=""

# Schedules a probar
SCHEDULES=("static" "guided" "dynamic")

# ----------------------------------------------------
# Compilación
# ----------------------------------------------------
echo "Compilando $ARCHIVO..."
$COMPILADOR $FLAGS "$ARCHIVO" -o "${EJECUTABLE#./}"

if [ $? -ne 0 ]; then
    echo "Error al compilar."
    exit 1
fi

echo "Compilación completada."
echo

export OMP_NUM_THREADS=$OMP_THREADS

# ----------------------------------------------------
# Función para calcular media y desviación típica
# ----------------------------------------------------
calcular_estadisticas() {
    awk '
    {
        x[NR]=$1
        suma += $1
    }
    END {
        if (NR == 0) {
            print "0 0"
            exit
        }

        media = suma / NR

        if (NR == 1) {
            print media, 0
            exit
        }

        for (i = 1; i <= NR; i++) {
            suma2 += (x[i] - media)^2
        }

        desv = sqrt(suma2 / (NR - 1))   # desviación típica muestral
        print media, desv
    }'
}

# ----------------------------------------------------
# Ejecución
# ----------------------------------------------------
for sched in "${SCHEDULES[@]}"; do
    echo "=============================================="
    echo "OMP_SCHEDULE=$sched"
    echo "=============================================="

    export OMP_SCHEDULE="$sched"

    tiempos=""

    for ((i=1; i<=M; i++)); do
        salida=$($EJECUTABLE $ARGS 2>/dev/null)

        tiempo=$(echo "$salida" | grep "TIME:" | awk '{print $2}')

        if [ -z "$tiempo" ]; then
            echo "No se encontró TIME en la salida."
            echo "Salida del programa:"
            echo "$salida"
            exit 1
        fi

        echo "Ejecución $i: $tiempo"
        tiempos+="$tiempo"$'\n'
    done

    stats=$(echo "$tiempos" | calcular_estadisticas)
    media=$(echo "$stats" | awk '{print $1}')
    desv=$(echo "$stats" | awk '{print $2}')

    echo
    echo "Media: $media"
    echo "Desviación típica: $desv"
    echo
done