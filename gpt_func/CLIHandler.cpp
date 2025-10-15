#include "CLIHandler.h"
#include "ErrorHandler.h"
#include "TMParser.h"
#include "InputValidator.h"
#include "MachineSimulator.h"
#include "VerboseTracer.h"
#include "ResultPrinter.h"
#include <iostream>
#include <vector>
#include <string>

int CLIHandler::Main(int argc, char* argv[]) {
    bool showHelp = (argc == 1);
    for (int i = 1; i < argc && !showHelp; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            showHelp = true;
        }
    }
    if (showHelp) {
        CLIHandler::PrintHelp();
        return 0;
    }

    bool verboseMode = false;
    std::vector<std::string> filteredArgs;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            verboseMode = true;
        } else {
            filteredArgs.push_back(arg);
        }
    }

    if (filteredArgs.size() != 2) {
        ErrorHandler::ReportUsageError();
        return 1;
    }

    std::string tmFilePath = filteredArgs[0];
    std::string inputString = filteredArgs[1];

    TuringMachine turingMachine;
    try {
        turingMachine = TMParser::Parse(tmFilePath);
    } catch (const std::exception& e) {
        ErrorHandler::Report(e.what());
        return 1;
    } catch (...) {
        ErrorHandler::Report("Unknown error");
        return 1;
    }

    bool isValid = InputValidator::Validate(inputString, turingMachine.inputAlphabet);
    if (!isValid) {
        if (verboseMode) {
            ErrorHandler::ReportVerboseIllegalInput(inputString, turingMachine.inputAlphabet);
        } else {
            ErrorHandler::ReportIllegalInput();
        }
        return 1;
    }

    if (verboseMode) {
        VerboseTracer::SimulateAndTrace(turingMachine, inputString);
    } else {
        MachineConfiguration finalConfig = MachineSimulator::Simulate(turingMachine, inputString);
        ResultPrinter::PrintFinalResult(finalConfig);
    }

    return 0;
}
void CLIHandler::PrintHelp() {
    std::cout << "usage: turing [-v|--verbose] [-h|--help] <tm> <input>" << std::endl;
} 