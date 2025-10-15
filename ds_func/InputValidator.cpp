#include "InputValidator.h"

bool InputValidator::Validate(const std::string& inputString, const std::set<char>& inputAlphabet) {
    for (size_t i = 0; i < inputString.length(); i++) {
        char symbol = inputString[i];
        if (inputAlphabet.find(symbol) == inputAlphabet.end()) {
            return false;
        }
    }
    return true;
}