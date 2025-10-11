#include "parser.hpp"
#include <sstream>

using namespace std;

Command Parser::parse(const string &line) {
    Command cmd;
    istringstream iss(line);
    string token;

    while (iss >> token) {
        if (token == "&") {
            cmd.background = true;         // Tarea en segundo plano
        } else if (token == ">") {
            iss >> cmd.outputFile;         // Redirección de salida
        } else if (token == "<") {
            iss >> cmd.inputFile;          // Redirección de entrada
        } else {
            cmd.argv.push_back(token);     // Argumentos normales
        }
    }

    return cmd;
}

