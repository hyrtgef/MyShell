#pragma once
#include <string>
#include <vector>

class Builtines
{
public:
    bool isBuiltin(const std::string& name);
    void execute(const std::string& name, const std::vector<std::string>& args);

    void cd(const std::vector<std::string>& args);
    void pwd();
    void exit();
    void showHistory(const std::vector<std::string>& history);
    void help();
    void echo(const std::vector<std::string>& args);
};

