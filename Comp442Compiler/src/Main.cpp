#include <iostream>
#include <sstream>
#include <filesystem>

#include "Lexer/Lexer.h"
#include "Core/Core.h"
#include "Parser/Parser.h"
#include "Core/Util.h"

class VisitorImpl : public Visitor
{
public:
	size_t numIDVisited = 0;
	void Visit(IDNode* element) override 
	{
		std::cout << "visited id \"" << element->GetID().GetLexeme() << "\"\n";
		numIDVisited++;
	}
};

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
	
	ProgramNode* program = Parser::Parse(path);
	std::cout << (program != nullptr) << "\n";

	if (program == nullptr)
	{
		return 1;
	}
	SymbolTableAssembler* assembler = new SymbolTableAssembler();
	program->AcceptVisit(assembler);
	for (SymbolTableEntry* entry : assembler->GetEntries())
	{
		std::cout << *entry << "\n";
		if (entry->GetSubTable() != nullptr)
		{
			std::cout << "\n";
			for (SymbolTableEntry* entry2 : *entry->GetSubTable())
			{
				std::cout << *entry2 << "\n";
			}

			std::cout << "\n";
		}
	}

	delete program;
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
			delete Parser::Parse(filename);
		}
	}
	std::cout << "Directory processing completed\n";
	ExitPrompt();
#endif
}