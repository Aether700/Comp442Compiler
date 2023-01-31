#include "Parser.h"
#include "../Core/Token.h"
#include "../Lexer/Lexer.h"

// Parser ////////////////////////////////////////////
void Parser::Parse(const std::string& filepath)
{
    Lexer::SetInputFile(filepath);

    // Begin parsing here
}

Parser::Parser() { }

Parser& Parser::GetInstance()
{
    static Parser p;
    return p;
}