#include <iostream>
#include <sstream>
#include <filesystem>

#include "Lexer/Lexer.h"
#include "Core/Core.h"
#include "Parser/Parser.h"
#include "Core/Util.h"

To Do:
- create test for the free functions only
- add more error messages for error cases

void ExitPrompt()
{
	std::cout << "\nPress enter to exit\n";
	std::cin.get();
}

void DebugPrintFile(const std::string& filepath, size_t offset = 0)
{
	std::ifstream f = std::ifstream(filepath, std::ios_base::in);
	f.seekg(offset);
	while (!f.eof())
	{
		char c;
		f.read(&c, 1);
		std::cout << c;
	}
}

int main(int argc, char* argv[])
{
	/*
	line printing for the error doesn't work properly check why
	with current input seems like lexer is storing one position too far and 
	needs and skips first character, fix then check for other errors in 
	the TempTestInputFileStorage.txt
	*/
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

	//Driver(path);
	std::cout << Parser::Parse(path) << "\n";
	//DebugPrintFile(path, 193);
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
			Parser::Parse(filename);
		}
	}
	std::cout << "Directory processing completed\n";
	ExitPrompt();
#endif
}