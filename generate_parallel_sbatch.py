# based off previous generate_sbatch.py files developed by zach1502 for assignement 6
import os
import time
import subprocess
import glob


# Compile the C++ code
subprocess.run(["make"], shell=False)

output_dir = "sbatch_files"
os.makedirs(output_dir, exist_ok=True)

sleep_time = 5 # make sleep time global var to change in one spot

STUDENT_ID = "jag18"
ASSIGNMENT_FOLDER = "CMPT_431_Project"

assert STUDENT_ID and ASSIGNMENT_FOLDER, "Please fill in the STUDENT_ID and ASSIGNMENT_FOLDER variables."

commands = [
    f"/home/{STUDENT_ID}/{ASSIGNMENT_FOLDER}/knapsack_parallel",
]

# chmod the commands
for command in commands:
    subprocess.run(["chmod", "u+x", command], check=True)

nodes = [1]
iterations = 3

# input_files = ["--fName thousand_item_input.txt", "--fName hundred_thousand_item_input.txt", "--fName ten_million_item_input.txt"]
input_files = ["--fName hundred_thousand_input.txt"]
num_threads = [8]
# capacities = ["--capacity 1000", "--capacity 100000", "--capacity 1000000"]
capacities = ["--capacity 1000000"]
granularities1 = ["--granularity 0", "--granularity 10", "--granularity 100"]
granularities2 = ["--granularity 0", "--granularity 100", "--granularity 10000"]
granularities3 = ["--granularity 100000", "--granularity 0", "--granularity 1000", "--granularity 10000"]
# capacities = ["--capacity 1500"]

max_jobs_per_batch = 4
max_total_cpus = 8

output_dir = "sbatch_files"
os.makedirs(output_dir, exist_ok=True)

def generate_sbatch_content(program, num_threads, iteration, params, gran, cap, input_file):
    return f"""#!/bin/bash
#SBATCH --cpus-per-task={num_threads}
#SBATCH --time=10:00
#SBATCH --mem=5G
#SBATCH --partition=slow

echo "Running {program.split('/')[-1]} with {num_threads} threads, input {input_file.split(' ')[-1]} & capacity {cap.split(' ')[-1]} & granularity {gran.split(' ')[-1]}: Iteration {iteration}"
srun {program} {params}
"""

sbatch_files = []
cpu_requests = []
for program in commands:
    program_name = program.split('/')[-1]

    for input_file in input_files:
        for cap in capacities:
            for threads in num_threads:
                granularities = granularities1
                if cap == "--capacity 100000":
                    granularities = granularities2
                elif cap == "--capacity 1000000":
                    granularities = granularities3
                for gran in granularities:
                    params = f"{input_file} {cap} --numThreads={threads} {gran}"
            
    # params = curve_area_params if "curve_area" in program_name else heat_transfer_params
                    
                    for iteration in range(1, iterations + 1):
                        filename = f"test_{program_name}_n{threads}_iter{iteration}_cap{cap}_gran{gran}.sbatch"
                        filepath = os.path.join(output_dir, filename)
                        
                        sbatch_content = generate_sbatch_content(
                            program, threads, iteration, params, gran, cap, input_file
                        )
                        
                        with open(filepath, 'w') as sbatch_file:
                            sbatch_file.write(sbatch_content)
                        
                        sbatch_files.append(filepath)
                        cpu_requests.append(threads)

print(f"Generated {len(sbatch_files)} sbatch files in directory: {output_dir}")

def submit_sbatch(file):
    try:
        subprocess.run(["sbatch", file], check=True)
        print(f"Submitted: {file}")
    except subprocess.CalledProcessError as e:
        print(f"Failed to submit: {file} with error: {e}")

def check_user_jobs(user_id):
    try:
        result = subprocess.run(["squeue", "-u", user_id], stdout = subprocess.PIPE, universal_newlines = True)
        return len(result.stdout.strip().split("\n")) > 1
    except subprocess.CalledProcessError as e:
        print(f"Error checking jobs: {e}")
        return False

i = 0
while i < len(sbatch_files):
    current_batch_jobs = 0
    current_batch_cpus = 0
    
    while current_batch_jobs < max_jobs_per_batch and i < len(sbatch_files):
        job_cpus = cpu_requests[i]
        
        if current_batch_cpus + job_cpus <= max_total_cpus:
            submit_sbatch(sbatch_files[i])
            current_batch_jobs += 1
            current_batch_cpus += job_cpus
            i += 1
        else:
            # over limit
            break

    print(f"Submitted {current_batch_jobs} jobs using {current_batch_cpus} CPUs.")
    print(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))

    while check_user_jobs(STUDENT_ID):
        print(f"Waiting for jobs to finish... checking again in {sleep_time} seconds.")
        time.sleep(sleep_time)

    print("No jobs left. Proceeding to the next batch.")

def combine_slurm_outputs(output_filename="parallel_combined_output.txt"):
    # Get all slurm output files (slurm-*.out)
    slurm_files = glob.glob("slurm-*.out")
    
    if not slurm_files:
        print("No slurm output files found.")
        return
    
    slurm_files.sort(key=lambda x: int(x.split("-")[1].split(".")[0]))

    with open(output_filename, 'w') as combined_file:
        for slurm_file in slurm_files:
            with open(slurm_file, 'r') as sf:
                combined_file.write(f"--- Contents of {slurm_file} ---\n")
                combined_file.write(sf.read())
                combined_file.write("\n\n")
    
    print(f"Combined all slurm output files into {output_filename}")


combine_slurm_outputs("parallel_combined_output.txt")