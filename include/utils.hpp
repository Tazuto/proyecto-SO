#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

//  Manipulación de texto 
std::vector<std::string> split(const std::string &s, char delimiter); 
std::string trim(const std::string &s);                                
std::string to_lower(const std::string &s);                            

//  Manejo de errores 
void print_error(const std::string &msg);                              

// Historial de comandos (~/.minishell_history) 
std::string history_file_path();                                       
void history_append(const std::string &line);                          
std::vector<std::string> history_read_all();                           

// Información de memoria (para builtin meminfo)
struct MemInfo {
    size_t total_allocated = 0;  
    size_t total_free = 0;       
};
MemInfo get_meminfo();

#endif 
