#include "builtines.hpp"
#include <iostream>
#include <unistd.h>
#include <cstdlib>

bool Builtines::isBuiltin(const std::string& name)
{
    return name == "cd" || name == "pwd" || name == "exit" || 
           name == "history" || name == "help" || name == "echo";
}

void Builtines::execute(const std::string& name, const std::vector<std::string>& args)
{
    if(name == "cd") cd(args);
    else if(name == "pwd") pwd();
    else if (name == "exit") exit();
    else if (name == "history") {
        
    }
    else if (name == "help") help();
    else if (name == "echo") echo(args);
}

void Builtines::cd(const std::vector<std::string>& args)
{
    if(args.empty())
    {
        std::cout << "cd: нужен путь" << "\n";
        return;
    }

    if(chdir(args[0].c_str()) != 0)
    {
        std::cerr << "cd: ошибка перехода в " << args[0] << '\n';
    }
}

void Builtines::pwd()
{
    char cwd[1024];

    if(getcwd(cwd, sizeof(cwd)))
    {
        std::cout << cwd  << "\n";
    }
}

void Builtines::exit()
{
    std::cout << "Выход из MyShell" << "\n";
    std::exit(0);
}

void Builtines::showHistory(const std::vector<std::string>& history)
{
    if(history.empty()) 
    {
        std::cout << "История пустая" << "\n";
        return;
    }
    
    std::cout << "История:" << "\n"; 
    for(size_t i = 0; i < history.size(); ++i)
    {
        std::cout << i + 1 << " " << history[i] << "\n";
    }
}

void Builtines::help()
{
    std::cout << "Встроенные команды: cd, pwd, exit, history, help, echo" << "\n"; 
}

void Builtines::echo(const std::vector<std::string>& args)
{
    for(size_t i = 0; i < args.size(); ++i)
    {
        std::cout << args[i];
        if(i + 1 < args.size()) std::cout << " ";
    }
    std::cout << "\n";
}