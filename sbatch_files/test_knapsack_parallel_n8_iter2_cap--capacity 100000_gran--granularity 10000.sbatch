#!/bin/bash
#SBATCH --cpus-per-task=8
#SBATCH --time=10:00
#SBATCH --mem=5G
#SBATCH --partition=slow

echo "Running knapsack_parallel with 8 threads, input hundred_thousand_input.txt & capacity 100000 & granularity 10000: Iteration 2"
srun /home/jag18/CMPT_431_Project/knapsack_parallel --fName hundred_thousand_input.txt --capacity 100000 --numThreads=8 --granularity 10000
