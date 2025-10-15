#include "VerboseTracer.h"
#include "ResultPrinter.h"
#include "MachineSimulator.h"
#include <iostream>

void VerboseTracer::SimulateAndTrace(const TuringMachine& tm, const std::string& input) {
    ResultPrinter::PrintVerboseStart(input);
    MachineConfiguration config = MachineSimulator::initializeConfiguration(tm, input);
    int step = 0;
    while (true) {
        ResultPrinter::PrintVerboseStep(step, config);
        bool matched = false;
        for (const auto& transition : tm.transitions) {
            if (transition.oldState != config.currentState) {
                continue;
            }
            if (MachineSimulator::matchSymbols(config, transition.oldSymbols, tm.blankSymbol)) {
                MachineSimulator::applyTransition(config, transition, tm.blankSymbol);
                matched = true;
                break;
            }
        }
        if (!matched) {
            break;
        }
        step = step + 1;
    }
    ResultPrinter::PrintVerboseResult(config);
}