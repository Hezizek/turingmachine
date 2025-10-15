#pragma once
#include "Transition.h"
#include <set>
#include <string>
#include <vector>

struct TuringMachine {
    std::set<std::string> states;
    std::set<char> inputAlphabet;
    std::set<char> tapeAlphabet;
    std::string initialState;
    char blankSymbol;
    std::set<std::string> finalStates;
    int tapeCount;
    std::vector<Transition> transitions;
};