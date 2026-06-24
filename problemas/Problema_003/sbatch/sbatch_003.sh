#!/bin/bash

#SBATCH -J notebook_3
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -t 04:00:00
#SBATCH --mem=32G
#SBATCH --output=notebook.out
#SBATCH --error=notebook.err

module load python

jupyter nbconvert \
    --to notebook \
    --execute  /home/usc/cursos/curso1285/home/TFG/problemas/Problema_003/scripts/script_003.ipynb\
    --output /home/usc/cursos/curso1285/home/TFG/problemas/Problema_003/sbatch/debug_003.ipynb\
    --ExecutePreprocessor.timeout=-1
