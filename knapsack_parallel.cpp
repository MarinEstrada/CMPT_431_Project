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
                // if (thread_id == 0 && i == num_items) {
                //     for (const auto& row : max_current) { // Loop through each row
                //         for (const auto& element : row) { // Loop through each element in the row
                //             std::cout << element << " "; // Print the element
                //         }
                //         std::cout << std::endl; // Print a newline after each row
                //     }
                // }
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
    timer t1;
    t1.start();
    for (uint thread_id = 0; thread_id < num_threads; thread_id++) {
        threads.emplace_back([capacity, &weights, &profits, num_items, thread_id, num_threads, granularity, &max_current, &next_initial_weight_range, &the_wall](){
            knapSack(capacity, weights, profits, num_items, thread_id, num_threads, granularity, max_current, next_initial_weight_range, the_wall);});
    }

    for (auto &thread: threads) {
        thread.join();
    }
    auto execution_time = t1.stop();
    // Returning the maximum value of knapsack
    printf("For granularity: %u\nWith num_threads: %u\nMaximum value of knapsack is %d\nTook %f seconds\n",granularity, num_threads, max_current[num_items][capacity], execution_time);
    //std::cout << max_current[num_items][capacity] << std::endl;
    return max_current[num_items][capacity];
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

    knapSack_parallel(capacity, weights, values, num_tuples, num_threads, granularity);

    // std::vector<int> tmp_values = {5, 10, 10, 15, 12};
    // std::vector<int> tmp_weights = {3, 4, 5, 6, 7};
    // int capacity = 10;
    // int num_items = 5;
    // uint granularity = 1;
    // uint num_threads = 2;
    // // cout << knapSack_parallel(capacity, weights, values, num_tuples);
    // // auto max_profit = knapSack_parallel(capacity, tmp_weights, tmp_values, num_items, num_threads, granularity);
    // // assert(max_profit == 25);
    // int max_profit;

    // for (uint threads = 1; threads < 10; threads++) {
    //     for (uint gran = 1; gran < 5; gran++) {
    //         max_profit = knapSack_parallel(capacity, tmp_weights, tmp_values, num_items, threads, gran);
    //         assert(max_profit == 25);
    //     }
    // }


    // // testing values → should give 15170
    // tmp_values = {297, 295, 293, 292, 291, 289, 284, 284, 283, 283, 281, 280, 279,
    //                             277, 276, 275, 273,264, 260, 257, 250, 236, 236, 235, 235, 233, 232,
    //                             232, 228, 218, 217, 214, 211, 208, 205, 204, 203, 201, 196, 194, 193,
    //                             193, 192, 191, 190, 187, 187, 184, 184, 184, 181, 179, 176, 173, 172,
    //                             171, 160, 128, 123, 114, 113, 107, 105, 101, 100, 100, 99, 98, 97, 94,
    //                             94, 93, 91, 80, 74, 73, 72, 63, 63, 62, 61, 60, 56, 53, 52, 50, 48, 46,
    //                             40, 40, 35, 28, 22, 22, 18, 15, 12, 11, 6, 5};
    // tmp_weights = {54, 95, 36, 18, 4, 71, 83, 16, 27, 84, 88, 45, 94, 64, 14, 80, 4, 23,
    //                             75, 36, 90, 20, 77, 32, 58, 6, 14, 86, 84, 59, 71, 21, 30, 22, 96, 49, 81,
    //                             48, 37, 28, 6, 84, 19, 55, 88, 38, 51, 52, 79, 55, 70, 53, 64, 99, 61, 86,
    //                             1, 64, 32, 60, 42, 45, 34, 22, 49, 37, 33, 1, 78, 43, 85, 24, 96, 32, 99,
    //                             57, 23, 8, 10, 74, 59, 89, 95, 40, 46, 65, 6, 89, 84, 83, 6, 19, 45, 59,
    //                             26, 13, 8, 26, 5, 9};
    // num_items = 100;
    // capacity = 3818;

    // for (uint threads = 1; threads < 10; threads++) {
    //     for (uint gran = 1; gran < 300; gran+=25) {
    //         max_profit = knapSack_parallel(capacity, tmp_weights, tmp_values, num_items, threads, gran);
    //         assert(max_profit == 15170);
    //     }
    // }

    // // max_profit = knapSack_parallel(capacity, tmp_weights, tmp_values, num_items, num_threads, granularity);
    // // assert(max_profit == 15170);

    // // // testing values → should give → 295
    // tmp_values = {55, 10, 47, 5, 4, 50, 8, 61, 85, 87};
    // tmp_weights = {95, 4, 60, 32, 23, 72, 80, 62, 65, 46};
    // num_items = 10;
    // capacity = 269;

    // for (uint threads = 1; threads < 10; threads++) {
    //     for (uint gran = 1; gran < 300; gran+=25) {
    //         max_profit = knapSack_parallel(capacity, tmp_weights, tmp_values, num_items, threads, gran);
    //         assert(max_profit == 295);
    //     }
    // }

    // max_profit = knapSack_parallel(capacity, tmp_weights, tmp_values, num_items, num_threads, granularity);
    // assert(max_profit == 295);

    return 0;
}