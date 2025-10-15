#include "VerboseTracer.h"
#include "ResultPrinter.h"
#include "MachineSimulator.h"
#include <iostream>

void VerboseTracer::SimulateAndTrace(const TuringMachine& turingMachine, const std::string& inputString) {
    ResultPrinter::PrintVerboseStart(inputString);
    MachineConfiguration config = MachineSimulator::initializeConfiguration(turingMachine, inputString);
    int step = 0;
    while (true) {
        ResultPrinter::PrintVerboseStep(step, config);
        bool matched = false;
        for (const Transition& transition : turingMachine.transitions) {
            if (transition.oldState != config.currentState) {
                continue;
            }
            if (MachineSimulator::matchSymbols(config, transition.oldSymbols, turingMachine.blankSymbol)) {
                MachineSimulator::applyTransition(config, transition, turingMachine.blankSymbol);
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