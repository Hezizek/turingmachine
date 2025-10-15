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
    output.reserve(static_cast<size_t>(right - left + 1));
    for (int i = left; i <= right; ++i) {
        char symbol = '_';
        auto it = tape.cells.find(i);
        if (it != tape.cells.end()) {
            symbol = it->second;
        }
        output.push_back(symbol);
    }
    std::cout << output << std::endl;
}
void ResultPrinter::PrintVerboseStart(const std::string& input) {
    std::cout << "Input: " << input << std::endl;
    std::cout << "==================== RUN ====================" << std::endl;
}
void ResultPrinter::PrintVerboseStep(int step, const MachineConfiguration& config) {
    std::cout << "Step   : " << step << std::endl;
    std::cout << "State  : " << config.currentState << std::endl;

    for (size_t i = 0; i < config.tapes.size(); ++i) {
        const Tape& tape = config.tapes[i];
        int head = tape.headPosition;

        int left = head;
        int right = head;
        if (!tape.cells.empty()) {
            left = std::min(left, tape.cells.begin()->first);
            right = std::max(right, tape.cells.rbegin()->first);
        }

        std::ostringstream indexPrefix, tapePrefix, headPrefix;
        indexPrefix << "Index" << i << " :";
        tapePrefix << "Tape" << i << "  :";
        headPrefix << "Head" << i << "  :";

        std::string indexLine = indexPrefix.str();
        std::string symbolLine = tapePrefix.str();
        std::string headLine = headPrefix.str();

        for (int j = left; j <= right; ++j) {
            std::string indexStr = std::to_string(std::abs(j));
            char symbol = '_';
            auto it = tape.cells.find(j);
            if (it != tape.cells.end()) {
                symbol = it->second;
            }
            bool isHead = (j == head);
            std::string pad(indexStr.size(), ' ');

            indexLine += " " + indexStr;
            symbolLine += pad;
            symbolLine.push_back(symbol);
            headLine += pad;
            headLine += (isHead ? "^" : " ");
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
        output.reserve(static_cast<size_t>(right - left + 1));
        for (int i = left; i <= right; ++i) {
            char symbol = '_';
            auto it = tape.cells.find(i);
            if (it != tape.cells.end()) {
                symbol = it->second;
            }
            output.push_back(symbol);
        }
        std::cout << "Result: " << output << std::endl;
    }
    std::cout << "==================== END ====================" << std::endl;
}