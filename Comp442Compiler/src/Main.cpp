#include <iostream>
#include "Lexer/Lexer.h"

int main()
{
	std::ifstream file = std::ifstream("testInputFile.txt");
	if (!file)
	{
		std::cout << "cannot find file\n";
		return 1;
	}
	Token t = Lexer::GetNextToken(file);
	int x = 0;
}