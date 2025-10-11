#include "builtins.hpp"
#include "utils.hpp"
#include "parser.hpp"
#include <iostream>
#include <unistd.h>
#include <pwd.h>
#include <pthread.h>
#include <unordered_map>

using namespace std;

// Función auxiliar para ejecutar builtins desde hilos
struct ThreadArg { Command cmd; };
static void *run_builtin_thread(void *arg) {
    ThreadArg *ta = static_cast<ThreadArg*>(arg);
    if (ta) { Builtins::execute(ta->cmd); delete ta; }
    return nullptr;
}

bool Builtins::isBuiltin(const string &cmd) {
    static const vector<string> list = {
        "cd","pwd","help","history","alias","parallel","meminfo","exit","echo"
    };
    return find(list.begin(), list.end(), cmd) != list.end();
}

// Tabla de dispatch simplificada
int Builtins::execute(const Command &cmd) {
    if (cmd.argv.empty()) return 1;
    const string &name = cmd.argv[0];

    if      (name == "cd")       return cmd_cd(cmd);
    else if (name == "pwd")      return cmd_pwd(cmd);
    else if (name == "help")     return cmd_help(cmd);
    else if (name == "history")  return cmd_history(cmd);
    else if (name == "alias")    return cmd_alias(cmd);
    else if (name == "parallel") return cmd_parallel(cmd);
    else if (name == "meminfo")  return cmd_meminfo(cmd);
    else if (name == "exit")     return cmd_exit(cmd);
    else if (name == "echo")     return cmd_echo(cmd);
    return 1;
}

// cd: cambia de directorio (usa HOME por defecto)
int Builtins::cmd_cd(const Command &cmd) {
    string path;
    if (cmd.argv.size() > 1) {
        path = cmd.argv[1];
    } else if (const char *h = getenv("HOME")) {
        path = h;
    } else {
        path.clear();
    }

    if (path.empty()) path = expand_home("~");
    if (chdir(path.c_str()) != 0) { perror("cd"); return 1; }
    return 0;
}

int Builtins::cmd_pwd(const Command &) {
    char cwd[4096];
    return getcwd(cwd, sizeof(cwd)) ? (cout << cwd << '\n', 0)
                                    : (perror("pwd"), 1);
}

int Builtins::cmd_help(const Command &) {
    cout <<
      "Comandos internos disponibles:\n"
      "  cd [dir]       - Cambiar directorio\n"
      "  pwd            - Mostrar directorio actual\n"
      "  help           - Mostrar esta ayuda\n"
      "  history [n]    - Mostrar historial\n"
      "  alias name=cmd - Definir alias\n"
      "  parallel cmds  - Ejecutar builtins en paralelo\n"
      "  meminfo        - Mostrar uso de memoria\n"
      "  exit           - Salir de la shell\n"
      "  echo ...       - Imprimir texto\n";
    return 0;
}

// history: muestra el historial
int Builtins::cmd_history(const Command &cmd) {
    auto lines = history_read_all();
    size_t n = lines.size();
    if (cmd.argv.size() > 1)
        try { int k = stoi(cmd.argv[1]); n = min(n, (size_t)k); } catch (...) {}
    for (size_t i = lines.size() - n; i < lines.size(); ++i)
        cout << (i+1) << "  " << lines[i] << '\n';
    return 0;
}

// alias
unordered_map<string,string> &Builtins::alias_map() {
    static unordered_map<string,string> map; return map;
}

int Builtins::cmd_alias(const Command &cmd) {
    auto &map = alias_map();
    if (cmd.argv.size() == 1) {
        for (auto &p : map) cout << p.first << "='" << p.second << "'\n";
        return 0;
    }
    string token = cmd.argv[1];
    size_t eq = token.find('=');
    if (eq == string::npos) {
        auto it = map.find(token);
        return (it != map.end()) ? (cout << it->first << "='" << it->second << "'\n",0)
                                 : (print_error("alias no encontrado: " + token),1);
    }
    map[token.substr(0,eq)] = token.substr(eq+1);
    return 0;
}

// parallel
int Builtins::cmd_parallel(const Command &cmd) {
    if (cmd.argv.size() < 2) { print_error("parallel: requiere comandos"); return 1; }
    string joined; for (size_t i=1;i<cmd.argv.size();++i) joined += (i>1?" ":"") + cmd.argv[i];
    auto cmds = split(joined,';'); vector<pthread_t> threads;
    for (auto &c : cmds) {
        Command sub = Parser::parse(trim(c));
        if (sub.argv.empty() || !isBuiltin(sub.argv[0])) continue;
        ThreadArg *ta = new ThreadArg{ sub };
        pthread_t tid;
        if (!pthread_create(&tid,nullptr,run_builtin_thread,ta))
            threads.push_back(tid);
        else { print_error("parallel: no se pudo crear hilo"); delete ta; }
    }
    for (auto &t : threads) pthread_join(t,nullptr);
    return 0;
}

int Builtins::cmd_meminfo(const Command &) {
    MemInfo m = get_meminfo();
    cout << "MemInfo (aprox):\n"
         << "  total_allocated: " << m.total_allocated << " bytes\n"
         << "  total_free:      " << m.total_free << " bytes\n";
    return 0;
}

int Builtins::cmd_exit(const Command &) {
    cout << "Saliendo del minishell..." << endl;
    exit(0);
}

int Builtins::cmd_echo(const Command &cmd) {
    for (size_t i=1;i<cmd.argv.size();++i) {
        if (i>1) cout << ' ';
        cout << cmd.argv[i];
    }
    cout << '\n';
    return 0;
}
