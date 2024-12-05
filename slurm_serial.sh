#!/bin/bash
#
#SBATCH --cpus-per-task=1
#SBATCH --time=10:00
#SBATCH --mem=5G
#SBATCH --partition=slow

srun ./knapsack_serial --fName hundred_thousand_input.txt --capacity 1000000
