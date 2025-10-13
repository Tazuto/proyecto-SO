#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

using namespace std;

//  Manipulación de texto 
vector<string> split(const string &s, char delimiter); 
string trim(const string &s);                                
string to_lower(const string &s);
string expand_home(const string &path);

//  Manejo de errores 
void print_error(const string &msg);                              

// Historial de comandos (~/.minishell_history) 
string history_file_path();                                       
void history_append(const string &line);                          
vector<string> history_read_all();                        

// Información de memoria (para builtin meminfo)
struct MemInfo {
    size_t total_allocated = 0;  
    size_t total_free = 0;       
};
MemInfo get_meminfo();

#endif 
