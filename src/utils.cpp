#include "utils.hpp"
#include <iostream>
#include <sstream>
using namespace std;

vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string item;
    while (getline(ss, item, delimiter)) {
        tokens.push_back(item);
    }
    return tokens;
}

void print_error(const string& msg) {
    cerr << "Error: " << msg << endl;
}