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
    
    int left = std::min_element(tape.cells.begin(), tape.cells.end())->first;
    int right = std::max_element(tape.cells.begin(), tape.cells.end())->first;
    std::string output;
    
    for (int i = left; i <= right; i++) {
        auto it = tape.cells.find(i);
        char symbol = (it != tape.cells.end()) ? it->second : '_';
        output += symbol;
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
    
    for (int i = 0; i < config.tapes.size(); i++) {
        const Tape& tape = config.tapes[i];
        int head = tape.headPosition;
        
        std::set<int> indices;
        for (const auto& pair : tape.cells) {
            indices.insert(pair.first);
        }
        indices.insert(head);
        
        if (indices.empty()) {
            indices.insert(0);
        }
        
        int left = *indices.begin();
        int right = *indices.rbegin();
        
        std::string indexLine = "Index" + std::to_string(i) + " :";
        std::string symbolLine = "Tape" + std::to_string(i) + "  :";
        std::string headLine = "Head" + std::to_string(i) + "  :";
        
        for (int j = left; j <= right; j++) {
            std::string indexStr = std::to_string(std::abs(j));
            char symbol = '_';
            auto it = tape.cells.find(j);
            if (it != tape.cells.end()) {
                symbol = it->second;
            }
            bool isHead = (j == head);
            
            std::string pad(indexStr.length(), ' ');
            indexLine += " " + indexStr;
            symbolLine += pad + symbol;
            headLine += pad + (isHead ? "^" : " ");
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
        int left = std::min_element(tape.cells.begin(), tape.cells.end())->first;
        int right = std::max_element(tape.cells.begin(), tape.cells.end())->first;
        std::string output;
        
        for (int i = left; i <= right; i++) {
            auto it = tape.cells.find(i);
            char symbol = (it != tape.cells.end()) ? it->second : '_';
            output += symbol;
        }
        
        std::cout << "Result: " << output << std::endl;
    }
    std::cout << "==================== END ====================" << std::endl;
}