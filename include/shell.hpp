#pragma once
#include <string>

using namespace std;

class Shell {
public:
    void run();  // inicia el ciclo principal del shell
private:
    void handleCommand(const string &line); // analiza y ejecuta cada línea
    void showPrompt();                      // muestra el prompt en pantalla
};
