#pragma once
#include <string>
#include <set>

class ErrorHandler {
public:
    static void Report(const std::string& errorMessage);
    static void ReportUsageError();
    static void ReportIllegalInput();
    static void ReportVerboseIllegalInput(const std::string& input, const std::set<char>& inputAlphabet);
};
