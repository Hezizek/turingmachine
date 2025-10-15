#pragma once
#include "Direction.h"
#include <string>
#include <vector>

struct Transition {
    std::string oldState;
    std::vector<char> oldSymbols;
    std::vector<char> newSymbols;
    std::vector<Direction> directions;
    std::string newState;
};
