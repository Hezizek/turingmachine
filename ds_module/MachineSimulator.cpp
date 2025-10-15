#include "MachineSimulator.h"

MachineConfiguration MachineSimulator::Simulate(const TuringMachine& turingMachine, const std::string& inputString) {
    MachineConfiguration config = MachineSimulator::initializeConfiguration(turingMachine, inputString);
    int step = 0;
    while (true) {
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
    return config;
}

MachineConfiguration MachineSimulator::initializeConfiguration(const TuringMachine& turingMachine, const std::string& inputString) {
    std::vector<Tape> tapes;
    for (int i = 0; i < turingMachine.tapeCount; i++) {
        Tape tape;
        if (i == 0) {
            for (int j = 0; j < inputString.length(); j++) {
                tape.cells[j] = inputString[j];
            }
        }
        tape.headPosition = 0;
        tapes.push_back(tape);
    }
    MachineConfiguration config;
    config.currentState = turingMachine.initialState;
    config.tapes = tapes;
    return config;
}

bool MachineSimulator::matchSymbols(const MachineConfiguration& config, const std::vector<char>& oldSymbols, char blankSymbol) {
    for (int i = 0; i < oldSymbols.size(); i++) {
        int headPos = config.tapes[i].headPosition;
        char symbol = blankSymbol;
        if (config.tapes[i].cells.find(headPos) != config.tapes[i].cells.end()) {
            symbol = config.tapes[i].cells.at(headPos);
        }
        char expected = oldSymbols[i];
        if (expected == '*') {
            if (symbol == blankSymbol) {
                return false;
            }
        } else {
            if (symbol != expected) {
                return false;
            }
        }
    }
    return true;
}

void MachineSimulator::applyTransition(MachineConfiguration& config, const Transition& transition, char blankSymbol) {
    for (int i = 0; i < config.tapes.size(); i++) {
        Tape& tape = config.tapes[i];
        int headPos = tape.headPosition;
        char writeSymbol = transition.newSymbols[i];
        if (writeSymbol != '*') {
            tape.cells[headPos] = writeSymbol;
        }
        Direction direction = transition.directions[i];
        if (direction == Direction::LEFT) {
            tape.headPosition = headPos - 1;
        } else if (direction == Direction::RIGHT) {
            tape.headPosition = headPos + 1;
        } else {
            tape.headPosition = headPos;
        }
    }
    config.currentState = transition.newState;
}