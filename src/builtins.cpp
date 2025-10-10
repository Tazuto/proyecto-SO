#include "builtins.hpp"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
using namespace std;

int builtin_cd(const vector<string>& args) {
    if (args.size() < 2) {
        cerr << "cd: falta argumento\n";
        return 1;
    }
    if (chdir(args[1].c_str()) != 0) {
        perror("cd");
        return 1;
    }
    return 0;
}

int builtin_exit(const vector<string>& args) {
    cout << "Saliendo del minishell...\n";
    exit(0);
}

int builtin_echo(const vector<string>& args) {
    for (size_t i = 1; i < args.size(); ++i) {
        cout << args[i] << " ";
    }
    cout << endl;
    return 0;
}