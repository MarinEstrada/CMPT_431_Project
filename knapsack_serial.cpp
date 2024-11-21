// found at: https://www.geeksforgeeks.org/0-1-knapsack-problem-dp-10/
// C++ program for the above approach

#include <bits/stdc++.h>
#include "core/utils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

// Function to find the maximum values
int knapSack(int capacity, std::vector<int> wt, std::vector<int> val, int n)
{
    // Making and initializing dp array
    int dp[capacity + 1];
    memset(dp, 0, sizeof(dp));

    for (int i = 1; i < n + 1; i++) {
        for (int w = capacity; w >= 0; w--) {

            if (wt[i - 1] <= w)
                
                // Finding the maximum value
                dp[w] = max(dp[w],
                            dp[w - wt[i - 1]] + val[i - 1]);
        }
    }
    // Returning the maximum value of knapsack
    return dp[capacity];
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
        });
    auto cl_options = options.parse(argc, argv);
    std::string file_name = cl_options["fName"].as<std::string>();

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

    //output
    for(int i = 0; i < num_tuples; ++i){
        std::cout << "(" << values[i] << ", " << weights[i] << ")" 
                  << "added ints are: " << values[i] + weights[i] << std::endl;
    }
    int capacity = 50;
    int n = sizeof(values) / sizeof(values[0]);
    cout << knapSack(capacity, weights, values, n);
    return 0;
}