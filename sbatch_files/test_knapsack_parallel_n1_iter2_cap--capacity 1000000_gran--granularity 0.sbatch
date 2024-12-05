#!/bin/bash
#SBATCH --cpus-per-task=1
#SBATCH --time=10:00
#SBATCH --mem=5G
#SBATCH --partition=slow

echo "Running knapsack_parallel with 1 threads, input hundred_thousand_input.txt & capacity 1000000 & granularity 0: Iteration 2"
srun /home/jag18/CMPT_431_Project/knapsack_parallel --fName hundred_thousand_input.txt --capacity 1000000 --numThreads=1 --granularity 0
