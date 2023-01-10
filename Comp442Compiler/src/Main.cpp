#include <iostream>
#include "Lexer/Lexer.h"

int main(int argc, char* argv[])
{
	std::string fileName = "testInputFile.txt";
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
	}
	Token t = Lexer::GetNextToken(file);
	std::cout << "token lexeme: " << t.GetLexeme() << "\n";
}