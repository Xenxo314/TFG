#!/bin/bash

#SBATCH -J notebook
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -t 02:00:00
#SBATCH --mem=32G
#SBATCH --output=notebook.out
#SBATCH --error=notebook.err

module load python

jupyter nbconvert \
    --to notebook \
    --execute  /home/usc/cursos/curso1285/home/TFG/problemas/Problema_002/scripts/script_002.ipynb\
    --output /home/usc/cursos/curso1285/home/TFG/problemas/Problema_002/sbatch/debug_002.ipynb\
    --ExecutePreprocessor.timeout=-1
