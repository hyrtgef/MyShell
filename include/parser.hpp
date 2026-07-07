#pragma once

#include "command.hpp"
#include <string>
#include <vector>   

class Parser
{
    private:
    std::vector<std::string> tokenize(const std::string& line);
    void handleRedirection(Command& cmd, std::vector<std::string>& tokens);
public:
    Command parse(const std::string& line);
    std::vector<Command> parsePipeLine(const std::string& line);
};

