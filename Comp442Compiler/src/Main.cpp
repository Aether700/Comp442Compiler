#include <iostream>
#include "Lexer/Lexer.h"
#include "Core/Core.h"
#include <sstream>

/*notes
need to support nested comments
in tools section note programming language used and why use that one
*/

std::string SimplifyFilename(const std::string& filepath)
{
	size_t lastExtention = filepath.find_last_of(".");
	return filepath.substr(0, lastExtention);
}

std::string ErrorTokenToStr(const Token& t)
{
	std::stringstream ss;
	ss << "Lexical error: " << t.GetTokenType() << ": \""
		<< t.GetLexeme() << "\": line  " << t.GetLine() << ".\n";
	// write to console
	std::cout << "Lexical error: " << t.GetTokenType() << ": \""
		<< t.GetLexeme() << "\": line  " << t.GetLine() << ".\n";
	
	return ss.str();
}

std::string TokenToStr(const Token& t)
{
	std::stringstream ss;
	ss << "[" << t.GetTokenType() << ", \"";
	if (t.GetTokenType() == TokenType::EndOfFile) 
	{
		ss << "EOF";
	}
	else
	{
		ss << t.GetLexeme();
	}
	ss << "\", " << t.GetLine() << "]";

	// write to console
	std::cout << ss.str() << "\n";
	
	return ss.str();
}

size_t WriteTokenToFile(std::ofstream& tokenFile, std::ofstream& errorFile, Token& t, size_t lastLine)
{
	if (t.IsError())
	{
		errorFile << ErrorTokenToStr(t);
	}
	else
	{
		for (; lastLine < t.GetLine(); lastLine++)
		{
			tokenFile << '\n';
		}
		tokenFile << TokenToStr(t);
	}
	return t.GetLine();
}

void Driver(const std::string& filepath)
{
	std::string simplifiedName = SimplifyFilename(filepath); 
	std::string tokenFilename = simplifiedName + ".outlextokens"; 
	std::string errorFilename = simplifiedName + ".outlexerrors";
	
	std::ofstream tokenFile = std::ofstream(tokenFilename);
	std::ofstream errorFile = std::ofstream(errorFilename);

	Lexer::SetInputFile(filepath);
	size_t lastLine = 1;
	Token t = Lexer::GetNextToken();
	lastLine = WriteTokenToFile(tokenFile, errorFile, t, lastLine);
	while (t.GetTokenType() != TokenType::EndOfFile)
	{
		t = Lexer::GetNextToken();
		lastLine = WriteTokenToFile(tokenFile, errorFile, t, lastLine);
	}
}

int main(int argc, char* argv[])
{
	std::string fileName = "testInputFile.txt";
	if (argc > 1)
	{
		fileName = argv[1];
		std::cout << "provided with file: " << fileName << "\n\n";
	}

	std::ifstream file = std::ifstream(fileName);
	if (!file)
	{
		// extract path for vscode
		std::string currDir = std::string(argv[0]);
		std::string path = currDir.substr(0, currDir.find_last_of("/\\"));
		path = path.substr(0, path.find_last_of("/\\"));
		path += "/Comp442Compiler/Comp442Compiler/" + fileName;
		
		file = std::ifstream(path);
		if (!file)
		{
			std::cout << "cannot find file\n";
			return 1;
		}
		fileName = path;
	}
	Driver(fileName);
}