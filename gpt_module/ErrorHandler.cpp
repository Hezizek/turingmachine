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

void ErrorHandler::ReportVerboseIllegalInput(const std::string& input, const std::set<char>& inputAlphabet) {
    std::cout << "Input: " << input << std::endl;
    std::cout << "==================== ERR ====================" << std::endl;
    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (inputAlphabet.find(c) == inputAlphabet.end()) {
            std::cout << "error: '" << c << "' was not declared in the set of input symbols" << std::endl;
            std::cout << "Input: " << input << std::endl;
            std::string marker(i, ' ');
            marker.push_back('^');
            std::cout << "       " << marker << std::endl;
            break;
        }
    }
    std::cout << "==================== END ====================" << std::endl;
}
