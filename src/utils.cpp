#include "utils.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <unistd.h>
#include <pwd.h>
#if defined(__GLIBC__)
#include <malloc.h>
#endif

using namespace std;

// Obtiene el directorio HOME del usuario actual
static string get_home_dir() {
    const char *home = getenv("HOME");
    if (!home) {
        if (struct passwd *pw = getpwuid(getuid()))
            home = pw->pw_dir;
    }
    return home ? string(home) : ".";
}

// Muestra mensaje de error simple
void print_error(const string &msg) {
    cerr << "Error: " << msg << '\n';
}

// Utilidades de string
vector<string> split(const string &s, char delimiter) {
    istringstream ss(s);
    vector<string> out; string token;
    while (getline(ss, token, delimiter)) out.push_back(token);
    return out;
}

string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    return (a == string::npos) ? "" : s.substr(a, b - a + 1);
}

string to_lower(const string &s) {
    string r = s; transform(r.begin(), r.end(), r.begin(), ::tolower); return r;
}

string expand_home(const string &path) {
    if (path.empty() || path[0] != '~') return path;
    string home = get_home_dir();
    return (path == "~") ? home : home + path.substr(1);
}

// Manejo de historial
string history_file_path() { return get_home_dir() + "/.minishell_history"; }

void history_append(const string &line) {
    ofstream(history_file_path(), ios::app) << line << '\n';
}

vector<string> history_read_all() {
    ifstream f(history_file_path());
    vector<string> out; string line;
    while (getline(f, line)) out.push_back(line);
    return out;
}

// Información de memoria
MemInfo get_meminfo() {
    MemInfo m{};
#if defined(__GLIBC__)
    auto mi = mallinfo();
    m.total_allocated = mi.uordblks;
    m.total_free = mi.fordblks;
#else
    void *brk = sbrk(0);
    if (brk != (void*)-1) m.total_allocated = reinterpret_cast<size_t>(brk);
#endif
    return m;
}
