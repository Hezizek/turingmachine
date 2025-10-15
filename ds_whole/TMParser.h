#pragma once
#include "types/TuringMachine.h"
#include <string>

class TMParser {
public:
    static TuringMachine Parse(const std::string& filePath);

private:
    static std::set<std::string> parseSet(const std::string& line, const std::string& type);
    static std::string parseSingle(const std::string& line);
    static int parseInt(const std::string& line);
    static Transition parseTransition(
        const std::string& line,
        int tapeCount,
        const std::set<std::string>& states,
        const std::set<char>& symbols
    );
};
