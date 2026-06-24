#!/bin/bash

#SBATCH -J notebook
#SBATCH -n 1
#SBATCH -c 16
#SBATCH -t 04:30:00
#SBATCH --mem=32G
#SBATCH --output=notebook.out
#SBATCH --error=notebook.err

module load python

jupyter nbconvert \
    --to notebook \
    --execute  /home/usc/cursos/curso1285/home/TFG/problemas/Problema_001/scripts/script_001.ipynb\
    --output /home/usc/cursos/curso1285/home/TFG/problemas/Problema_001/sbatch/debug_001.ipynb\
    --ExecutePreprocessor.timeout=-1
