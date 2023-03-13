#include "Lexer/Lexer.h"
#include "Core/Core.h"
#include "Parser/Parser.h"
#include "Core/Util.h"
#include "SemanticChecking/SemanticErrors.h"

#include <iostream>
#include <sstream>
#include <filesystem>

/*
To Do
- dot should only be used on a class type (ex: class.something) also check that said 
	class has member/func (finished logic but need to create and add errors)

- check circular dependancy in memvar + parent memvar too
- type check parameters passed to function calls (still need to do constructors and memfunc)
- need checks for main (should be exactly 1 main func)
- issue warnings if the main has parameters
- check return type vs function return type
- need to check array types properly
*/

void ExitPrompt()
{
	std::cout << "\nPress enter to exit\n";
	std::cin.get();
}

void Compile(const std::string& filepath)
{
	ProgramNode* program = Parser::Parse(filepath);
	if (program == nullptr)
	{
		std::cout << "Parsing error was found in file \"" << filepath 
			<< "\", check logs for detail\n";
		return;
	}

	SemanticErrorManager::Clear();
	SemanticErrorManager::SetFilePath(SimplifyFilename(filepath) + ".outsemanticerrors");

	// semantic checking
	SymbolTableAssembler* assembler = new SymbolTableAssembler();
	program->AcceptVisit(assembler);

	if (SemanticErrorManager::HasError())
	{
		SemanticErrorManager::LogData();
		return;
	}

	{
		std::ofstream symbolTableFile = std::ofstream(SimplifyFilename(filepath) 
			+ ".outsymboltables");
		symbolTableFile << SymbolTableDisplayManager::TableToStr(assembler->
			GetGlobalSymbolTable());
	}

	SemanticChecker* checker = new SemanticChecker(assembler->GetGlobalSymbolTable());
	program->AcceptVisit(checker);

	SemanticErrorManager::LogData();

	delete checker;
	delete assembler;
	delete program;
}

int main(int argc, char* argv[])
{
#ifdef DEBUG_MODE
	std::string file = "testInputFile.txt";
	std::string currDir = std::string(argv[0]);
	std::string path = currDir.substr(0, currDir.find_last_of("/\\"));
	path = path.substr(0, path.find_last_of("/\\"));
	path += "/Comp442Compiler/Comp442Compiler/" + file;
	
	Compile(path);
#else
	std::string directoryPath = "TestFiles";
	
	if (!std::filesystem::exists(directoryPath))
	{
		std::cout << "Directory \"" << directoryPath 
			<< "\" could not be found. Please put all the test files in "
			<< "the relative directory \"" << directoryPath << "\"\n";
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
			Compile(filename);
		}
	}
	std::cout << "Directory processing completed\n";
	ExitPrompt();
#endif
}