#include "builtins.hpp"
#include "utils.hpp"
#include "parser.hpp"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <pwd.h>
#include <pthread.h>
#include <sstream>

using namespace std;

// Estructura simple para pasar el comando a ejecutar al hilo
// (se copia un Command por hilo)
struct ThreadArg {
    Command cmd;
};

// Rutina de hilo: ejecuta el builtin recibido y libera la memoria
static void *run_builtin_thread(void *arg) {
    ThreadArg *ta = static_cast<ThreadArg*>(arg);
    if (ta) {
        Builtins::execute(ta->cmd);
        delete ta;
    }
    return nullptr;
}

// Comprueba si el nombre corresponde a un builtin conocido
bool Builtins::isBuiltin(const std::string &cmd) {
    if (cmd.empty()) return false;
    static const std::vector<std::string> list = {
        "cd", "pwd", "help", "history", "alias", "parallel", "meminfo", "exit", "echo"
    };
    for (auto &c : list) if (c == cmd) return true;
    return false;
}

// Enruta la ejecución al handler correspondiente según el nombre
int Builtins::execute(const Command &cmd) {
    if (cmd.argv.empty()) return 1;
    const string &name = cmd.argv[0];
    if (name == "cd") return cmd_cd(cmd);
    if (name == "pwd") return cmd_pwd(cmd);
    if (name == "help") return cmd_help(cmd);
    if (name == "history") return cmd_history(cmd);
    if (name == "alias") return cmd_alias(cmd);
    if (name == "parallel") return cmd_parallel(cmd);
    if (name == "meminfo") return cmd_meminfo(cmd);
    if (name == "exit") return cmd_exit(cmd);
    if (name == "echo") return cmd_echo(cmd);
    return 1; // builtin desconocido
}

int Builtins::cmd_cd(const Command &cmd) {
    // cd: cambia el directorio actual; si no hay arg va a HOME
    if (cmd.argv.size() < 2) {
        const char *home = getenv("HOME");
        if (!home) {
            struct passwd *pw = getpwuid(getuid());
            if (pw) home = pw->pw_dir;
        }
        if (!home) {
            print_error("cd: HOME no definido");
            return 1;
        }
        if (chdir(home) != 0) {
            perror("cd");
            return 1;
        }
        return 0;
    }
    if (chdir(cmd.argv[1].c_str()) != 0) {
        perror("cd");
        return 1;
    }
    return 0;
}

int Builtins::cmd_pwd(const Command &cmd) {
    // pwd: imprime el directorio de trabajo actual
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
        perror("pwd");
        return 1;
    }
    cout << cwd << endl;
    return 0;
}

int Builtins::cmd_help(const Command &cmd) {
    // help: lista breve de builtins y su uso
    cout << "Comandos internos disponibles:\n";
    cout << "  cd [dir]       - cambiar directorio (HOME si no se especifica)\n";
    cout << "  pwd            - mostrar directorio actual\n";
    cout << "  help           - mostrar esta ayuda\n";
    cout << "  history        - mostrar historial (~/.minishell_history)\n";
    cout << "  alias name=cmd - definir alias; alias (sin args) lista aliases\n";
    cout << "  parallel cmds  - ejecutar varios builtins en paralelo (separar por ';')\n";
    cout << "  meminfo        - mostrar uso de memoria (aprox)\n";
    cout << "  exit           - salir de la shell\n";
    cout << "  echo ...       - imprimir argumentos\n";
    return 0;
}

int Builtins::cmd_history(const Command &cmd) {
    // history: muestra el historial almacenado en ~/.minishell_history
    auto lines = history_read_all();
    if (cmd.argv.size() >= 2) {
        // si se pasa un número, mostrar las últimas N entradas
        try {
            int n = stoi(cmd.argv[1]);
            int start = (int)lines.size() - n;
            if (start < 0) start = 0;
            for (size_t i = start; i < lines.size(); ++i) cout << (i+1) << "  " << lines[i] << '\n';
            return 0;
        } catch (...) {
            // en caso de error al convertir, mostrar todo el historial
        }
    }
    for (size_t i = 0; i < lines.size(); ++i) cout << (i+1) << "  " << lines[i] << '\n';
    return 0;
}

std::unordered_map<std::string, std::string> &Builtins::alias_map() {
    // Almacenamiento estático de aliases: nombre -> comando
    static std::unordered_map<std::string, std::string> map;
    return map;
}

int Builtins::cmd_alias(const Command &cmd) {
    // alias: definir, listar o mostrar alias previamente guardados
    auto &map = alias_map();
    if (cmd.argv.size() == 1) {
        // listar todos
        for (auto &p : map) cout << p.first << "='" << p.second << "'\n";
        return 0;
    }
    // sintaxis: alias nombre=valor
    string token = cmd.argv[1];
    size_t eq = token.find('=');
    if (eq == string::npos) {
        // show single alias
        auto it = map.find(token);
        if (it == map.end()) {
            print_error("alias: no encontrada: " + token);
            return 1;
        }
        cout << it->first << "='" << it->second << "'\n";
        return 0;
    }
    string name = token.substr(0, eq);
    string value = token.substr(eq+1);
    // remove surrounding quotes if present
    if (!value.empty() && value.front() == '\'' && value.back() == '\'') {
        value = value.substr(1, value.size()-2);
    }
    map[name] = value;
    return 0;
}

int Builtins::cmd_parallel(const Command &cmd) {
    // parallel: ejecutar varios builtins en hilos. Los comandos se separan por ';'
    // Solo se permiten builtins dentro de parallel.
    if (cmd.argv.size() < 2) {
        print_error("parallel: requiere al menos un comando (separar por ';')");
        return 1;
    }
    string joined;
    for (size_t i = 1; i < cmd.argv.size(); ++i) {
        if (i > 1) joined += " ";
        joined += cmd.argv[i];
    }
    auto cmds = split(joined, ';');
    vector<pthread_t> threads;
    for (auto &c : cmds) {
        string line = trim(c);
        if (line.empty()) continue;
    // parsear la línea con Parser::parse
        Command sub = Parser::parse(line);
        if (sub.argv.empty()) continue;
        if (!Builtins::isBuiltin(sub.argv[0])) {
            print_error("parallel: solo se soportan builtins en paralelo: " + sub.argv[0]);
            continue;
        }
        ThreadArg *ta = new ThreadArg();
        ta->cmd = sub;
        pthread_t tid;
        if (pthread_create(&tid, nullptr, run_builtin_thread, ta) != 0) {
            print_error("parallel: error creando hilo");
            delete ta;
            continue;
        }
        threads.push_back(tid);
    }
    // esperar a que terminen todos los hilos
    for (auto &t : threads) pthread_join(t, nullptr);
    return 0;
}

int Builtins::cmd_meminfo(const Command &cmd) {
    // meminfo: muestra información aproximada del heap (según allocator)
    MemInfo m = get_meminfo();
    cout << "MemInfo (aprox):\n";
    cout << "  total_allocated: " << m.total_allocated << " bytes\n";
    cout << "  total_free:      " << m.total_free << " bytes\n";
    return 0;
}

int Builtins::cmd_exit(const Command &cmd) {
    // exit: terminar el proceso de la shell
    cout << "Saliendo del minishell..." << endl;
    exit(0);
}

int Builtins::cmd_echo(const Command &cmd) {
    // echo: imprimir argumentos separados por espacio
    for (size_t i = 1; i < cmd.argv.size(); ++i) {
        if (i > 1) cout << ' ';
        cout << cmd.argv[i];
    }
    cout << '\n';
    return 0;
}