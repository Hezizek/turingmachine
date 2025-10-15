#include "TMParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <algorithm>

namespace {
    std::string Trim(const std::string& s) {
        auto begin = s.begin();
        auto end = s.end();

        while (begin != end && std::isspace(static_cast<unsigned char>(*begin))) {
            ++begin;
        }
        if (begin == end) {
            return std::string();
        }
        do {
            --end;
        } while (end != begin && std::isspace(static_cast<unsigned char>(*end)));
        return std::string(begin, end + 1);
    }

    bool startsWith(const std::string& s, const std::string& prefix) {
        return s.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), s.begin());
    }

    std::set<char> toCharSet(const std::set<std::string>& tokens) {
        std::set<char> result;
        for (const auto& tok : tokens) {
            if (tok.size() != 1) {
                throw std::runtime_error("syntax error");
            }
            result.insert(tok[0]);
        }
        return result;
    }
}

TuringMachine TMParser::Parse(const std::string& filePath) {
    std::ifstream in(filePath);
    if (!in) {
        throw std::runtime_error("cannot open file");
    }

    TuringMachine tm;
    tm.transitions.clear();

    std::string line;
    while (std::getline(in, line)) {
        std::string trimmed = Trim(line);
        if (trimmed.empty() || (!trimmed.empty() && trimmed[0] == ';')) {
            continue;
        }

        if (startsWith(trimmed, "#Q")) {
            tm.states = TMParser::parseSet(trimmed, "state");
        } else if (startsWith(trimmed, "#S")) {
            tm.inputAlphabet = toCharSet(TMParser::parseSet(trimmed, "inputalphabet"));
        } else if (startsWith(trimmed, "#G")) {
            tm.tapeAlphabet = toCharSet(TMParser::parseSet(trimmed, "tapealphabet"));
            for (char symbol : tm.inputAlphabet) {
                if (tm.tapeAlphabet.find(symbol) == tm.tapeAlphabet.end()) {
                    throw std::runtime_error("syntax error");
                }
            }
        } else if (startsWith(trimmed, "#q0")) {
            std::string s = TMParser::parseSingle(trimmed);
            if (tm.states.find(s) == tm.states.end()) {
                throw std::runtime_error("syntax error");
            }
            tm.initialState = s;
        } else if (startsWith(trimmed, "#B")) {
            std::string s = TMParser::parseSingle(trimmed);
            if (s.size() != 1) {
                throw std::runtime_error("syntax error");
            }
            char b = s[0];
            if (tm.tapeAlphabet.find(b) == tm.tapeAlphabet.end() || b != '_') {
                throw std::runtime_error("syntax error");
            }
            tm.blankSymbol = b;
        } else if (startsWith(trimmed, "#F")) {
            tm.finalStates = TMParser::parseSet(trimmed, "state");
            for (const auto& state : tm.finalStates) {
                if (tm.states.find(state) == tm.states.end()) {
                    throw std::runtime_error("syntax error");
                }
            }
        } else if (startsWith(trimmed, "#N")) {
            tm.tapeCount = TMParser::parseInt(trimmed);
        } else {
            Transition t = TMParser::parseTransition(trimmed, tm.tapeCount, tm.states, tm.tapeAlphabet);
            tm.transitions.push_back(t);
        }
    }

    return tm;
}
std::set<std::string> TMParser::parseSet(const std::string& line, const std::string& type) {
    size_t pos = line.find(" = ");
    if (pos == std::string::npos || line.find(" = ", pos + 3) != std::string::npos) {
        throw std::runtime_error("syntax error");
    }

    std::string body = Trim(line.substr(pos + 3));
    if (body.size() < 2 || body.front() != '{' || body.back() != '}') {
        throw std::runtime_error("syntax error");
    }

    std::string content = body.substr(1, body.size() - 2);

    std::vector<std::string> items;
    size_t start = 0;
    while (true) {
        size_t comma = content.find(',', start);
        std::string token = (comma == std::string::npos)
            ? content.substr(start)
            : content.substr(start, comma - start);
        token = Trim(token);

        if (token.empty()) {
            throw std::runtime_error("syntax error");
        }

        if (type == "state") {
            for (char c : token) {
                if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '_')) {
                    throw std::runtime_error("syntax error");
                }
            }
        } else if (type == "inputalphabet") {
            if (token.size() != 1) {
                throw std::runtime_error("syntax error");
            }
            char c = token[0];
            if (c == ' ' || c == ',' || c == ';' || c == '{' || c == '}' || c == '*' || c == '_') {
                throw std::runtime_error("syntax error");
            }
        } else if (type == "tapealphabet") {
            if (token.size() != 1) {
                throw std::runtime_error("syntax error");
            }
            char c = token[0];
            if (c == ' ' || c == ',' || c == ';' || c == '{' || c == '}' || c == '*') {
                throw std::runtime_error("syntax error");
            }
        }

        items.push_back(token);

        if (comma == std::string::npos) {
            break;
        }
        start = comma + 1;
    }

    return std::set<std::string>(items.begin(), items.end());
}
std::string TMParser::parseSingle(const std::string& line) {
    size_t pos = line.find(" = ");
    if (pos == std::string::npos || line.find(" = ", pos + 3) != std::string::npos) {
        throw std::runtime_error("syntax error");
    }
    return line.substr(pos + 3);
}
int TMParser::parseInt(const std::string& line) {
    std::string value = TMParser::parseSingle(line);
    value = Trim(value);
    if (value.empty()) {
        throw std::runtime_error("syntax error");
    }
    for (char c : value) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            throw std::runtime_error("syntax error");
        }
    }
    try {
        return std::stoi(value);
    } catch (...) {
        throw std::runtime_error("syntax error");
    }
}
Transition TMParser::parseTransition(
    const std::string& line,
    int tapeCount,
    const std::set<std::string>& states,
    const std::set<char>& symbols
) {
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string tok;
    while (iss >> tok) {
        tokens.push_back(tok);
    }
    if (tokens.size() != 5) {
        throw std::runtime_error("syntax error");
    }

    const std::string& oldState = tokens[0];
    const std::string& readSymbols = tokens[1];
    const std::string& writeSymbols = tokens[2];
    const std::string& directionsStr = tokens[3];
    const std::string& newState = tokens[4];

    if (static_cast<int>(readSymbols.size()) != tapeCount ||
        static_cast<int>(writeSymbols.size()) != tapeCount ||
        static_cast<int>(directionsStr.size()) != tapeCount) {
        throw std::runtime_error("syntax error");
    }

    if (states.find(oldState) == states.end() || states.find(newState) == states.end()) {
        throw std::runtime_error("syntax error");
    }

    std::vector<char> oldSymbolsVec;
    std::vector<char> newSymbolsVec;
    std::vector<Direction> directionsVec;
    oldSymbolsVec.reserve(tapeCount);
    newSymbolsVec.reserve(tapeCount);
    directionsVec.reserve(tapeCount);

    for (int i = 0; i < tapeCount; ++i) {
        char r = readSymbols[i];
        char w = writeSymbols[i];
        char d = directionsStr[i];

        if (r != '*' && symbols.find(r) == symbols.end()) {
            throw std::runtime_error("syntax error");
        }
        if (w != '*' && symbols.find(w) == symbols.end()) {
            throw std::runtime_error("syntax error");
        }
        if (d != 'l' && d != 'r' && d != '*') {
            throw std::runtime_error("syntax error");
        }

        oldSymbolsVec.push_back(r);
        newSymbolsVec.push_back(w);
        if (d == 'l') {
            directionsVec.push_back(Direction::LEFT);
        } else if (d == 'r') {
            directionsVec.push_back(Direction::RIGHT);
        } else {
            directionsVec.push_back(Direction::STAY);
        }
    }

    Transition t;
    t.oldState = oldState;
    t.oldSymbols = std::move(oldSymbolsVec);
    t.newSymbols = std::move(newSymbolsVec);
    t.directions = std::move(directionsVec);
    t.newState = newState;
    return t;
}