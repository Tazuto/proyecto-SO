#pragma once
#include <string>
#include <vector>

using namespace std;

struct Command {
    vector<string> argv;   // lista de argumentos del comando (ej: ["ls", "-l"])
    bool background = false; // indica si se ejecuta en segundo plano (&)
    string outputFile;       // archivo para redirección de salida (>)
    string inputFile;        // archivo para redirección de entrada (<)
};

class Parser {
public:
    static Command parse(const string &line); // analiza el comando ingresado
};
