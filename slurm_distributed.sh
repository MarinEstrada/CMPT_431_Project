#!/bin/bash
#
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH --ntasks=8
#SBATCH --mem=4G
#SBATCH --time=10:00
#SBATCH --partition=slow


srun ./knapsack_distributed --fName hundred_thousand_input.txt --capacity 100000
