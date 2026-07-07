#pragma once

#include <string>
#include <vector>

struct Command
{
    std::string name;
    std::vector<std::string> args;
    bool background;
    std::string input_file;
    std::string output_file;
    bool append_output;

    Command() : background(false) , append_output(false) {}
};
