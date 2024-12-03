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
//          -aided in understanding how to use ofstream to write to a file

#include "core/utils.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

int create_file(std::string file_name, uint num_items, uint min_val, uint max_val, uint min_weight, uint max_weight) {

    std::ofstream file(file_name); // create file and check if properly created

    if (!file) {
        std::cerr << "Error creating file!" << std::endl;
        return 1;
    }

    file << num_items << "\n"; // put number of items at top of file
    
    // seed for rand num generation
    srand(static_cast<unsigned>(time(0)));

    //generate tuples for random values & weights
    for (uint i = 0; i < num_items; ++i) {
        uint value = rand() % max_val + min_val;   // Random value between min_val and max_val
        uint weight = rand() % max_weight + min_weight;   // Random weight between min_weight and max_weight
        file << "(" << value << ", " << weight << ")\n";
    }

    file.close(); // close file
    std::cout << "File \"" << file_name  << "\" created successfully." << std::endl;

    return 0;
}

int main(int argc, char* argv[]) {
    // Initialize command line arguments
    cxxopts::Options options("Create input file",
                            "Creation of input file for knapsack problem");
    options.add_options(
        "custom",
        {
            {"fName", "File Name",
            cxxopts::value<std::string>()->default_value("knapsack_input.txt")},
            {"nItems", "Number of items for knapsack",
            cxxopts::value<uint>()->default_value("1000")},
            {"minVal", "Minimum value for an item",
            cxxopts::value<uint>()->default_value("1")},
            {"maxVal", "Maximum value for an item",
            cxxopts::value<uint>()->default_value("100")},
            {"minWeight", "Minimum weight for an item",
            cxxopts::value<uint>()->default_value("1")},
            {"maxWeight", "Maximum weight for an item",
            cxxopts::value<uint>()->default_value("50")}
        });
    auto cl_options = options.parse(argc, argv);
    std::string file_name = cl_options["fName"].as<std::string>();
    uint num_items = cl_options["nItems"].as<uint>();
    uint min_val = cl_options["minVal"].as<uint>();
    uint max_val = cl_options["maxVal"].as<uint>();
    //if min of value||weight is greater than max, abort mission
    if (min_val > max_val) {
        std::cerr << "Minimum value cannot be greater than maximum value!" << std::endl;
        return 1;
    }
    uint min_weight = cl_options["minWeight"].as<uint>();
    uint max_weight = cl_options["maxWeight"].as<uint>();
    if (min_weight > max_weight) {
        std::cerr << "Minimum weight cannot be greater than maximum weight!" << std::endl;
        return 1;
    }

    return create_file(file_name, num_items, min_val, max_val, min_weight, max_weight);
}

