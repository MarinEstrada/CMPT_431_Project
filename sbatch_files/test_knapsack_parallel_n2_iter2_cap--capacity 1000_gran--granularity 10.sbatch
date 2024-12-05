#!/bin/bash
#SBATCH --cpus-per-task=2
#SBATCH --time=10:00
#SBATCH --mem=5G
#SBATCH --partition=slow

echo "Running knapsack_parallel with 2 threads, input hundred_thousand_input.txt & capacity 1000 & granularity 10: Iteration 2"
srun /home/jag18/CMPT_431_Project/knapsack_parallel --fName hundred_thousand_input.txt --capacity 1000 --numThreads=2 --granularity 10
