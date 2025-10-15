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
        while (begin != end && std::isspace(static_cast<unsigned char>(*begin))) ++begin;
        while (begin != end && std::isspace(static_cast<unsigned char>(*(end - 1)))) --end;
        return std::string(begin, end);
    }

    bool startsWith(const std::string& s, const std::string& prefix) {
        return s.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), s.begin());
    }

    bool endsWith(const std::string& s, const std::string& suffix) {
        return s.size() >= suffix.size() &&
               std::equal(suffix.rbegin(), suffix.rend(), s.rbegin());
    }

    std::vector<std::string> SplitExact(const std::string& s, const std::string& delim) {
        std::vector<std::string> parts;
        std::size_t pos = s.find(delim);
        if (pos == std::string::npos) {
            parts.push_back(s);
        } else {
            parts.push_back(s.substr(0, pos));
            parts.push_back(s.substr(pos + delim.size()));
        }
        return parts;
    }

    std::vector<std::string> SplitByChar(const std::string& s, char delim) {
        std::vector<std::string> parts;
        std::string item;
        std::stringstream ss(s);
        while (std::getline(ss, item, delim)) {
            parts.push_back(item);
        }
        return parts;
    }

    bool isInteger(const std::string& s) {
        if (s.empty()) return false;
        for (char c : s) {
            if (!std::isdigit(static_cast<unsigned char>(c))) return false;
        }
        return true;
    }
}

TuringMachine TMParser::Parse(const std::string& filePath) {
    std::ifstream in(filePath);
    if (!in) {
        throw std::runtime_error("failed to open file");
    }

    TuringMachine tm;
    tm.transitions.clear();

    std::string line;
    while (std::getline(in, line)) {
        std::string trimmed = Trim(line);
        if (trimmed.empty() || startsWith(trimmed, ";")) {
            continue;
        }
        if (startsWith(trimmed, "#Q")) {
            std::set<std::string> states = TMParser::parseSet(trimmed, "state");
            tm.states = states;
        } else if (startsWith(trimmed, "#S")) {
            std::set<std::string> items = TMParser::parseSet(trimmed, "inputalphabet");
            std::set<char> alpha;
            for (const auto& it : items) {
                if (it.size() != 1) {
                    throw std::runtime_error("syntax error");
                }
                alpha.insert(it[0]);
            }
            tm.inputAlphabet = alpha;
        } else if (startsWith(trimmed, "#G")) {
            std::set<std::string> items = TMParser::parseSet(trimmed, "tapealphabet");
            std::set<char> alpha;
            for (const auto& it : items) {
                if (it.size() != 1) {
                    throw std::runtime_error("syntax error");
                }
                alpha.insert(it[0]);
            }
            tm.tapeAlphabet = alpha;
            for (char symbol : tm.inputAlphabet) {
                if (tm.tapeAlphabet.find(symbol) == tm.tapeAlphabet.end()) {
                    throw std::runtime_error("syntax error");
                }
            }
        } else if (startsWith(trimmed, "#q0")) {
            std::string init = TMParser::parseSingle(trimmed);
            if (tm.states.find(init) == tm.states.end()) {
                throw std::runtime_error("syntax error");
            }
            tm.initialState = init;
        } else if (startsWith(trimmed, "#B")) {
            std::string bs = TMParser::parseSingle(trimmed);
            if (bs.size() != 1) {
                throw std::runtime_error("syntax error");
            }
            char b = bs[0];
            if (tm.tapeAlphabet.find(b) == tm.tapeAlphabet.end() || b != '_') {
                throw std::runtime_error("syntax error");
            }
            tm.blankSymbol = b;
        } else if (startsWith(trimmed, "#F")) {
            std::set<std::string> finals = TMParser::parseSet(trimmed, "state");
            for (const auto& state : finals) {
                if (tm.states.find(state) == tm.states.end()) {
                    throw std::runtime_error("syntax error");
                }
            }
            tm.finalStates = finals;
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
    auto parts = SplitExact(line, " = ");
    if (parts.size() != 2) {
        throw std::runtime_error("syntax error");
    }
    std::string body = Trim(parts[1]);
    if (!(startsWith(body, "{") && endsWith(body, "}"))) {
        throw std::runtime_error("syntax error");
    }
    std::string content = body.substr(1, body.size() - 2);
    std::vector<std::string> items = SplitByChar(content, ',');
    std::set<std::string> result;
    for (auto& rawItem : items) {
        std::string item = Trim(rawItem);
        if (item.empty()) {
            throw std::runtime_error("syntax error");
        }
        if (type == "state") {
            for (char c : item) {
                bool ok = (c >= 'a' && c <= 'z') ||
                          (c >= 'A' && c <= 'Z') ||
                          (c >= '0' && c <= '9') ||
                          (c == '_');
                if (!ok) {
                    throw std::runtime_error("syntax error");
                }
            }
        } else if (type == "inputalphabet") {
            if (item.size() != 1 || item == " " || item == "," || item == ";" || item == "{" || item == "}" || item == "*" || item == "_") {
                throw std::runtime_error("syntax error");
            }
        } else if (type == "tapealphabet") {
            if (item.size() != 1 || item == " " || item == "," || item == ";" || item == "{" || item == "}" || item == "*") {
                throw std::runtime_error("syntax error");
            }
        }
        result.insert(item);
    }
    return result;
}

std::string TMParser::parseSingle(const std::string& line) {
    auto parts = SplitExact(line, " = ");
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

Transition TMParser::parseTransition(const std::string& line, int tapeCount, const std::set<std::string>& states, const std::set<char>& symbols) {
    std::vector<std::string> tokens;
    {
        std::stringstream ss(line);
        std::string tok;
        while (ss >> tok) {
            tokens.push_back(tok);
        }
    }
    if (tokens.size() != 5) {
        throw std::runtime_error("syntax error");
    }
    std::string oldState = tokens[0];
    std::string readSymbols = tokens[1];
    std::string writeSymbols = tokens[2];
    std::string directions = tokens[3];
    std::string newState = tokens[4];

    if (static_cast<int>(readSymbols.size()) != tapeCount ||
        static_cast<int>(writeSymbols.size()) != tapeCount ||
        static_cast<int>(directions.size()) != tapeCount) {
        throw std::runtime_error("syntax error");
    }
    if (states.find(oldState) == states.end() || states.find(newState) == states.end()) {
        throw std::runtime_error("syntax error");
    }

    std::vector<char> oldSyms;
    std::vector<char> newSyms;
    std::vector<Direction> dirs;
    for (int i = 0; i < tapeCount; ++i) {
        char r = readSymbols[static_cast<std::size_t>(i)];
        char w = writeSymbols[static_cast<std::size_t>(i)];
        char d = directions[static_cast<std::size_t>(i)];

        if (r != '*' && symbols.find(r) == symbols.end()) {
            throw std::runtime_error("syntax error");
        }
        if (w != '*' && symbols.find(w) == symbols.end()) {
            throw std::runtime_error("syntax error");
        }
        if (!(d == 'l' || d == 'r' || d == '*')) {
            throw std::runtime_error("syntax error");
        }

        oldSyms.push_back(r);
        newSyms.push_back(w);
        if (d == 'l') {
            dirs.push_back(Direction::LEFT);
        } else if (d == 'r') {
            dirs.push_back(Direction::RIGHT);
        } else {
            dirs.push_back(Direction::STAY);
        }
    }

    Transition t;
    t.oldState = oldState;
    t.oldSymbols = std::move(oldSyms);
    t.newSymbols = std::move(newSyms);
    t.directions = std::move(dirs);
    t.newState = newState;
    return t;
}
