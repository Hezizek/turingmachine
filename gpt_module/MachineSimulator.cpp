#include "MachineSimulator.h"

MachineConfiguration MachineSimulator::Simulate(const TuringMachine& tm, const std::string& input) {
    MachineConfiguration config = MachineSimulator::initializeConfiguration(tm, input);
    size_t step = 0;
    while (true) {
        bool matched = false;
        for (const Transition& transition : tm.transitions) {
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
        step += 1;
    }
    return config;
}

MachineConfiguration MachineSimulator::initializeConfiguration(const TuringMachine& tm, const std::string& input) {
    std::vector<Tape> tapes;
    for (int i = 0; i < tm.tapeCount; ++i) {
        Tape tape;
        tape.cells.clear();
        if (i == 0) {
            for (size_t j = 0; j < input.size(); ++j) {
                tape.cells[static_cast<int>(j)] = input[j];
            }
        }
        tape.headPosition = 0;
        tapes.push_back(tape);
    }
    MachineConfiguration config;
    config.currentState = tm.initialState;
    config.tapes = std::move(tapes);
    return config;
}

bool MachineSimulator::matchSymbols(const MachineConfiguration& config, const std::vector<char>& expectedSymbols, char blankSymbol) {
    for (size_t i = 0; i < expectedSymbols.size(); ++i) {
        int headPos = config.tapes[i].headPosition;
        char symbol = blankSymbol;
        auto it = config.tapes[i].cells.find(headPos);
        if (it != config.tapes[i].cells.end()) {
            symbol = it->second;
        }
        char expected = expectedSymbols[i];
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

void MachineSimulator::applyTransition(MachineConfiguration& config, const Transition& t, char /*blankSymbol*/) {
    for (size_t i = 0; i < config.tapes.size(); ++i) {
        Tape& tape = config.tapes[i];
        int headPos = tape.headPosition;
        char writeSymbol = t.newSymbols[i];
        if (writeSymbol != '*') {
            tape.cells[headPos] = writeSymbol;
        }
        Direction direction = t.directions[i];
        if (direction == Direction::LEFT) {
            tape.headPosition = headPos - 1;
        } else if (direction == Direction::RIGHT) {
            tape.headPosition = headPos + 1;
        } else {
            tape.headPosition = headPos;
        }
    }
    config.currentState = t.newState;
}