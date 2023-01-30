#include <iostream>
#include <sstream>
#include <filesystem>

#include "Lexer/Lexer.h"
#include "Core/Core.h"

//check to add invalid character token error

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
	return ss.str();
}

size_t WriteTokenToFile(std::ofstream& tokenFile, std::ofstream& errorFile, Token& t, size_t lastLine)
{
	if (t.IsError())
	{
		errorFile << ErrorTokenToStr(t);
	}

	for (; lastLine < t.GetLine(); lastLine++)
	{
		tokenFile << '\n';
	}
	tokenFile << TokenToStr(t);
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

void ExitPrompt()
{
	std::cout << "\nPress enter to exit\n";
	std::cin.get();
}

int main(int argc, char* argv[])
{
#ifdef DEBUG_MODE
	std::string file = "testInputFile.txt";
	std::string currDir = std::string(argv[0]);
	std::string path = currDir.substr(0, currDir.find_last_of("/\\"));
	path = path.substr(0, path.find_last_of("/\\"));
	path += "/Comp442Compiler/Comp442Compiler/" + file;
	if (false)
	{
		std::ifstream f = std::ifstream(path);

		if (!f)
		{
			return 1;
		}

		while (!f.eof())
		{
			char c;
			f.read(&c, 1);
			std::cout << c;
		}
	}

	Driver(path);
#else
	std::string directoryPath = "TestFiles";
	if (!std::filesystem::exists(directoryPath))
	{
		std::cout << "Directory \"" << directoryPath 
			<< "\" could not be found. Please put all the test files in the relative directory \"" 
			<< directoryPath << "\"\n";

		ExitPrompt();
		return 1;
	}

	for (auto& file : std::filesystem::directory_iterator(directoryPath))
	{
		std::string filename = directoryPath + "/" + file.path().filename().generic_string();
		std::string fileExtention = filename.substr(filename.length() - 4);
		if (fileExtention == ".src")
		{
			std::cout << "Processing file \"" << filename << "\"\n"; 
			Driver(filename);
		}
	}
	std::cout << "Directory processing completed\n";
	ExitPrompt();
#endif
}