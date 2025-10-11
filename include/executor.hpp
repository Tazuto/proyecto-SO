#pragma once
#include "parser.hpp"

using namespace std;

class Executor {
public:
    static void execute(const Command &cmd); // ejecuta un comando ya parseado
};
