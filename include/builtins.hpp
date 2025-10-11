#ifndef BUILTINS_HPP
#define BUILTINS_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include "parser.hpp"

// Clase que agrupa los builtins de la minishell.
// Provee: isBuiltin() para detectar comandos internos y execute() para ejecutarlos
class Builtins {
public:
	// devuelve true si cmd es un builtin conocido
	static bool isBuiltin(const std::string &cmd);

	// ejecuta el builtin representado por Command (se ejecuta en el proceso padre)
	// devuelve 0 en exito, >0 en error
	static int execute(const Command &cmd);

private:
	// implementaciones internas de cada builtin (devuelven código de salida)
	static int cmd_cd(const Command &cmd);
	static int cmd_pwd(const Command &cmd);
	static int cmd_help(const Command &cmd);
	static int cmd_history(const Command &cmd);
	static int cmd_alias(const Command &cmd);
	static int cmd_parallel(const Command &cmd);
	static int cmd_meminfo(const Command &cmd);
	static int cmd_exit(const Command &cmd);
+
	// echo y utilidades menores
	static int cmd_echo(const Command &cmd);
+
	// mapa de alias (nombre -> reemplazo de cadena)
	static std::unordered_map<std::string, std::string> &alias_map();
};

#endif // BUILTINS_HPP
