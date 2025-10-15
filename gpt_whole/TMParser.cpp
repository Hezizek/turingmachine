#include "TMParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <algorithm>

namespace {
    inline std::string Trim(const std::string& s) {
        size_t start = 0;
        while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) ++start;
        size_t end = s.size();
        while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;
        return s.substr(start, end - start);
    }

    inline bool startsWith(const std::string& s, const std::string& prefix) {
        return s.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), s.begin());
    }

    inline bool endsWith(const std::string& s, const std::string& suffix) {
        return s.size() >= suffix.size() && std::equal(suffix.rbegin(), suffix.rend(), s.rbegin());
    }

    inline std::vector<std::string> SplitOnce(const std::string& s, const std::string& delim) {
        std::vector<std::string> parts;
        size_t pos = s.find(delim);
        if (pos == std::string::npos) {
            parts.push_back(s);
        } else {
            parts.push_back(s.substr(0, pos));
            parts.push_back(s.substr(pos + delim.size()));
        }
        return parts;
    }

    inline std::vector<std::string> SplitByComma(const std::string& s) {
        std::vector<std::string> items;
        std::string current;
        std::istringstream iss(s);
        while (std::getline(iss, current, ',')) {
            items.push_back(current);
        }
        return items;
    }

    inline bool isInteger(const std::string& s) {
        if (s.empty()) return false;
        for (char c : s) {
            if (!std::isdigit(static_cast<unsigned char>(c))) return false;
        }
        return true;
    }

    inline std::set<char> ToCharSet(const std::set<std::string>& strs) {
        std::set<char> out;
        for (const auto& it : strs) {
            if (it.size() == 1) {
                out.insert(it[0]);
            }
        }
        return out;
    }
}

TuringMachine TMParser::Parse(const std::string& filePath) {
    std::ifstream fin(filePath.c_str());
    if (!fin) {
        throw std::runtime_error("syntax error");
    }
    TuringMachine tm;
    tm.transitions.clear();
    std::string line;
    while (std::getline(fin, line)) {
        std::string trimmed = Trim(line);
        if (trimmed.empty() || startsWith(trimmed, ";")) {
            continue;
        }
        if (startsWith(trimmed, "#Q")) {
            tm.states = TMParser::parseSet(trimmed, "state");
        } else if (startsWith(trimmed, "#S")) {
            std::set<std::string> s = TMParser::parseSet(trimmed, "inputalphabet");
            tm.inputAlphabet = ToCharSet(s);
        } else if (startsWith(trimmed, "#G")) {
            std::set<std::string> s = TMParser::parseSet(trimmed, "tapealphabet");
            tm.tapeAlphabet = ToCharSet(s);
            for (char symbol : tm.inputAlphabet) {
                if (tm.tapeAlphabet.find(symbol) == tm.tapeAlphabet.end()) {
                    throw std::runtime_error("syntax error");
                }
            }
        } else if (startsWith(trimmed, "#q0")) {
            std::string s = TMParser::parseSingle(trimmed);
            tm.initialState = s;
            if (tm.states.find(tm.initialState) == tm.states.end()) {
                throw std::runtime_error("syntax error");
            }
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
            Transition transition = TMParser::parseTransition(trimmed, tm.tapeCount, tm.states, tm.tapeAlphabet);
            tm.transitions.push_back(transition);
        }
    }
    return tm;
}

std::set<std::string> TMParser::parseSet(const std::string& line, const std::string& type) {
    auto parts = SplitOnce(line, " = ");
    if (parts.size() != 2) {
        throw std::runtime_error("syntax error");
    }
    std::string body = Trim(parts[1]);
    if (!(startsWith(body, "{") && endsWith(body, "}"))) {
        throw std::runtime_error("syntax error");
    }
    std::string content = body.substr(1, body.size() - 2);
    auto rawItems = SplitByComma(content);
    std::set<std::string> items;
    for (auto item : rawItems) {
        item = Trim(item);
        if (item.empty()) {
            throw std::runtime_error("syntax error");
        }
        if (type == "state") {
            for (char c : item) {
                if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '_')) {
                    throw std::runtime_error("syntax error");
                }
            }
        } else if (type == "inputalphabet") {
            if (item.size() != 1) {
                throw std::runtime_error("syntax error");
            }
            char c = item[0];
            if (c == ' ' || c == ',' || c == ';' || c == '{' || c == '}' || c == '*' || c == '_') {
                throw std::runtime_error("syntax error");
            }
        } else if (type == "tapealphabet") {
            if (item.size() != 1) {
                throw std::runtime_error("syntax error");
            }
            char c = item[0];
            if (c == ' ' || c == ',' || c == ';' || c == '{' || c == '}' || c == '*') {
                throw std::runtime_error("syntax error");
            }
        }
        items.insert(item);
    }
    return items;
}

std::string TMParser::parseSingle(const std::string& line) {
    auto parts = SplitOnce(line, " = ");
    if (parts.size() != 2) {
        throw std::runtime_error("syntax error");
    }
    return Trim(parts[1]);
}

int TMParser::parseInt(const std::string& line) {
    std::string value = TMParser::parseSingle(line);
    if (!isInteger(value)) {
        throw std::runtime_error("syntax error");
    }
    return std::stoi(value);
}

Transition TMParser::parseTransition(
    const std::string& line,
    int tapeCount,
    const std::set<std::string>& states,
    const std::set<char>& symbols
) {
    std::istringstream iss(line);
    std::string oldState, readSymbols, writeSymbols, directions, newState;
    if (!(iss >> oldState >> readSymbols >> writeSymbols >> directions >> newState)) {
        throw std::runtime_error("syntax error");
    }
    // Ensure no extra tokens
    std::string extra;
    if (iss >> extra) {
        throw std::runtime_error("syntax error");
    }

    if (static_cast<int>(readSymbols.size()) != tapeCount ||
        static_cast<int>(writeSymbols.size()) != tapeCount ||
        static_cast<int>(directions.size()) != tapeCount) {
        throw std::runtime_error("syntax error");
    }
    if (states.find(oldState) == states.end() || states.find(newState) == states.end()) {
        throw std::runtime_error("syntax error");
    }

    for (int i = 0; i < tapeCount; ++i) {
        char r = readSymbols[static_cast<size_t>(i)];
        char w = writeSymbols[static_cast<size_t>(i)];
        char d = directions[static_cast<size_t>(i)];
        if (r != '*' && symbols.find(r) == symbols.end()) {
            throw std::runtime_error("syntax error");
        }
        if (w != '*' && symbols.find(w) == symbols.end()) {
            throw std::runtime_error("syntax error");
        }
        if (!(d == 'l' || d == 'r' || d == '*')) {
            throw std::runtime_error("syntax error");
        }
    }

    Transition t;
    t.oldState = oldState;
    t.oldSymbols.assign(readSymbols.begin(), readSymbols.end());
    t.newSymbols.assign(writeSymbols.begin(), writeSymbols.end());
    t.directions.clear();
    t.directions.reserve(static_cast<size_t>(tapeCount));
    for (char d : directions) {
        if (d == 'l') t.directions.push_back(Direction::LEFT);
        else if (d == 'r') t.directions.push_back(Direction::RIGHT);
        else t.directions.push_back(Direction::STAY);
    }
    t.newState = newState;
    return t;
}