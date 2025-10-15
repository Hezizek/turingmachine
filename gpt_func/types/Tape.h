#pragma once
#include <map>

struct Tape {
    std::map<int, char> cells;
    int headPosition = 0;
};
