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

// Function to find the maximum values
int knapSack(const int capacity, std::vector<int> weights, std::vector<int> profits, const int num_items, 
            const int my_rank, const int world_size)
{
    // prep dividing work amongst processes & getting time
    timer program_timer; // will measure time taken by entire program (printed by root process)
    double process_time_taken = 0.0; // will measure time taken by individual process
    uint start_x = 0; // start index for each process
    uint end_x = 0; // end index for each process

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
    uint min_indexes = capacity + 1 / world_size; // minimum indexes per process. capacity + 1 to account for max capacity index
    uint extra_indexes = capacity % world_size; // extra indexes to be distributed amongst processes
    //computing start & end indexes for each process
    if(my_rank < extra_indexes){ // provide it with 1 extra index
        start_x = my_rank * (min_indexes + 1);
        end_x = start_x + min_indexes;
    } else { // no extra indexes
        start_x = my_rank * min_indexes + extra_indexes; // use extra_indexes to determin how many previous processes have been given 1 extra index
        // minux 1 because start index included in num min indexes (eg 0 + 2 = 3 indexes, if only 2ant 2 indexes do (0+2) -1)
        end_x = start_x + min_indexes - 1;
    }
        
    std::vector<int> profit_at_capacity(capacity + 1, 0);

    for (int i = 1; i < num_items; i++) {
        for (int w = capacity; w >= weights[i - 1]; w--) {     
          // Finding the maximum value 
          profit_at_capacity[w] = std::max(profit_at_capacity[w], profit_at_capacity[w - weights[i - 1]] + profits[i - 1]);
        }
        //need to have some sor to sync process for parallel versions HERE.
    }

    if (capacity < weights[num_items - 1]) {
        return profit_at_capacity[capacity];
    }

    profit_at_capacity[capacity] = std::max(profit_at_capacity[capacity],
                                    profit_at_capacity[capacity - weights[num_items - 1]] + profits[num_items - 1]);

    // Returning the maximum value of knapsack
    return profit_at_capacity[capacity];
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

    // // Open the file
    // std::ifstream file(file_name); // Open the file
    // if (!file) {
    //     std::cerr << "Unable to open file.\n";
    //     return 1;
    // }

    // std::string line;
    // std::getline(file, line); // Read the first line for the number of tuples
    // uint num_tuples = std::stoi(line);

    // std::vector<int> values;
    // std::vector<int> weights;

    // while (std::getline(file, line)) { // Read each line, one I/O per line
    //     std::istringstream line_stream(line); // get line stream for parsing in memory
    //     char ch;
    //     int value, weight;

    //     line_stream >> ch >> value >> ch >> weight >> ch; // Parse the tuple format (1, 2)
    //     values.push_back(value); // Add the value to the vector
    //     weights.push_back(weight); // add weight to the vector
    // }
    // file.close(); // Close the file

    // // output
    // for(int i = 0; i < num_tuples; ++i){
    //     std::cout << "(" << values[i] << ", " << weights[i] << ")" 
    //               << "added ints are: " << values[i] + weights[i] << std::endl;
    // }
    std::vector<int> tmp_values = {60, 100, 120};
    std::vector<int> tmp_weights = {10, 20, 30};
    int num_tuples = 3;
    
    //printing out some starter info
    if(world_rank == 0){
        std::cout << "Starting knapsack problem\n"
                  << "Capacity: " << capacity << "\n"
                  << "Num Items: " << num_tuples << "\n"
                  << "Num proccesses: " << world_size << std::endl;
    }

    // cout << knapSack(capacity, weights, values, num_tuples);
    knapSack(capacity, tmp_weights, tmp_values, num_tuples, world_rank, world_size);
    return 0;
}