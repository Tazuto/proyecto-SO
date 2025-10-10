#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

// Divide una cadena por un delimitador
std::vector<std::string> split(const std::string& str, char delimiter);

// Imprime un mensaje de error
void print_error(const std::string& msg);

#endif // UTILS_HPP