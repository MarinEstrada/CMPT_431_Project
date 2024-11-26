#include <bits/stdc++.h>
#include "core/utils.h"
#include <atomic>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <thread>

/*
    This function considers different granularities
*/
void knapSack(
    const int total_capacity,
    const std::vector<int> &weights,
    const std::vector<int> &profits,
    const int num_items,
    const uint thread_id,
    const uint num_threads,
    const uint granularity,
    std::vector<std::vector<int>> &max_current,
    std::atomic<uint> &next_initial_weight_range,
    CustomBarrier &the_wall
) {
    uint initial_weight_range = thread_id * granularity;
    for (int i = 1; i < num_items + 1; i++) {
        while(true) {
            //printf("Initial weight is %d\n", initial_weight_range);
            for (int w = initial_weight_range; (w < granularity + initial_weight_range && w <= total_capacity); w++) {     
                // Finding the maximum value
                //printf("In capacity: %d, From Thread: %d \n", w, thread_id);
                // Check if capacity could hold weight of new item
                if (w >= weights[i - 1]) {
                    //printf("Comparing previous: %d with %d \nMax current of item %d in capacity %d is: %d\n", max_current[i - 1][w], max_current[i - 1][w - weights[i - 1]] + profits[i - 1], i, w, max_current[i][w]);
                    max_current[i][w] = std::max(max_current[i - 1][w], max_current[i - 1][w - weights[i - 1]] + profits[i - 1]);
                } else {
                    max_current[i][w] = max_current[i-1][w];
                }
                //printf("Max current of item %d in capacity %d is: %d\n", i, w, max_current[i][w]);
            }
            // Check if there are more 
            if (next_initial_weight_range.load() * granularity > total_capacity) {
                // barrier
                the_wall.wait();
                if (thread_id == 0 && i == num_items) {
                    for (const auto& row : max_current) { // Loop through each row
                        for (const auto& element : row) { // Loop through each element in the row
                            std::cout << element << " "; // Print the element
                        }
                        std::cout << std::endl; // Print a newline after each row
                    }
                }
                uint current_next = next_initial_weight_range.load();
                while (!next_initial_weight_range.compare_exchange_weak(current_next, num_threads));
                break;
            }
            // Get next initial weight range...
            while (true) {
                uint current_next = next_initial_weight_range.load();
                uint next = current_next + 1;
                if (next_initial_weight_range.compare_exchange_weak(current_next, next)) {
                    initial_weight_range = current_next * granularity;
                    break;
                }
            }
        }
        initial_weight_range = thread_id * granularity;
    }
}

// Function to find the maximum values
int knapSack_parallel(
    const int capacity,
    const std::vector<int> &weights,
    const std::vector<int> &profits,
    const int num_items,
    const uint num_threads,
    const uint granularity
) {
    std::vector<std::vector<int>> max_current(num_items + 1, std::vector<int>(capacity+1, 0));
    std::atomic<uint> next_initial_weight_range;
    next_initial_weight_range.store(num_threads);

    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    CustomBarrier the_wall(num_threads);
    
    for (uint thread_id = 0; thread_id < num_threads; thread_id++) {
        threads.emplace_back([capacity, &weights, &profits, num_items, thread_id, num_threads, granularity, &max_current, &next_initial_weight_range, &the_wall](){
            knapSack(capacity, weights, profits, num_items, thread_id, num_threads, granularity, max_current, next_initial_weight_range, the_wall);});
    }

    for (auto &thread: threads) {
        thread.join();
    }
    // Returning the maximum value of knapsack
    std::cout << max_current[num_items][capacity] << std::endl;
    return max_current[num_items][capacity];
}

// Driver code
int main(int argc, char* argv[]) {

    // reading stuff from a file:

    // //init command line args
    // cxxopts::Options options("Read input file",
    //                         "Read input file for knapsack problem");
    // options.add_options(
    //     "custom",
    //     {
    //         {"fName", "File Name",
    //         cxxopts::value<std::string>()->default_value("knapsack_input.txt")},
    //     });
    // auto cl_options = options.parse(argc, argv);
    // std::string file_name = cl_options["fName"].as<std::string>();

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
    std::vector<int> tmp_values = {5, 10, 10, 15, 12};
    std::vector<int> tmp_weights = {3, 4, 5, 6, 7};
    int capacity = 10;
    int num_items = 5;
    uint granularity = 1;
    uint num_threads = 2;
    // cout << knapSack_parallel(capacity, weights, values, num_tuples);
    auto max_profit = knapSack_parallel(capacity, tmp_weights, tmp_values, num_items, num_threads, granularity);
    assert(max_profit == 25);
    return 0;
}