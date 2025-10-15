#include "TMParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <algorithm>

TuringMachine TMParser::Parse(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Cannot open file " + filePath);
    }
    
    TuringMachine tm;
    tm.transitions = std::vector<Transition>();
    std::string line;
    
    while (std::getline(file, line)) {
        std::string trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
        trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);
        
        if (trimmed.empty() || trimmed[0] == ';') {
            continue;
        }
        
        if (trimmed.find("#Q") == 0) {
            tm.states = TMParser::parseSet(trimmed, "state");
        } else if (trimmed.find("#S") == 0) {
            tm.inputAlphabet = TMParser::parseSet(trimmed, "inputalphabet");
        } else if (trimmed.find("#G") == 0) {
            tm.tapeAlphabet = TMParser::parseSet(trimmed, "tapealphabet");
            for (char symbol : tm.inputAlphabet) {
                if (tm.tapeAlphabet.find(symbol) == tm.tapeAlphabet.end()) {
                    throw std::runtime_error("Syntax error: Input symbol not in tape alphabet");
                }
            }
        } else if (trimmed.find("#q0") == 0) {
            tm.initialState = TMParser::parseSingle(trimmed);
            if (tm.states.find(tm.initialState) == tm.states.end()) {
                throw std::runtime_error("Syntax error: Initial state not in states set");
            }
        } else if (trimmed.find("#B") == 0) {
            std::string blankStr = TMParser::parseSingle(trimmed);
            if (blankStr.length() != 1) {
                throw std::runtime_error("Syntax error: Blank symbol must be single character");
            }
            tm.blankSymbol = blankStr[0];
            if (tm.tapeAlphabet.find(tm.blankSymbol) == tm.tapeAlphabet.end() || tm.blankSymbol != '_') {
                throw std::runtime_error("Syntax error: Blank symbol must be '_' and in tape alphabet");
            }
        } else if (trimmed.find("#F") == 0) {
            tm.finalStates = TMParser::parseSet(trimmed, "state");
            for (const std::string& state : tm.finalStates) {
                if (tm.states.find(state) == tm.states.end()) {
                    throw std::runtime_error("Syntax error: Final state not in states set");
                }
            }
        } else if (trimmed.find("#N") == 0) {
            tm.tapeCount = TMParser::parseInt(trimmed);
        } else {
            Transition transition = TMParser::parseTransition(trimmed, tm.tapeCount, tm.states, tm.tapeAlphabet);
            tm.transitions.push_back(transition);
        }
    }
    
    return tm;
}

std::set<std::string> TMParser::parseSet(const std::string& line, const std::string& type) {
    size_t pos = line.find(" = ");
    if (pos == std::string::npos) {
        throw std::runtime_error("Syntax error: Invalid set format");
    }
    
    std::string body = line.substr(pos + 3);
    if (body.length() < 2 || body[0] != '{' || body[body.length() - 1] != '}') {
        throw std::runtime_error("Syntax error: Set must be enclosed in braces");
    }
    
    std::string content = body.substr(1, body.length() - 2);
    std::set<std::string> items;
    std::stringstream ss(content);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        if (item.empty()) {
            throw std::runtime_error("Syntax error: Empty item in set");
        }
        
        if (type == "state") {
            for (char c : item) {
                if (!std::isalnum(c) && c != '_') {
                    throw std::runtime_error("Syntax error: Invalid character in state name");
                }
            }
        } else if (type == "inputalphabet") {
            if (item.length() != 1 || item.find_first_of(" ,;{}*_") != std::string::npos) {
                throw std::runtime_error("Syntax error: Invalid input alphabet symbol");
            }
        } else if (type == "tapealphabet") {
            if (item.length() != 1 || item.find_first_of(" ,;{}*") != std::string::npos) {
                throw std::runtime_error("Syntax error: Invalid tape alphabet symbol");
            }
        }
        
        items.insert(item);
    }
    
    return items;
}

std::string TMParser::parseSingle(const std::string& line) {
    size_t pos = line.find(" = ");
    if (pos == std::string::npos) {
        throw std::runtime_error("Syntax error: Invalid single value format");
    }
    return line.substr(pos + 3);
}

int TMParser::parseInt(const std::string& line) {
    std::string value = TMParser::parseSingle(line);
    for (char c : value) {
        if (!std::isdigit(c)) {
            throw std::runtime_error("Syntax error: Not a valid integer");
        }
    }
    return std::stoi(value);
}

Transition TMParser::parseTransition(const std::string& line, int tapeCount, const std::set<std::string>& states, const std::set<char>& symbols) {
    std::stringstream ss(line);
    std::vector<std::string> tokens;
    std::string token;
    
    while (ss >> token) {
        tokens.push_back(token);
    }
    
    if (tokens.size() != 5) {
        throw std::runtime_error("Syntax error: Transition must have exactly 5 tokens");
    }
    
    std::string oldState = tokens[0];
    std::string readSymbols = tokens[1];
    std::string writeSymbols = tokens[2];
    std::string directions = tokens[3];
    std::string newState = tokens[4];
    
    if (readSymbols.length() != tapeCount || writeSymbols.length() != tapeCount || directions.length() != tapeCount) {
        throw std::runtime_error("Syntax error: Transition symbol/direction count mismatch with tape count");
    }
    
    if (states.find(oldState) == states.end() || states.find(newState) == states.end()) {
        throw std::runtime_error("Syntax error: State not in states set");
    }
    
    for (int i = 0; i < tapeCount; i++) {
        char r = readSymbols[i];
        char w = writeSymbols[i];
        char d = directions[i];
        
        if (r != '*' && symbols.find(r) == symbols.end()) {
            throw std::runtime_error("Syntax error: Read symbol not in tape alphabet");
        }
        if (w != '*' && symbols.find(w) == symbols.end()) {
            throw std::runtime_error("Syntax error: Write symbol not in tape alphabet");
        }
        if (d != 'l' && d != 'r' && d != '*') {
            throw std::runtime_error("Syntax error: Invalid direction");
        }
    }
    
    Transition transition;
    transition.oldState = oldState;
    transition.oldSymbols = std::vector<char>(readSymbols.begin(), readSymbols.end());
    transition.newSymbols = std::vector<char>(writeSymbols.begin(), writeSymbols.end());
    
    std::vector<Direction> dirs;
    for (char d : directions) {
        if (d == 'l') dirs.push_back(Direction::LEFT);
        else if (d == 'r') dirs.push_back(Direction::RIGHT);
        else dirs.push_back(Direction::STAY);
    }
    transition.directions = dirs;
    transition.newState = newState;
    
    return transition;
}
