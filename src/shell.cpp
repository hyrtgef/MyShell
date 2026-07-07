#include "shell.hpp"
#include "command.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <fcntl.h>

Shell::Shell() : running(true) {}

void Shell::run()
{
    while (running)
    {
        printPromt();
        std::string command_line = readCommand();

        if (!command_line.empty())
        {
            history.push_back(command_line);
            executeCommand(command_line);
        }
    }
    std::cout << "Выход из MyShell" << "\n";
}

void Shell::printPromt()
{
    std::cout << "myShell> ";
}

std::string Shell::readCommand()
{
    std::string command_line;
    std::getline(std::cin, command_line);
    return command_line;
}

void Shell::executeCommand(const std::string &line)
{
    std::vector<Command> commands = parser.parsePipeLine(line);

    if (commands.empty())
        return;

    if (commands.size() == 1)
    {
        executeSingleCommand(commands[0]);
    }
    else
    {
        executePipeline(commands);
    }
}

void Shell::executeSingleCommand(const Command &cmd)
{
    if (cmd.name.empty())
        return;

    if (cmd.name == "history")
    {
        builtins.showHistory(history);
        return;
    }

    if (builtins.isBuiltin(cmd.name))
    {
        int saved_stdout = dup(STDOUT_FILENO);
        int saved_stdin = dup(STDIN_FILENO);

        if (!cmd.output_file.empty())
        {
            int flags = O_WRONLY | O_CREAT;
            if (cmd.append_output)
                flags |= O_APPEND;
            else
                flags |= O_TRUNC;

            int fd = open(cmd.output_file.c_str(), flags, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        builtins.execute(cmd.name, cmd.args);

        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
        if (!cmd.output_file.empty())
        {
            close(saved_stdout);
        }

        return;
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        if (!cmd.output_file.empty())
        {
            int flags = O_WRONLY | O_CREAT;
            if (cmd.append_output)
                flags |= O_APPEND;
            else
                flags |= O_TRUNC;

            int fd = open(cmd.output_file.c_str(), flags, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        if (!cmd.input_file.empty())
        {
            int fd = open(cmd.input_file.c_str(), O_RDONLY);
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        std::vector<char *> args;
        args.reserve(cmd.args.size() + 2);
        args.push_back(const_cast<char *>(cmd.name.c_str()));
        for (auto &arg : cmd.args)
        {
            args.push_back(const_cast<char *>(arg.c_str()));
        }
        args.push_back(nullptr);

        execvp(args[0], args.data());
        std::cerr << "Ошибка: команда не найдена" << "\n";
        exit(1);
    }
    else if (pid > 0)
    {
        if (cmd.background)
        {
            // ✅ Фоновый режим — НЕ ждём!
            std::cout << "[PID: " << pid << "] Запущен в фоне" << std::endl;
        }
        else
        {
            int status;
            waitpid(pid, &status, 0);
        }
    }
    else
    {
        std::cerr << "Ошибка fork()" << "\n";
    }
}

void Shell::executePipeline(const std::vector<Command> &commands)
{
    int num_commands = commands.size();
    int pipefds[2];
    int prev_pipe_read = -1;

    for (size_t i = 0; i < num_commands; ++i)
    {
        if (i < num_commands - 1)
        {
            pipe(pipefds);
        }

        pid_t pid = fork();
        if (pid == 0)
        {
            if (i > 0)
            {
                dup2(prev_pipe_read, STDIN_FILENO);
                close(prev_pipe_read);
            }

            if (i < num_commands - 1)
            {
                dup2(pipefds[1], STDOUT_FILENO);
                close(pipefds[0]);
                close(pipefds[1]);
            }

            if (i == num_commands - 1)
            {
                if (!commands[i].output_file.empty())
                {
                    int flags = O_WRONLY | O_CREAT;
                    if (commands[i].append_output)
                        flags |= O_APPEND;
                    else
                        flags |= O_TRUNC;

                    int fd = open(commands[i].output_file.c_str(), flags, 0644);
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }
            }

            if (i == 0 && !commands[i].input_file.empty())
            {
                int fd = open(commands[i].input_file.c_str(), O_RDONLY);
                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            std::vector<char *> args;
            args.reserve(commands[i].args.size() + 2);
            args.push_back(const_cast<char *>(commands[i].name.c_str()));
            for (auto &arg : commands[i].args)
            {
                args.push_back(const_cast<char *>(arg.c_str()));
            }
            args.push_back(nullptr);

            execvp(args[0], args.data());
            std::cerr << "Ошибка: команда не найдена" << std::endl;
            exit(1);
        }
        else if (pid > 0)
        {
            if (i > 0)
            {
                close(prev_pipe_read);
            }
            if (i < num_commands - 1)
            {
                close(pipefds[1]);
            }
            if (i < num_commands - 1)
            {
                prev_pipe_read = pipefds[0];
            }
        }
        else
        {
            std::cerr << "Ошибка Fork()" << std::endl;
            exit(1);
        }
    }

    for (int i = 0; i < num_commands; ++i)
    {
        int status;
        wait(&status);
    }
}