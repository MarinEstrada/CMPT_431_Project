#!/bin/bash
#SBATCH --cpus-per-task=4
#SBATCH --time=10:00
#SBATCH --mem=5G
#SBATCH --partition=slow

echo "Running knapsack_parallel with 4 threads, input hundred_thousand_input.txt & capacity 100000 & granularity 100: Iteration 3"
srun /home/jag18/CMPT_431_Project/knapsack_parallel --fName hundred_thousand_input.txt --capacity 100000 --numThreads=4 --granularity 100
