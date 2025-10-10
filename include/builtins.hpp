#ifndef BUILTINS_HPP
#define BUILTINS_HPP

#include <vector>
#include <string>

// Ejecuta el comando 'cd'
int builtin_cd(const std::vector<std::string>& args);

// Ejecuta el comando 'exit'
int builtin_exit(const std::vector<std::string>& args);

// Ejecuta el comando 'echo'
int builtin_echo(const std::vector<std::string>& args);

#endif // BUILTINS_HPP