# Evaluación de Estrategias de Paralelización en Problemas de Cómputo

Este repositorio contiene la implementación y evaluación experimental de distintas estrategias de paralelización aplicadas a tres problemas de cómputo. Para cada problema se desarrollaron cuatro versiones:

- Secuencial
- OpenMP
- ESP1
- ESP2

Los experimentos fueron ejecutados en un entorno HPC gestionado mediante SLURM y los resultados obtenidos se almacenan en ficheros CSV para su posterior análisis y visualización.

---

## Estructura del repositorio

```text
.
├── include/
│   └── run.sh
│
├── problemas/
│   ├── Problema_001/
│   ├── Problema_002/
│   └── Problema_003/
│       ├── src/
│       ├── scripts/
│       ├── results/
│       └── sbatch/
│
└── README.md