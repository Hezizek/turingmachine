#include "TMParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <algorithm>

TuringMachine TMParser::Parse(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filePath);
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
                    throw std::runtime_error("Input alphabet symbol not in tape alphabet");
                }
            }
        } else if (trimmed.find("#q0") == 0) {
            tm.initialState = TMParser::parseSingle(trimmed);
            if (tm.states.find(tm.initialState) == tm.states.end()) {
                throw std::runtime_error("Initial state not in states set");
            }
        } else if (trimmed.find("#B") == 0) {
            std::string blankStr = TMParser::parseSingle(trimmed);
            if (blankStr.length() != 1) {
                throw std::runtime_error("Blank symbol must be a single character");
            }
            tm.blankSymbol = blankStr[0];
            if (tm.tapeAlphabet.find(tm.blankSymbol) == tm.tapeAlphabet.end() || tm.blankSymbol != '_') {
                throw std::runtime_error("Blank symbol must be '_' and in tape alphabet");
            }
        } else if (trimmed.find("#F") == 0) {
            tm.finalStates = TMParser::parseSet(trimmed, "state");
            for (const std::string& state : tm.finalStates) {
                if (tm.states.find(state) == tm.states.end()) {
                    throw std::runtime_error("Final state not in states set");
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
    size_t equalPos = line.find('=');
    if (equalPos == std::string::npos) {
        throw std::runtime_error("Syntax error: missing '=' in set definition");
    }
    
    std::string body = line.substr(equalPos + 1);
    body.erase(0, body.find_first_not_of(" \t"));
    body.erase(body.find_last_not_of(" \t") + 1);
    
    if (body.empty() || body[0] != '{' || body[body.length() - 1] != '}') {
        throw std::runtime_error("Syntax error: set must be enclosed in braces");
    }
    
    std::string content = body.substr(1, body.length() - 2);
    std::set<std::string> items;
    std::stringstream ss(content);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        item.erase(0, item.find_first_not_of(" \t"));
        item.erase(item.find_last_not_of(" \t") + 1);
        
        if (item.empty()) {
            throw std::runtime_error("Syntax error: empty item in set");
        }
        
        if (type == "state") {
            for (char c : item) {
                if (!std::isalnum(c) && c != '_') {
                    throw std::runtime_error("Syntax error: invalid character in state name");
                }
            }
        } else if (type == "inputalphabet") {
            if (item.length() != 1 || item == " " || item == "," || item == ";" || 
                item == "{" || item == "}" || item == "*" || item == "_") {
                throw std::runtime_error("Syntax error: invalid input alphabet symbol");
            }
        } else if (type == "tapealphabet") {
            if (item.length() != 1 || item == " " || item == "," || item == ";" || 
                item == "{" || item == "}" || item == "*") {
                throw std::runtime_error("Syntax error: invalid tape alphabet symbol");
            }
        }
        
        items.insert(item);
    }
    
    return items;
}
std::string TMParser::parseSingle(const std::string& line) {
    size_t equalPos = line.find('=');
    if (equalPos == std::string::npos) {
        throw std::runtime_error("Syntax error: missing '=' in single value definition");
    }
    
    std::string value = line.substr(equalPos + 1);
    value.erase(0, value.find_first_not_of(" \t"));
    value.erase(value.find_last_not_of(" \t") + 1);
    
    return value;
}
int TMParser::parseInt(const std::string& line) {
    std::string value = TMParser::parseSingle(line);
    
    if (value.empty() || !std::all_of(value.begin(), value.end(), ::isdigit)) {
        throw std::runtime_error("Syntax error: expected integer value");
    }
    
    return std::stoi(value);
}
Transition TMParser::parseTransition(const std::string& line, int tapeCount, const std::set<std::string>& states, const std::set<char>& symbols) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    
    while (ss >> token) {
        tokens.push_back(token);
    }
    
    if (tokens.size() != 5) {
        throw std::runtime_error("Syntax error: transition must have exactly 5 tokens");
    }
    
    std::string oldState = tokens[0];
    std::string readSymbols = tokens[1];
    std::string writeSymbols = tokens[2];
    std::string directions = tokens[3];
    std::string newState = tokens[4];
    
    if (readSymbols.length() != tapeCount || writeSymbols.length() != tapeCount || directions.length() != tapeCount) {
        throw std::runtime_error("Syntax error: symbol/direction count must match tape count");
    }
    
    if (states.find(oldState) == states.end() || states.find(newState) == states.end()) {
        throw std::runtime_error("Syntax error: state not defined");
    }
    
    for (int i = 0; i < tapeCount; i++) {
        char r = readSymbols[i];
        char w = writeSymbols[i];
        char d = directions[i];
        
        if (r != '*' && symbols.find(r) == symbols.end()) {
            throw std::runtime_error("Syntax error: read symbol not in tape alphabet");
        }
        
        if (w != '*' && symbols.find(w) == symbols.end()) {
            throw std::runtime_error("Syntax error: write symbol not in tape alphabet");
        }
        
        if (d != 'l' && d != 'r' && d != '*') {
            throw std::runtime_error("Syntax error: invalid direction");
        }
    }
    
    Transition transition;
    transition.oldState = oldState;
    
    for (char c : readSymbols) {
        transition.oldSymbols.push_back(c);
    }
    
    for (char c : writeSymbols) {
        transition.newSymbols.push_back(c);
    }
    
    for (char c : directions) {
        if (c == 'l') {
            transition.directions.push_back(Direction::LEFT);
        } else if (c == 'r') {
            transition.directions.push_back(Direction::RIGHT);
        } else {
            transition.directions.push_back(Direction::STAY);
        }
    }
    
    transition.newState = newState;
    
    return transition;
}