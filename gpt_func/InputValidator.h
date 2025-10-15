#pragma once
#include <string>
#include <set>

class InputValidator {
public:
    static bool Validate(const std::string& input, const std::set<char>& inputAlphabet);
};