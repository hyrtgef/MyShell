#include "parser.hpp"
#include <sstream>
#include <iostream>

std::vector<std::string> Parser::tokenize(const std::string &line)
{
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;

    while (ss >> token)
    {
        tokens.push_back(token);
    }
    return tokens;
}

void Parser::handleRedirection(Command &cmd, std::vector<std::string> &tokens)
{
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        if (tokens[i] == ">" && i + 1 < tokens.size())
        {
            cmd.output_file = tokens[i + 1];
            cmd.append_output = false;
            tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            --i;
        }
        else if (tokens[i] == ">>" && i + 1 < tokens.size())
        {
            cmd.output_file = tokens[i + 1];
            cmd.append_output = true;
            tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            --i;
        }
        else if (tokens[i] == "<" && i + 1 < tokens.size())
        {
            cmd.input_file = tokens[i + 1];
            tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            --i;
        }
        else if (tokens[i] == "&")
        {
            cmd.background = true;
            tokens.erase(tokens.begin() + i);
            --i;
        }
    }
}

Command Parser::parse(const std::string &line)
{
    Command cmd;
    std::vector<std::string> tokens = tokenize(line);

    if (tokens.empty())
    {
        return cmd;
    }

    std::cout << "DEBUG: tokens before = ";
    for (auto &t : tokens)
        std::cout << t << " ";
    std::cout << std::endl;

    handleRedirection(cmd, tokens);

    std::cout << "DEBUG: tokens after = ";
    for (auto &t : tokens)
        std::cout << t << " ";
    std::cout << std::endl;
    std::cout << "DEBUG: cmd.output_file = " << cmd.output_file << std::endl;

    if (tokens.empty())
    {
        return cmd;
    }

    cmd.name = tokens[0];
    for (size_t i = 1; i < tokens.size(); ++i)
    {
        cmd.args.push_back(tokens[i]);
    }
    return cmd;
}

std::vector<Command> Parser::parsePipeLine(const std::string &line)
{
    std::vector<Command> commands;
    std::stringstream ss(line);
    std::string part;

    while (std::getline(ss, part, '|'))
    {
        size_t start = part.find_first_not_of(" \t");
        size_t end = part.find_last_not_of(" \t");

        if (start != std::string::npos && end != std::string::npos)
        {
            std::string trimmed = part.substr(start, end - start + 1);
            if (!trimmed.empty())
            {
                commands.push_back(parse(trimmed));
            }
        }
    }
    if (commands.empty())
    {
        commands.push_back(parse(line));
    }

    return commands;
}