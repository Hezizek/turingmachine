#include "ResultPrinter.h"
#include <iostream>
#include <set>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <vector>

void ResultPrinter::PrintFinalResult(const MachineConfiguration& config) {
    const Tape& tape = config.tapes[0];
    std::vector<int> indices;
    indices.reserve(tape.cells.size());
    for (const auto& kv : tape.cells) {
        indices.push_back(kv.first);
    }
    std::sort(indices.begin(), indices.end());
    if (indices.empty()) {
        std::cout << "" << std::endl;
        return;
    }
    int left = indices.front();
    int right = indices.back();
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
    for (size_t i = 0; i < config.tapes.size(); ++i) {
        const Tape& tape = config.tapes[i];
        int head = tape.headPosition;
        std::vector<int> indices;
        indices.reserve(tape.cells.size() + 1);
        for (const auto& kv : tape.cells) {
            indices.push_back(kv.first);
        }
        indices.push_back(head);
        std::sort(indices.begin(), indices.end());
        indices.erase(std::unique(indices.begin(), indices.end()), indices.end());
        int left = indices.front();
        int right = indices.back();

        std::ostringstream indexLine;
        std::ostringstream symbolLine;
        std::ostringstream headLine;

        indexLine << "Index" << i << " :";
        symbolLine << "Tape" << i << "  :";
        headLine << "Head" << i << "  :";

        for (int j = left; j <= right; ++j) {
            int aj = j < 0 ? -j : j;
            std::string indexStr = std::to_string(aj);
            char symbol = '_';
            auto it = tape.cells.find(j);
            if (it != tape.cells.end()) symbol = it->second;

            bool isHead = (j == head);
            std::string pad(indexStr.size(), ' ');

            indexLine << " " << indexStr;
            symbolLine << pad << symbol;
            if (isHead) {
                headLine << pad << "^";
            } else {
                headLine << pad << " ";
            }
        }

        std::cout << indexLine.str() << std::endl;
        std::cout << symbolLine.str() << std::endl;
        std::cout << headLine.str() << std::endl;
    }
    std::cout << "---------------------------------------------" << std::endl;
}

void ResultPrinter::PrintVerboseResult(const MachineConfiguration& config) {
    const Tape& tape = config.tapes[0];
    std::vector<int> indices;
    indices.reserve(tape.cells.size());
    for (const auto& kv : tape.cells) {
        indices.push_back(kv.first);
    }
    std::sort(indices.begin(), indices.end());
    if (indices.empty()) {
        std::cout << "Result: " << std::endl;
    } else {
        int left = indices.front();
        int right = indices.back();
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