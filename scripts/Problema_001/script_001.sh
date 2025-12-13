#!/bin/bash

gcc ../../problemas/Problema_001/sequential/Problema_001_secuencial.c -o P_001_seq -lm
# ---- Configuration ----
PROGRAM=./P_001_seq              # your compiled program name
M=20                             # number of repetitions per N
N_VALUES=(10000 50000 100000 500000 1000000)  # array of N values
RESULT_FILE=/home/TFG/results/Problema_001/P_001_seq.csv
# ---- Execution ----
echo "N, Time" > "$RESULT_FILE"
for N in "${N_VALUES[@]}"; do
    total=0

    for ((i=1; i<=M; i++)); do
        # Run the program and capture its output
        output=$($PROGRAM "$N")
        
        # Extract the number after 'TIME:'
        time=$(echo "$output" | grep "TIME:" | awk '{print $2}')
        
        # Accumulate total
        total=$(echo "$total + $time" | bc -l)
    done

    # Compute average
    avg=$(echo "scale=8; $total / $M" | bc -l)
    echo "$N, $avg" >> $RESULT_FILE
done
