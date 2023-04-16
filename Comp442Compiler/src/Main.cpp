#include "Lexer/Lexer.h"
#include "Core/Core.h"
#include "Parser/Parser.h"
#include "Core/Util.h"
#include "Core/MessagePrinter.h"
#include "SemanticChecking/SemanticErrors.h"
#include "CodeGeneration/CodeGeneration.h"

#include <iostream>
#include <sstream>
#include <filesystem>

void ExitPrompt()
{
	std::cout << "\nPress enter to exit\n";
	std::cin.get();
}

void PrintMessages(const std::string& filepath)
{
	std::cout << "Output from " << filepath << ":\n\n";
	MessagePrinter::PrintToConsole();
	MessagePrinter::ClearMessages();
}

void DisplaySymbolTable(const std::string& filepath, ProgramNode* prog)
{
	std::ofstream symbolTableFile = std::ofstream(SimplifyFilename(filepath)
		+ ".outsymboltables");
	symbolTableFile << SymbolTableDisplayManager::TableToStr(prog->GetSymbolTable());
}

void GenerateBatFile(const std::string& filepath)
{
	std::cout << "generating bat file for \"" << filepath << "\"\n";
	std::string simplifiedFilepath = SimplifyFilename(filepath);
	std::string filename = simplifiedFilepath.substr(simplifiedFilepath.find_last_of("/\\") + 1);
	std::ofstream batFile = std::ofstream(simplifiedFilepath + ".bat");

	batFile << "Moon.exe +x " << filename << ".moon lib.m\n";
	batFile << "pause";
	batFile.close();
}

void Compile(const std::string& filepath)
{
	ProgramNode* program = Parser::Parse(filepath);
	if (program == nullptr)
	{
		PrintMessages(filepath);
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
		PrintMessages(filepath);
		return;
	}

	SemanticChecker* checker = new SemanticChecker(assembler->GetGlobalSymbolTable());
	program->AcceptVisit(checker);

	if (SemanticErrorManager::HasError())
	{
		SemanticErrorManager::LogData();
		DisplaySymbolTable(filepath, program);
		PrintMessages(filepath);
		return;
	}

	// code generation
	SizeGenerator* sizeGen = new SizeGenerator(program->GetSymbolTable());
	program->AcceptVisit(sizeGen);

	CodeGenerator* generator = new CodeGenerator(program->GetSymbolTable(), filepath);
	program->AcceptVisit(generator);
	generator->OutputCode();

#ifdef WINDOWS_BUILD
	GenerateBatFile(filepath);
#endif

	SemanticErrorManager::LogData();
	DisplaySymbolTable(filepath, program);
	PrintMessages(filepath);

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