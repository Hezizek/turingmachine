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
    std::cerr << "Input: " << inputString << std::endl;
    std::cerr << "==================== ERR ====================" << std::endl;
    
    for (size_t i = 0; i < inputString.length(); i++) {
        if (inputAlphabet.find(inputString[i]) == inputAlphabet.end()) {
            std::cerr << "error: '" << inputString[i] << "' was not declared in the set of input symbols" << std::endl;
            std::cerr << "Input: " << inputString << std::endl;
            std::string marker(i, ' ');
            marker += "^";
            std::cerr << "       " << marker << std::endl;
            break;
        }
    }
    
    std::cerr << "==================== END ====================" << std::endl;
}