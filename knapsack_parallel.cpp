#include <bits/stdc++.h>
#include "core/utils.h"
#include <atomic>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <thread>

#define DEFAULT_CAPACITY "50"

void knapSack(
    const int total_capacity,
    const std::vector<int> &weights,
    const std::vector<int> &profits,
    const int num_items,
    const uint thread_id,
    const uint num_threads,
    const uint granularity,
    std::vector<int> &max_current,
    std::vector<int> &max_prev,
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
                    max_current[w] = std::max(max_prev[w], max_prev[w - weights[i - 1]] + profits[i - 1]);
                } else {
                    max_current[w] = max_prev[w];
                }
                //printf("Max current of item %d in capacity %d is: %d\n", i, w, max_current[i][w]);
            }
            // Check if there are more 
            if (next_initial_weight_range.load() * granularity > total_capacity) {
                // barrier
                the_wall.wait();
                // if (thread_id == 0 && i == num_items) {
                //     for (const auto& row : max_current) { // Loop through each row
                //         for (const auto& element : row) { // Loop through each element in the row
                //             std::cout << element << " "; // Print the element
                //         }
                //         std::cout << std::endl; // Print a newline after each row
                //     }
                // }
                // Swap arrays
                if (thread_id == 0) {
                    uint current_next = next_initial_weight_range.load();
                    while (!next_initial_weight_range.compare_exchange_weak(current_next, num_threads));
                    max_prev = max_current;
                }
                the_wall.wait();
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

void knapSack_default(
    const int total_capacity,
    const std::vector<int> &weights,
    const std::vector<int> &profits,
    const int num_items,
    const uint thread_id,
    std::vector<int> &max_current,
    std::vector<int> &max_prev,
    CustomBarrier &the_wall,
    const uint initial_index,
    const uint end_index
) {
    for (int i = 1; i < num_items + 1; i++) {
        for (int w = initial_index; w < end_index; w++) {
            if (w >= weights[i - 1]) {
                //printf("Comparing previous: %d with %d \nMax current of item %d in capacity %d is: %d\n", max_current[i - 1][w], max_current[i - 1][w - weights[i - 1]] + profits[i - 1], i, w, max_current[i][w]);
                max_current[w] = std::max(max_prev[w], max_prev[w - weights[i - 1]] + profits[i - 1]);
            } else {
                max_current[w] = max_prev[w];
            }
        }
        the_wall.wait();
        if (thread_id == 0) {
            max_prev = max_current;
        }
        the_wall.wait();
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
    // 2d matrix where rows are items and columns are weights
    std::vector<int> max_current(capacity+1, 0);
    std::vector<int> max_prev(capacity+1, 0);
    // Initial index for next weight range
    // Index increases according to granularity and threads take it when they are done with a weight range
    std::atomic<uint> next_initial_weight_range;
    next_initial_weight_range.store(num_threads);
    
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    CustomBarrier the_wall(num_threads);
    timer t1;
    t1.start();

    if (granularity == 0) {
        uint capacity_per_thread = (capacity+1)/num_threads;
        uint start_index = 0;
        uint end_index = start_index+capacity_per_thread+(capacity+1)%num_threads;
        
        for (uint thread_id = 0; thread_id < num_threads; thread_id++) {
            if (thread_id == num_threads - 1) {
                end_index = capacity + 1;
            }
            threads.emplace_back([capacity, &weights, &profits, num_items, thread_id, &max_current, &max_prev, &the_wall, start_index, end_index](){
                knapSack_default(capacity, weights, profits, num_items, thread_id, max_current, max_prev, the_wall, start_index, end_index);});
            start_index = end_index;
            end_index = start_index+capacity_per_thread;
        }
    } else {
        for (uint thread_id = 0; thread_id < num_threads; thread_id++) {
            threads.emplace_back([capacity, &weights, &profits, num_items, thread_id, num_threads, granularity, &max_current, &max_prev, &next_initial_weight_range, &the_wall](){
                knapSack(capacity, weights, profits, num_items, thread_id, num_threads, granularity, max_current, max_prev, next_initial_weight_range, the_wall);});
        }
    }

    for (auto &thread: threads) {
        thread.join();
    }

    auto execution_time = t1.stop();
    // Returning the maximum value of knapsack
    printf("Maximum value of knapsack is %d\nTook %f seconds\n", max_current[capacity], execution_time);
    //std::cout << max_current[num_items][capacity] << std::endl;
    return max_current[capacity];
}

// Driver code
int main(int argc, char* argv[]) {

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
            {"numThreads", "Number of threads",
            cxxopts::value<uint>()->default_value(DEFAULT_CAPACITY)},
            {"granularity", "Granularity for threads to work in",
            cxxopts::value<uint>()->default_value(DEFAULT_CAPACITY)},
        });
    auto cl_options = options.parse(argc, argv);
    std::string file_name = cl_options["fName"].as<std::string>(); // acquire file from where to read input
    int capacity = cl_options["capacity"].as<int>(); // acquire capacity of knapsack
    uint num_threads = cl_options["numThreads"].as<uint>();
    uint granularity = cl_options["granularity"].as<uint>();


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

    std::cout << "Starting knapsack problem\n"
            << "Capacity: " << capacity << "\n"
            << "Num Items: " << num_tuples << "\n"
            << "Num threads: " << num_threads << "\n"
            << "Granularity: " << granularity << std::endl;
    knapSack_parallel(capacity, weights, values, num_tuples, num_threads, granularity);

    return 0;
}