#include "Lexer/Lexer.h"
#include "Core/Core.h"
#include "Parser/Parser.h"
#include "Core/Util.h"
#include "SemanticChecking/SemanticErrors.h"
#include "CodeGeneration/CodeGeneration.h"

#include <iostream>
#include <sstream>
#include <filesystem>


/* To Do
 - check can use arrays of objects and access specific mem var of the object (might already work)
 - object accessing mem var which is an object which accesses a mem var
 - implement parameters being passed by reference for arrays
 - support multiple inheritance
 - check can read into arrays
*/

/*
float representation idea:
1 byte mantissa, 1 byte exponent
literal = 0.mantissa * 10^exponent

to write
multiply mantissa by 10, "exponent" times (might overflow double check to be sure, maybe add additional bytes 
	of storage to be sure) then write (might struggle with dot)

to add/substract
bring larger exponent to smaller exponent (use more space temporarily) then do computation before 
bringing it back to 2 bytes

to mult/div
mult/div mantissa
add/sub exponent

to compare (rel op)
compare exponent first then depending on operator check mantissa
*/


void ExitPrompt()
{
	std::cout << "\nPress enter to exit\n";
	std::cin.get();
}

void DisplaySymbolTable(const std::string& filepath, ProgramNode* prog)
{
	std::ofstream symbolTableFile = std::ofstream(SimplifyFilename(filepath)
		+ ".outsymboltables");
	symbolTableFile << SymbolTableDisplayManager::TableToStr(prog->GetSymbolTable());
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
		DisplaySymbolTable(filepath, program);
		std::cout << "Semantic error was found in file \"" << filepath
			<< "\", check logs for detail\n";
		return;
	}

	SemanticChecker* checker = new SemanticChecker(assembler->GetGlobalSymbolTable());
	program->AcceptVisit(checker);

	if (SemanticErrorManager::HasError())
	{
		SemanticErrorManager::LogData();
		DisplaySymbolTable(filepath, program);
		std::cout << "Semantic error was found in file \"" << filepath
			<< "\", check logs for detail\n";
		return;
	}

	// code generation
	SizeGenerator* sizeGen = new SizeGenerator(program->GetSymbolTable());
	program->AcceptVisit(sizeGen);

	CodeGenerator* generator = new CodeGenerator(program->GetSymbolTable(), filepath);
	program->AcceptVisit(generator);
	generator->OutputCode();

	SemanticErrorManager::LogData();
	DisplaySymbolTable(filepath, program);

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
#ifdef VS_BUILD
	path = path.substr(0, path.find_last_of("/\\"));
	path += "/Comp442Compiler/" + file;
#else
	path += "/Comp442Compiler/Comp442Compiler/" + file;
#endif
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