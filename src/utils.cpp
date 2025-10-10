#include "utils.hpp"
#include <iostream>
#include <sstream>

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        tokens.push_back(item);
    }
    return tokens;
}

void print_error(const std::string& msg) {
    std::cerr << "Error: " << msg << std::endl;
}