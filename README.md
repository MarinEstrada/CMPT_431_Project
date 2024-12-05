# CMPT 431 Project
By [Adrian Marin Estrada], Juan Antonio Gonzalez, [Raymond Kong] 

## Project description

We parallelized the serial implementation of the Knapsack Problem by [GeeksforGeeks] using threads and MPI in C++

## Installing proper libraries
Our code is implemented to be run using SFU's Slurm Cluster
If you plan to run it on your personal machine unix-based machine please ensure you have C++11 or higher by running `g++ --version`.  If you do not have C++ installed, please install gcc:
```sh
sudo apt update
sudo apt install g++ -y
```
Ensure you have the MPI compiler and runtime environment on your machine by running
```sh
mpicc --version
mpirun --version
```
_If you do not have MPI on your **Linux** machine, run:_

```sh
sudo apt install mpich
sudo apt install openmpi-bin
```
_If you do not have MPI on your **Mac** machine, run:_

```sh
brew install mpich
```
## To run on Slurm:
If running on slurm, you can run our three test scripts `generate_serial_sbatch.py`, `generate_serial_sbatch.py` , or `generate_distributed_sbatch.py` using the command:
```sh
python3 <test_script>
```
**Before running the test scripts**, ensure that you have editted the script to include your computing ID, and the path to where you have stored this folder.

If you wish to run programs individually, use our shell scripts `slurm_serial.sh`, `slurm_parallel.sh`, or `slurm_distributed.sh` using the command:
```sh
make
sbatch <shell_script>
```
## To run on your machine:
to run on your machine, using the following commands:
_For the serial implementation:_
```sh
make 
./knapsack_serial --fName hundred_thousand_input.txt --capacity 1000000
```
_For the thread implementation:_
```sh
make 
./knapsack_parallel --fName=hundred_thousand_input.txt --capacity=100000 --numThreads=4 --granularity=100
```
_For the distributed implementation:_
```sh
make 
mpirun  -np 8 ./knapsack_serial --fName hundred_thousand_input.txt --capacity 1000000
```

For the MPI implementation, _ensure that your computer can handle 8 processes._

## Editing the parameters
For all three implemenations you can edit the `capacity` parameter to change the capacity of the knapsack, and edit `fName` to edit what the input file you wish to use is.

Specific to our thread implementation, you can edit `numThreads` to change the number of threads being used by the program, and `granularity` to change the granularity used in the program with 0 being no granularity being used (work is evently distributed  at the start), and numbers > 0 indicating how much work each thread does at one time.

Specific to our distributed implementation, to change the number of processes used by MPI provide a different number after the `np` option in 
```sh
mpirun -np 8 ./knapsack_distributed --fName hundred_thousand_input.txt --capacity 1000000
```

## To create your own input file
To create your own input file run:
```sh
make
./create_file --minWeight 100 --maxWeight 5000 --fName hundred_thousand_input.txt --nItems 100000 --maxVal 10000
```
You can edit `minWeight` & `maxWeight` to change the weight range of created items.
Change `minVal` & `maxVal` to change the value range of created items.
Change `nItems` to change number of items in your new input file.
Change `fName` to change the name of your new input file.


## File Structure:
```
CMPT_431_Project
├── README.md
└── core
    ├── cxxopts.h
    ├── get_time.h
    ├── types.h
    └── utils.h
```
### Programs:
## Results

## Code References
| File/Folder | README |
| ------ | ------ |
| Core folder | [Provided by our 431 course][431 course] |
| Serial Knapsack Implementation | [By GeeksforGeeks, lightly improved by us][geeksforgeeks]

[//]: # (Reference links:)
[adrian marin estrada]: <https://marinestrada.github.io/>
[raymond kong]: <https://rkong04.github.io/>
[431 course]: <https://www.sfu.ca/outlines.html?2024/fall/cmpt/431/d100>
[geeksforgeeks]: <https://www.geeksforgeeks.org/0-1-knapsack-problem-dp-10/>

