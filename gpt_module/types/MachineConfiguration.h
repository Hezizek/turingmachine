#pragma once
#include "Tape.h"
#include <string>
#include <vector>

struct MachineConfiguration {
    std::string currentState;
    std::vector<Tape> tapes;
};
