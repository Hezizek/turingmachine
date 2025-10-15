#include "ResultPrinter.h"
#include <iostream>
#include <set>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <vector>

void ResultPrinter::PrintFinalResult(const MachineConfiguration& config) {
    const Tape& tape = config.tapes[0];
    if (tape.cells.empty()) {
        std::cout << "" << std::endl;
        return;
    }
    int left = tape.cells.begin()->first;
    int right = tape.cells.rbegin()->first;
    std::string output;
    for (int i = left; i <= right; ++i) {
        auto it = tape.cells.find(i);
        char symbol = (it != tape.cells.end()) ? it->second : '_';
        output.push_back(symbol);
    }
    std::cout << output << std::endl;
}

void ResultPrinter::PrintVerboseStart(const std::string& inputString) {
    std::cout << "Input: " << inputString << std::endl;
    std::cout << "==================== RUN ====================" << std::endl;
}

void ResultPrinter::PrintVerboseStep(int step, const MachineConfiguration& config) {
    std::cout << "Step   : " << step << std::endl;
    std::cout << "State  : " << config.currentState << std::endl;
    for (std::size_t i = 0; i < config.tapes.size(); ++i) {
        const Tape& tape = config.tapes[i];
        int head = tape.headPosition;

        int left, right;
        if (tape.cells.empty()) {
            left = head;
            right = head;
        } else {
            left = std::min(head, tape.cells.begin()->first);
            right = std::max(head, tape.cells.rbegin()->first);
        }

        std::string indexLine = std::string("Index") + std::to_string(i) + " :";
        std::string symbolLine = std::string("Tape") + std::to_string(i) + "  :";
        std::string headLine = std::string("Head") + std::to_string(i) + "  :";

        for (int j = left; j <= right; ++j) {
            int aj = (j < 0) ? -j : j;
            std::string indexStr = std::to_string(aj);
            auto it = tape.cells.find(j);
            char symbol = (it != tape.cells.end()) ? it->second : '_';
            bool isHead = (j == head);
            std::string pad(indexStr.size(), ' ');

            indexLine += " ";
            indexLine += indexStr;

            symbolLine += pad;
            symbolLine.push_back(symbol);

            headLine += pad;
            headLine.push_back(isHead ? '^' : ' ');
        }

        std::cout << indexLine << std::endl;
        std::cout << symbolLine << std::endl;
        std::cout << headLine << std::endl;
    }
    std::cout << "---------------------------------------------" << std::endl;
}

void ResultPrinter::PrintVerboseResult(const MachineConfiguration& config) {
    const Tape& tape = config.tapes[0];
    if (tape.cells.empty()) {
        std::cout << "Result: " << std::endl;
    } else {
        int left = tape.cells.begin()->first;
        int right = tape.cells.rbegin()->first;
        std::string output;
        for (int i = left; i <= right; ++i) {
            auto it = tape.cells.find(i);
            char symbol = (it != tape.cells.end()) ? it->second : '_';
            output.push_back(symbol);
        }
        std::cout << "Result: " << output << std::endl;
    }
    std::cout << "==================== END ====================" << std::endl;
}