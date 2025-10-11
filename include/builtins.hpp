#ifndef BUILTINS_HPP
#define BUILTINS_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include "parser.hpp"

// Clase que agrupa los comandos internos (builtins) de la minishell.
class Builtins {
public:
    
    static bool isBuiltin(const std::string &cmd);

    static int execute(const Command &cmd);

private:
    
    static int cmd_cd(const Command &cmd);
    static int cmd_pwd(const Command &cmd);
    static int cmd_help(const Command &cmd);
    static int cmd_history(const Command &cmd);
    static int cmd_alias(const Command &cmd);
    static int cmd_parallel(const Command &cmd);
    static int cmd_meminfo(const Command &cmd);
    static int cmd_exit(const Command &cmd);
    static int cmd_echo(const Command &cmd);

    
    static std::unordered_map<std::string, std::string> &alias_map();
};

#endif 
