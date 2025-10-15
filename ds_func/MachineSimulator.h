#pragma once
#include "types/TuringMachine.h"
#include "types/MachineConfiguration.h"

class MachineSimulator {
public:
    static MachineConfiguration Simulate(const TuringMachine& tm, const std::string& input);
    static MachineConfiguration initializeConfiguration(const TuringMachine& tm, const std::string& input);
    static bool matchSymbols(const MachineConfiguration& config, const std::vector<char>& expectedSymbols, char blank);
    static void applyTransition(MachineConfiguration& config, const Transition& t, char blank);
};
