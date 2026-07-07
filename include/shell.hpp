#pragma once

#include <string>
#include <vector>
#include "parser.hpp"
#include "builtines.hpp"

class Shell
{    
public:
    Shell();
    ~Shell() = default;

    void run();
    void printPromt();
    std::string readCommand();
    void executeCommand(const std::string& line);
    void executeSingleCommand(const Command& cmd);
    void executePipeline(const std::vector<Command>& commands);

    private:
    bool running;
    std::vector<std::string> history;
    Parser parser;
    Builtines builtins;
};

