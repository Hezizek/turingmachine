#include "ErrorHandler.h"
#include <iostream>

void ErrorHandler::Report(const std::string& errorMessage) {
    std::cerr << errorMessage << std::endl;
}

void ErrorHandler::ReportUsageError() {
    std::cerr << "usage: turing [-v|--verbose] [-h|--help] <tm> <input>" << std::endl;
}

void ErrorHandler::ReportIllegalInput() {
    std::cerr << "illegal input" << std::endl;
}

void ErrorHandler::ReportVerboseIllegalInput(const std::string& inputString, const std::set<char>& inputAlphabet) {
    std::cout << "Input: " << inputString << std::endl;
    std::cout << "==================== ERR ====================" << std::endl;
    
    for (int i = 0; i < inputString.length(); i++) {
        if (inputAlphabet.find(inputString[i]) == inputAlphabet.end()) {
            std::cout << "error: '" << inputString[i] << "' was not declared in the set of input symbols" << std::endl;
            std::cout << "Input: " << inputString << std::endl;
            std::string marker(i, ' ');
            marker += "^";
            std::cout << "       " << marker << std::endl;
            break;
        }
    }
    
    std::cout << "==================== END ====================" << std::endl;
}