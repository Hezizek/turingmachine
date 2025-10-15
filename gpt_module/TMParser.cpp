#include "TMParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <algorithm>

namespace {
    std::string Trim(const std::string& s) {
        auto begin = std::find_if_not(s.begin(), s.end(), [](unsigned char ch) { return std::isspace(ch); });
        auto end = std::find_if_not(s.rbegin(), s.rend(), [](unsigned char ch) { return std::isspace(ch); }).base();
        if (begin >= end) return std::string();
        return std::string(begin, end);
    }

    bool startsWith(const std::string& s, const std::string& prefix) {
        return s.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), s.begin());
    }

    bool endsWith(const std::string& s, const std::string& suffix) {
        return s.size() >= suffix.size() && std::equal(s.end() - suffix.size(), s.end(), suffix.begin());
    }

    std::vector<std::string> SplitByExact(const std::string& str, const std::string& delim) {
        std::vector<std::string> parts;
        size_t start = 0;
        while (true) {
            size_t pos = str.find(delim, start);
            if (pos == std::string::npos) {
                parts.push_back(str.substr(start));
                break;
            }
            parts.push_back(str.substr(start, pos - start));
            start = pos + delim.size();
        }
        for (auto& p : parts) p = Trim(p);
        return parts;
    }

    std::vector<std::string> SplitByWhitespace(const std::string& str) {
        std::vector<std::string> tokens;
        std::istringstream iss(str);
        std::string tok;
        while (iss >> tok) {
            tokens.push_back(tok);
        }
        return tokens;
    }

    bool isInteger(const std::string& s) {
        if (s.empty()) return false;
        return std::all_of(s.begin(), s.end(), [](unsigned char ch) { return std::isdigit(ch); });
    }

    std::set<char> toCharSet(const std::set<std::string>& items) {
        std::set<char> chars;
        for (const auto& it : items) {
            if (it.size() != 1) throw std::runtime_error("syntax error");
            chars.insert(it[0]);
        }
        return chars;
    }
}

TuringMachine TMParser::Parse(const std::string& filePath) {
    std::ifstream in(filePath);
    if (!in.is_open()) {
        throw std::runtime_error("failed to open file");
    }

    TuringMachine tm;
    std::string line;
    while (std::getline(in, line)) {
        std::string trimmed = Trim(line);
        if (trimmed.empty() || (!trimmed.empty() && trimmed[0] == ';')) {
            continue;
        }
        if (startsWith(trimmed, "#Q")) {
            tm.states = TMParser::parseSet(trimmed, "state");
        } else if (startsWith(trimmed, "#S")) {
            std::set<std::string> items = TMParser::parseSet(trimmed, "inputalphabet");
            tm.inputAlphabet = toCharSet(items);
        } else if (startsWith(trimmed, "#G")) {
            std::set<std::string> items = TMParser::parseSet(trimmed, "tapealphabet");
            tm.tapeAlphabet = toCharSet(items);
            for (char symbol : tm.inputAlphabet) {
                if (tm.tapeAlphabet.find(symbol) == tm.tapeAlphabet.end()) {
                    throw std::runtime_error("syntax error");
                }
            }
        } else if (startsWith(trimmed, "#q0")) {
            std::string val = TMParser::parseSingle(trimmed);
            if (tm.states.find(val) == tm.states.end()) {
                throw std::runtime_error("syntax error");
            }
            tm.initialState = val;
        } else if (startsWith(trimmed, "#B")) {
            std::string val = TMParser::parseSingle(trimmed);
            if (val.size() != 1) {
                throw std::runtime_error("syntax error");
            }
            char c = val[0];
            if (tm.tapeAlphabet.find(c) == tm.tapeAlphabet.end() || c != '_') {
                throw std::runtime_error("syntax error");
            }
            tm.blankSymbol = c;
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
    auto parts = SplitByExact(line, " = ");
    if (parts.size() != 2) {
        throw std::runtime_error("syntax error");
    }
    const std::string& body = parts[1];
    if (!(startsWith(body, "{") && endsWith(body, "}"))) {
        throw std::runtime_error("syntax error");
    }
    std::string content = body.substr(1, body.size() - 2);
    std::vector<std::string> rawItems = SplitByExact(content, ",");
    std::set<std::string> result;
    for (std::string item : rawItems) {
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
        result.insert(item);
    }
    return result;
}

std::string TMParser::parseSingle(const std::string& line) {
    auto parts = SplitByExact(line, " = ");
    if (parts.size() != 2) {
        throw std::runtime_error("syntax error");
    }
    return parts[1];
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
    std::vector<std::string> tokens = SplitByWhitespace(line);
    if (tokens.size() != 5) {
        throw std::runtime_error("syntax error");
    }

    const std::string& oldState = tokens[0];
    const std::string& readSymbols = tokens[1];
    const std::string& writeSymbols = tokens[2];
    const std::string& directions = tokens[3];
    const std::string& newState = tokens[4];

    if (static_cast<int>(readSymbols.size()) != tapeCount ||
        static_cast<int>(writeSymbols.size()) != tapeCount ||
        static_cast<int>(directions.size()) != tapeCount) {
        throw std::runtime_error("syntax error");
    }

    if (states.find(oldState) == states.end() || states.find(newState) == states.end()) {
        throw std::runtime_error("syntax error");
    }

    Transition t;
    t.oldState = oldState;
    t.newState = newState;
    t.oldSymbols.reserve(tapeCount);
    t.newSymbols.reserve(tapeCount);
    t.directions.reserve(tapeCount);

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

        t.oldSymbols.push_back(r);
        t.newSymbols.push_back(w);
        if (d == 'l') {
            t.directions.push_back(Direction::LEFT);
        } else if (d == 'r') {
            t.directions.push_back(Direction::RIGHT);
        } else {
            t.directions.push_back(Direction::STAY);
        }
    }

    return t;
}