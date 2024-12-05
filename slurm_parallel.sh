#!/bin/bash
#
#SBATCH --cpus-per-task=8
#SBATCH --time=10:00
#SBATCH --mem=5G
#SBATCH --partition=slow

srun ./knapsack_parallel --fName=hundred_thousand_input.txt --capacity=100000 --numThreads=4 --granularity=100
