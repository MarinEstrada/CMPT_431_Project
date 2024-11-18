// Adrian Marin Estrada, Raymond Kong, Juan Antonio Gonzalez
// computing ID: ama269
// Student #: 301240077
// CMPT 431
// CITATIONS:
//      github copilot:
//          -logic is my own
//          -helped write code more quickly by finishing variable names
//          -Have setting where codpilot is allowed to take from other public repos OFF
//      previous_assignments:
//        -used prev assignemnts as reference in using cxxopts
//      chatGPT:
//          -aided in understanding how to use ifstream, getline, and istringstream to read from a file
#include "core/utils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

int main(int argc, char* argv[]) {

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

    return 0;
}
