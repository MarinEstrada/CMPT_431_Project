// found at: https://www.geeksforgeeks.org/0-1-knapsack-problem-dp-10/
// C++ program for the above approach

#include <bits/stdc++.h>
#include "core/utils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <mpi.h>

#define DEFAULT_CAPACITY "50"

void knapsack_parallel(const int capacity, std::vector<int> weights, std::vector<int> profits, const int num_items,
                        std::vector<int> &profit_at_capacity, double &time_taken, const int start_x, const int end_x,
                        const int my_rank, const int world_size, std::vector<int> &displacements, std::vector<int> &counts)
{
    timer process_timer;   // will measure time taken by individual process
    process_timer.start(); // starting timer
    // std::vector<int> prev_profits_at_capacity (capacity + 1, 0);

    for (int i = 1; i < num_items; i++) {
        std::vector<int> changes_to_make; // vector to store changes to be made to profit_at_capacity array
        std::vector<int> changes_to_process;
        std::vector<int> num_changes_per_process(world_size, 0); // vector to store number of changes to be made per process
        std::vector<int> change_displacements_per_process(world_size, 0); // vector to store displacements for changes to be made per process
        for (int c = end_x; c >= weights[i-1] && c >= start_x; c--) {
            // Finding the maximum value at capacity c
            int tmp_profit = profit_at_capacity[c - weights[i - 1]] + profits[i - 1]; // what if we add current item
            if (profit_at_capacity[c] < tmp_profit) { // if adding current item is more profitable...add it
                profit_at_capacity[c] = tmp_profit;
                changes_to_make.push_back(c); // add index where change is to be made to later send
                changes_to_make.push_back(profit_at_capacity[c]); // add new value to be inserted to later send
                // if(my_rank == 0) std::cout << "c: " << c << ", profit_at_capacity[c]: " << profit_at_capacity[c] << "\n";
            }
        }
        // need to have some sor to sync process for parallel versions HERE.
        // MPI_Allgatherv(profit_at_capacity.data() + start_x, counts[my_rank], MPI_INT, prev_profits_at_capacity.data(), counts.data(), displacements.data(), MPI_INT, MPI_COMM_WORLD);
        // std::swap(profit_at_capacity, prev_profits_at_capacity);
        //------------ new attempt----------------
        int num_changes = changes_to_make.size(); // get size of what to send
        MPI_Allgather(&num_changes, 1, MPI_INT, num_changes_per_process.data(), 1, MPI_INT, MPI_COMM_WORLD); //gather info on how many changes to expect (will work as counts[] in next all gather)
        int total_changes = 0; // total number of changes to be made, a counter
        for(int i = 0; i < world_size; ++i) {
            change_displacements_per_process[i] = total_changes; // set displacement for each process
            total_changes += num_changes_per_process[i]; // add up all changes to be made
        }
        changes_to_process.resize(total_changes); // resize vector to hold all changes
        MPI_Allgatherv(changes_to_make.data(), num_changes, MPI_INT, changes_to_process.data(), num_changes_per_process.data(), change_displacements_per_process.data(), MPI_INT, MPI_COMM_WORLD); // gather all changes to be made
        // apply changes to profit_at_capacity array
        for(int i = 0; i < total_changes; i += 2) {
            profit_at_capacity[changes_to_process[i]] = changes_to_process[i + 1];
        }



    }

    // for last process in charge of last index, check if last item should be used for max capacity
    // can ignore other indexes since they won't be used anymore
    if ((my_rank == (world_size - 1)) && (capacity >= weights[num_items - 1])) {
        profit_at_capacity[capacity] = std::max(profit_at_capacity[capacity],
                                                profit_at_capacity[capacity - weights[num_items - 1]] + profits[num_items - 1]);
    }

    time_taken = process_timer.stop(); // stopping timer
}

// Function to find the maximum values
void knapSack(const int capacity, std::vector<int> weights, std::vector<int> profits, const int num_items,
            const int my_rank, const int world_size)
{
    // prep dividing work amongst processes & getting time
    timer program_timer; // will measure time taken by entire program (printed by last process)
    double time_taken = 0.0; // will measure time taken by individual process
    uint start_x = 0; // start index for each process
    uint end_x = 0; // end index for each process
    int dummy_var = 0; // dummy variable for syncing processes

    //start timer & start distributing
    //--------------------------------------------------------------------------------
    // EXPLANATION ON HOW THIS WORKS -> THUS WHY WE DISTRIBUTE THE WAY WE DO
    // we will be working on profit_at_capacity array
    // index of profit_at_capacity array represents capacity knapsack can hold at that point
    //     ie: index of 30 can hold capacity of 30 weight,
    //         index of 40 can hold capacity of 40 weight, etc.
    // indexes of higher capacity use previous interation of profit_at_capacity array & lower indexes to find the max value it can hold
    //    eg: imagine you're faced with an item X of 10 weight
    //        if you're at index 30, can you hold more value by just using the previous index 30?
    //        or can you hold more value by using the previous index 20 (current index - curret item weight) and adding the new item
    // We distributing work by divying up indexes of profit_at_capacity array to be the responsibility of differing processes
    //--------------------------------------------------------------------------------
    program_timer.start(); //starting timer
    uint min_indexes = (capacity + 1) / world_size; // minimum indexes per process. capacity + 1 to account for max capacity index
    uint extra_indexes = (capacity + 1) % world_size; // extra indexes to be distributed amongst processes
    std::vector<int> displacements(world_size, 0); // vector to store displacements for MPI_Gatherv
    std::vector<int> counts(world_size, 0); // vector to store counts for MPI_Gatherv
    //computing start & end indexes for each process
    if(my_rank < extra_indexes){ // provide it with 1 extra index
        start_x = my_rank * (min_indexes + 1);
        end_x = start_x + min_indexes;
    } else { // no extra indexes
        start_x = my_rank * min_indexes + extra_indexes; // use extra_indexes to determin how many previous processes have been given 1 extra index
        // minux 1 because start index included in num min indexes (eg 0 + 2 = 3 indexes, if only 2ant 2 indexes do (0+2) -1)
        end_x = start_x + min_indexes - 1;
    }
    int disp_to_send = start_x; // set displacement for current process
    int count_to_send = end_x - start_x + 1; // set count for current process
    MPI_Allgather(&disp_to_send, 1, MPI_INT, displacements.data(), 1, MPI_INT, MPI_COMM_WORLD); // gather displacements
    MPI_Allgather(&count_to_send, 1, MPI_INT, counts.data(), 1, MPI_INT, MPI_COMM_WORLD); // gather counts

    // std::cout << "Process " << my_rank << " has start index: " << start_x << " and end index: " << end_x << "\n";
    // if(my_rank == 0) {
    //     std::cout << "displacements are: ";
    //     for(auto val : displacements) {
    //         std::cout << val << ", ";
    //     }
    //     std::cout << "\n";
    // }
        
    //create vector to store profit at capacities 0 through capacity
    std::vector<int> profit_at_capacity(capacity + 1, 0);

    knapsack_parallel(capacity, weights, profits, num_items, profit_at_capacity, time_taken,
                      start_x, end_x, my_rank, world_size, displacements, counts);

    //--------------------------------------------------------------------------------
    // PRINTING RESULTS
    //--------------------------------------------------------------------------------
    if(my_rank == 0) std::cout << "Process_rank, start_index, end_index, time_taken\n"; // only root prints result header

    //for syncing processes use send msg, tag of 2 for dummy variable
    if(my_rank != 0) MPI_Recv(&dummy_var, 1, MPI_INT, my_rank - 1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    std::cout << my_rank << ", " << start_x << ", " << end_x << ", " << std::setprecision(TIME_PRECISION) << time_taken << "\n"; // printing of results
    if(my_rank != world_size - 1) MPI_Send(&dummy_var, 1, MPI_INT, my_rank + 1, 2, MPI_COMM_WORLD);

    if(my_rank == world_size -1 ){ // only last process prints final time & result
        time_taken = program_timer.stop();
        std::cout << "Total time taken: " << std::setprecision(TIME_PRECISION) << time_taken << " seconds\n";
        std::cout << "Maximum value: " << profit_at_capacity[capacity] << std::endl;
    }

    // return profit_at_capacity[capacity];
}

// Driver code
int main(int argc, char* argv[]) {

      //first setting up MPI
    MPI_Init(&argc, &argv);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // get current proccess rank
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // get size of our world (eg: num proccesses)

    // reading stuff from a file:

    //init command line args
    cxxopts::Options options("Read input file",
                            "Read input file for knapsack problem");
    options.add_options(
        "custom",
        {
            {"fName", "File Name",
            cxxopts::value<std::string>()->default_value("knapsack_input.txt")},
            {"capacity", "Capacity of the knapsack",
            cxxopts::value<int>()->default_value(DEFAULT_CAPACITY)},
        });
    auto cl_options = options.parse(argc, argv);
    std::string file_name = cl_options["fName"].as<std::string>(); // acquire file from where to read input
    int capacity = cl_options["capacity"].as<int>(); // acquire capacity of knapsack

    // Open the file
    std::ifstream file(file_name); // Open the file
    if (!file) {
        std::cerr << "Unable to open file.\n";
        return 1;
    }

    std::string line;
    std::getline(file, line); // Read the first line for the number of tuples
    uint num_tuples = std::stoi(line);

    std::vector<int> values;
    std::vector<int> weights;

    while (std::getline(file, line)) { // Read each line, one I/O per line
        std::istringstream line_stream(line); // get line stream for parsing in memory
        char ch;
        int value, weight;

        line_stream >> ch >> value >> ch >> weight >> ch; // Parse the tuple format (1, 2)
        values.push_back(value); // Add the value to the vector
        weights.push_back(weight); // add weight to the vector
    }
    file.close(); // Close the file

    // // output
    // for(int i = 0; i < num_tuples; ++i){
    //     std::cout << "(" << values[i] << ", " << weights[i] << ")" 
    //               << "added ints are: " << values[i] + weights[i] << std::endl;
    // }
    
    // //testing values → should give 220
    // std::vector<int> values = {60, 100, 120};
    // std::vector<int> weights = {10, 20, 30};
    // int num_tuples = 3;

    // these tests values taken from https://www.researchgate.net/publication/349878676_A_Phase_Angle-Modulated_Bat_Algorithm_with_Application_to_Antenna_Topology_Optimization#pf9

    // // testing values → should give 15170
    // std::vector<int> values = {297, 295, 293, 292, 291, 289, 284, 284, 283, 283, 281, 280, 279,
    //                             277, 276, 275, 273,264, 260, 257, 250, 236, 236, 235, 235, 233, 232,
    //                             232, 228, 218, 217, 214, 211, 208, 205, 204, 203, 201, 196, 194, 193,
    //                             193, 192, 191, 190, 187, 187, 184, 184, 184, 181, 179, 176, 173, 172,
    //                             171, 160, 128, 123, 114, 113, 107, 105, 101, 100, 100, 99, 98, 97, 94,
    //                             94, 93, 91, 80, 74, 73, 72, 63, 63, 62, 61, 60, 56, 53, 52, 50, 48, 46,
    //                             40, 40, 35, 28, 22, 22, 18, 15, 12, 11, 6, 5};
    // std::vector<int> weights = {54, 95, 36, 18, 4, 71, 83, 16, 27, 84, 88, 45, 94, 64, 14, 80, 4, 23,
    //                             75, 36, 90, 20, 77, 32, 58, 6, 14, 86, 84, 59, 71, 21, 30, 22, 96, 49, 81,
    //                             48, 37, 28, 6, 84, 19, 55, 88, 38, 51, 52, 79, 55, 70, 53, 64, 99, 61, 86,
    //                             1, 64, 32, 60, 42, 45, 34, 22, 49, 37, 33, 1, 78, 43, 85, 24, 96, 32, 99,
    //                             57, 23, 8, 10, 74, 59, 89, 95, 40, 46, 65, 6, 89, 84, 83, 6, 19, 45, 59,
    //                             26, 13, 8, 26, 5, 9};
    // int num_tuples = 100;
    // capacity = 3818;

    // // testing values → should give → 295
    // std::vector<int> values = {55, 10, 47, 5, 4, 50, 8, 61, 85, 87};
    // std::vector<int> weights = {95, 4, 60, 32, 23, 72, 80, 62, 65, 46};
    // int num_tuples = 10;
    // capacity = 269;
    
    //printing out some starter info
    if(world_rank == 0){
        std::cout << "Starting knapsack problem\n"
                  << "Capacity: " << capacity << "\n"
                  << "Num Items: " << num_tuples << "\n"
                  << "Num proccesses: " << world_size << std::endl;
    }

    knapSack(capacity, weights, values, num_tuples, world_rank, world_size);
    MPI_Finalize();
    return 0;
}