#include "shell.hpp"
#include "parser.hpp"
#include "executor.hpp"
#include "builtins.hpp"
#include <iostream>
#include <csignal>
#include <unistd.h>

using namespace std;

void Shell::showPrompt() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd)); // Obtiene el directorio actual
    cout << "[minishell:" << cwd << "]$ ";
}

void Shell::run() {
    string line;
    signal(SIGINT, SIG_IGN); // Ignorar Ctrl+C en el proceso padre

    while (true) {
        showPrompt();                 // Mostrar el prompt
        if (!getline(cin, line)) break; // Leer línea de comando
        if (line.empty()) continue;     // Si está vacía, continuar
        if (line == "salir") break;     // Comando para terminar la shell
        handleCommand(line);            // Procesar comando
    }

    cout << "Saliendo de la minishell..." << endl;
}

void Shell::handleCommand(const string &line) {
    Command cmd = Parser::parse(line); // Analiza la línea y construye un objeto Command

    // Si el comando es interno (cd, pwd, help, etc.)
    if (Builtins::isBuiltin(cmd.argv[0])) {
        Builtins::execute(cmd);
    } else {
        // Si es un comando del sistema (ls, cat, etc.)
        Executor::execute(cmd);
    }
}

