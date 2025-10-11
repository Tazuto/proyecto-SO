#include "executor.hpp"
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

using namespace std;

void Executor::execute(const Command &cmd) {
    pid_t pid = fork(); // Crear un nuevo proceso

    if (pid < 0) {
        perror("Error al crear proceso");
        return;
    }

    if (pid == 0) { // Proceso hijo
        // Redirección de salida (>)
        if (!cmd.outputFile.empty()) {
            int fd = open(cmd.outputFile.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd < 0) {
                perror("Error abriendo archivo de salida");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO); // Redirige stdout al archivo
            close(fd);
        }

        // Redirección de entrada (<)
        if (!cmd.inputFile.empty()) {
            int fd = open(cmd.inputFile.c_str(), O_RDONLY);
            if (fd < 0) {
                perror("Error abriendo archivo de entrada");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO); // Redirige stdin al archivo
            close(fd);
        }

        // Preparar argumentos para execvp
        vector<char*> args;
        for (auto &arg : cmd.argv) args.push_back(const_cast<char*>(arg.c_str()));
        args.push_back(nullptr);

        execvp(args[0], args.data()); // Ejecutar comando
        perror("Error ejecutando comando"); // Solo se ejecuta si exec falla
        exit(EXIT_FAILURE);
    } 
    else { // Proceso padre
        if (!cmd.background) {
            // Esperar a que termine el hijo si no está en background
            waitpid(pid, nullptr, 0);
        } else {
            cout << "[Proceso en segundo plano: " << pid << "]" << endl;
        }
    }
}

