// found at: https://www.geeksforgeeks.org/0-1-knapsack-problem-dp-10/
// C++ program for the above approach

#include <bits/stdc++.h>
#include "core/utils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

// Function to find the maximum values
int knapSack(const int capacity, std::vector<int> weights, std::vector<int> profits, const int num_items)
{
    // Making and initializing max_current array
    // max_current array index represents capacity knapsack can hold at that point
    //     ie: index of 30 can hold capacity of 30 weight,
    //         index of 40 can hold capacity of 40 weight, etc.
    // indexes of higher capacity use previous interation of max_current array & lower indexes to find the max value it can hold
    //    eg: imagine you're faced with an item X of 10 weight
    //        if you're at index 30, can you hold more value by just using the previous index 30?
    //        or can you hold more value by using the previous index 20 (current index - current item weight) and adding the new item
    std::vector<int> max_current(capacity + 1, 0);

    for (int i = 1; i < num_items; i++) {
        for (int w = capacity; w >= weights[i - 1]; w--) {     
          // Finding the maximum value 
          max_current[w] = std::max(max_current[w], max_current[w - weights[i - 1]] + profits[i - 1]);
        }
        //need to have some sor to sync process for parallel versions HERE.
    }

    if (capacity < weights[num_items - 1]) {
        return max_current[capacity];
    }

    max_current[capacity] = std::max(max_current[capacity],
                                    max_current[capacity - weights[num_items - 1]] + profits[num_items - 1]);

    // Returning the maximum value of knapsack
    return max_current[capacity];
}

// Driver code
int main(int argc, char* argv[]) {

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
            cxxopts::value<int>()->default_value("50")},
        });
    auto cl_options = options.parse(argc, argv);
    std::string file_name = cl_options["fName"].as<std::string>();
    int capacity = cl_options["capacity"].as<int>();

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
    // std::vector<int> tmp_values = {60, 100, 120};
    // std::vector<int> tmp_weights = {10, 20, 30};
    // int capacity = 50;
    // int num_tuples = 3;
    std::cout << knapSack(capacity, weights, values, num_tuples) << std::endl;
    // std::cout << knapSack(capacity, tmp_weights, tmp_values, num_tuples);
    return 0;
}