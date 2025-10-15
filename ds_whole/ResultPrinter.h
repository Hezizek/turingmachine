#pragma once
#include "types/MachineConfiguration.h"
#include <string>

class ResultPrinter {
public:
    static void PrintFinalResult(const MachineConfiguration& config);
    static void PrintVerboseStart(const std::string& input);
    static void PrintVerboseStep(int step, const MachineConfiguration& config);
    static void PrintVerboseResult(const MachineConfiguration& config);
};
