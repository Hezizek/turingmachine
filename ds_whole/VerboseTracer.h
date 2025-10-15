#pragma once
#include "types/TuringMachine.h"

class VerboseTracer {
public:
    static void SimulateAndTrace(const TuringMachine& tm, const std::string& input);
};
