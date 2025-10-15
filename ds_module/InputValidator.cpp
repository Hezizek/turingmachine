#include "InputValidator.h"

bool InputValidator::Validate(const std::string& inputString, const std::set<char>& inputAlphabet) {
    for (char symbol : inputString) {
        if (inputAlphabet.find(symbol) == inputAlphabet.end()) {
            return false;
        }
    }
    return true;
}