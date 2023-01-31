#pragma once
#include <string>

class Parser
{
public:
    static void Parse(const std::string& filepath);
private:
    Parser();
    static Parser& GetInstance();
};