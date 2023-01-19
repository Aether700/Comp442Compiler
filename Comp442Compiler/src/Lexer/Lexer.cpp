#include "Lexer.h"
#include "../Core/Core.h"
#include <sstream>
#include <string.h>

//temp/////////////////////////
#include <iostream>
/////////////////////////

static constexpr char letterChar = 'l';
static constexpr char nonzeroChar = 'n';
static constexpr char whitespaceChar = 'w';
static constexpr char eofChar = '\0';
static constexpr char elseChar = 'o';
static constexpr char possibleCharacterInput[] = 
{
	letterChar, nonzeroChar, whitespaceChar, eofChar, elseChar,'e', '+', '-', '/', '*', 
	'=', '0', '{', '}', '[', ']', '(', ')', ';', ':', '.', '<', '>', ',', '_'
};
static constexpr size_t numCharInput = sizeof(possibleCharacterInput) / sizeof(char);

std::ostream& operator<<(std::ostream& stream, TokenType token)
{
	switch(token)
	{
		case TokenType::None:
			stream << "None";
			break;

		case TokenType::ID:
			stream << "ID";
			break;

		case TokenType::Integer:
			stream << "Integer";
			break;

		case TokenType::Float:
			stream << "Float";
			break;

		case TokenType::WhiteSpace:
			stream << "WhiteSpace";
			break;

		case TokenType::InlineComment:
			stream << "InlineComment";
			break;

		case TokenType::MultiLineComment:
			stream << "MultiLineComment";
			break;

		case TokenType::Keyword:
			stream << "Keyword";
			break;

		case TokenType::Operator:
			stream << "Operator";
			break;

		case TokenType::Punctuation:
			stream << "Punctuation";
			break;

		case TokenType::EndOfFile:
			stream << "EOF";
			break;

		case TokenType::InvalidCharacter:
			stream << "InvalidCharacter";
			break;
		
		case TokenType::InvalidNumber:
			stream << "InvalidNumber";
			break;
			
		case TokenType::InvalidIdentifier:
			stream << "InvalidIdentifier";
			break;

		default:
			stream << "Unknown Token";
			DEBUG_BREAK();
			break;
	}
	return stream;
}

// CharData ///////////////////////////////////////////////////////////////////////
char CharData::GetRepresentationChar(char c)
{
	if (c == s_floatPowerChar)
	{
		return c;
	}
	else if (IsLetter(c))
	{
		return s_letterChar;
	}
	else if (IsNonzero(c))
	{
		return s_nonzeroChar;
	}
	else if (IsWhitespace(c))
	{
		return s_whitespaceChar;
	}
	else if (IsPossibleChar(c))
	{
		return c;
	}
	return s_elseChar;
}

char CharData::GetPossibleChar(size_t index) { return s_possibleCharInput[index]; }
size_t CharData::GetNumberOfPossibleChar() { return s_numCharInput; }
bool CharData::IsLetter(char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
bool CharData::IsNonzero(char c) { return c >= '1' && c <= '9'; }
bool CharData::IsWhitespace(char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f'; }

bool CharData::IsKeyword(const std::string& str)
{
	for (size_t i = 0; i < s_numKeywords; i++)
	{
		if (strcmp(s_keywords[i], str.c_str()) == 0)
		{
			return true;
		}
	}
	return false;
}

bool CharData::IsPossibleChar(char c)
{
	for (size_t i = 0; i < s_numCharInput; i++)
	{
		if (s_possibleCharInput[i] == c)
		{
			return true;
		}
	}
	return false;
}

bool CharData::IsValidChar(char c)
{
	return IsLetter(c) || IsNonzero(c) || IsWhitespace(c) || IsPossibleChar(c);
}

char CharData::GetLetterChar() { return s_letterChar; }
char CharData::GetNonzeroChar() { return s_nonzeroChar; }
char CharData::GetWhitespaceChar() { return s_whitespaceChar; }
char CharData::GetEOFChar() { return s_eofChar; }
char CharData::GetElseChar() { return s_elseChar; }
char CharData::GetFloatPowerChar() { return s_floatPowerChar; }


// Token ////////////////////////////////////////////////////////////////

Token::Token() : m_type(TokenType::None), m_line(0) { }

Token::Token(const std::string& lexeme, TokenType type, size_t line) : m_lexeme(lexeme), 
	m_type(type), m_line(line) { }

const std::string& Token::GetLexeme() const { return m_lexeme; }
TokenType Token::GetTokenType() const { return m_type; }
size_t Token::GetLine() const { return m_line; }

bool Token::IsError() const
{
	return m_type == TokenType::InvalidCharacter || m_type == TokenType::InvalidNumber 
		|| m_type == TokenType::InvalidIdentifier;	
}

// LexicalTableEntry //////////////////////////////////////////////////////////////////////////

LexicalTableEntry::LexicalTableEntry(const std::initializer_list<std::pair<char, StateID>>& stateTransitions, bool isFinal, 
	bool isBackTrack, TokenType type) : m_backTrack(isBackTrack), m_final(isFinal), m_tokenType(type)
{
	for (auto pair : stateTransitions)
	{
		m_charToStateTransition[pair.first] = pair.second;
	}
}

StateID LexicalTableEntry::GetTransition(char c) const
{
	auto it = m_charToStateTransition.find(c);
	if (it == m_charToStateTransition.end())
	{
		// check if an "else" transition has been specified
		it = m_charToStateTransition.find(CharData::GetElseChar());
		if (it == m_charToStateTransition.end())
		{
			return NullState;
		}
	}
	return (*it).second;
}

bool LexicalTableEntry::IsFinal() const { return m_final; }
bool LexicalTableEntry::IsBackTrack() const { return m_backTrack; }
TokenType LexicalTableEntry::GetTokenType() const { return m_tokenType; }

// Lexer //////////////////////////////////////////////////////////////////

void Lexer::SetInputFile(const std::string& filepath)
{
	Lexer& l = GetInstance();
	l.m_inputFile = std::ifstream(filepath);
	l.m_lineCounter = 1;
}

Token Lexer::GetNextToken()
{
	Lexer& l = GetInstance();
	StateID currState = 0;
	Token t = Token();
	std::stringstream charBuffer;
	while (t.GetTokenType() == TokenType::None)
	{
		char lookup;
		l.m_inputFile.read(&lookup, 1);
		if (l.m_inputFile.eof())
		{
			lookup = CharData::GetEOFChar();
		}
		else if (lookup == '\n')
		{
			l.m_lineCounter++;
		}

		if (currState != 0 || !CharData::IsWhitespace(lookup))
		{
			charBuffer << lookup;
		}


		StateID nextState = TryToGenerateToken(currState, lookup, charBuffer, t);		
		currState = nextState;
	}
	return Token(t);
}


Lexer::Lexer()
{
	InitializeLexicalTable();
}

Lexer::~Lexer()
{
	for (auto& pair : m_lexicalTable)
	{
		delete pair.second;
	}
}

StateID Lexer::TryToGenerateToken(StateID currState, char lookup, 
	std::stringstream& charBuffer, Token& outToken)
{
	Lexer& l = GetInstance();
	char representationChar = CharData::GetRepresentationChar(lookup);

	StateID nextState = l.m_lexicalTable[currState]->GetTransition(representationChar);
	nextState = DoCustomStateChange(currState, nextState, charBuffer, lookup, representationChar);
	if (nextState == NullState)
	{
		std::cout << "[Debug]: Error encountered at state " << currState 
			<< " when receiving representation character " << representationChar << "\n";
		outToken = GenerateErrorToken(currState, lookup, charBuffer);
		return currState;
	}

	LexicalTableEntry* entry = l.m_lexicalTable[nextState];

	if (entry->IsFinal())
	{
		if (entry->IsBackTrack())
		{
			BackTrack(lookup, charBuffer);
		}
		outToken = Token(charBuffer.str(), entry->GetTokenType(), l.m_lineCounter);
		DoCustomStateBehavior(nextState, outToken);
	}
	return nextState;
}

void Lexer::BackTrack(char lookup, std::stringstream& charBuffer)
{
	Lexer& l = GetInstance();
	// bring back stream pos
	size_t currPos = l.m_inputFile.tellg();
	l.m_inputFile.seekg(currPos - 1);

	// update line counter if needed
	if (lookup == '\n')
	{
		l.m_lineCounter--;
	}

	// remove character from buffer
	std::string bufferCopy = charBuffer.str();
	charBuffer.str(bufferCopy.substr(0, bufferCopy.length() - 1));
}

Token Lexer::GenerateErrorToken(StateID currState, char lookup, std::stringstream& charBuffer)
{
	Lexer& l = GetInstance();
	if (!CharData::IsValidChar(lookup))
	{
		return Token(charBuffer.str(), TokenType::InvalidCharacter, l.m_lineCounter);
	}

	char currChar = lookup;
	charBuffer << currChar;
	while (!CharData::IsWhitespace(currChar))
	{
		l.m_inputFile.read(&currChar, 1);
		charBuffer << currChar;
	}

	BackTrack(currChar, charBuffer);
	

	switch(currState)
	{
	case 6:
	case 10:
	case 15:
	case 16:
		return Token(charBuffer.str(), TokenType::InvalidNumber, l.m_lineCounter);
	}

	return Token(charBuffer.str(), TokenType::InvalidIdentifier, l.m_lineCounter);
}

StateID Lexer::DoCustomStateChange(StateID currState, StateID nextState, std::stringstream& charBuffer,
	 char lookup, char representationChar)
{
	switch(currState)
	{
	case 20:
		std::string charBufferStr = charBuffer.str();
		if (charBufferStr.length() >= 2)
		{
			char lastChar = charBufferStr[charBufferStr.length() - 2];
			if (IsTwoCharOperator(lastChar, lookup))
			{
				return 21;
			}
		}
		break;
	}
	return nextState;
}

void Lexer::DoCustomStateBehavior(StateID state, Token& outToken)
{
	switch(state)
	{
	case 22:
		if (CharData::IsKeyword(outToken.GetLexeme()))
		{
			outToken.m_type = TokenType::Keyword;
		}
		break;
	}
}

bool Lexer::IsTwoCharOperator(char firstChar, char secondChar)
{
	switch(firstChar)
	{
	case '=':
		return secondChar == '=' || secondChar == '>';
	case '<':
		return secondChar == '=' || secondChar == '>';
	case '>':
		return secondChar == '=';
	case ':':
		return secondChar == ':';
	}
	return false;
}

void Lexer::InitializeLexicalTable()
{
	m_lexicalTable[0] = new LexicalTableEntry({{CharData::GetLetterChar(), 1}, 
		{CharData::GetNonzeroChar(), 2}, {CharData::GetWhitespaceChar(), 0}, {CharData::GetEOFChar(), 18}, 
		{CharData::GetElseChar(), 26}, {'+', 14}, {'-', 14}, {'/', 4}, {'*', 14}, {'=', 20}, 
		{'0', 3}, {'{', 14}, {'}', 14}, {'[', 14}, {']', 14},  {'(', 14}, {')', 14}, {';', 14}, 
		{':', 20}, {'.', 14}, {'<', 20}, {'>', 20}, {',', 14}, {'\n', 0}});

	m_lexicalTable[1] = new LexicalTableEntry({{CharData::GetLetterChar(), 1}, 
		{CharData::GetNonzeroChar(), 1}, {CharData::GetElseChar(), 22}, {'_', 1}});
	
	m_lexicalTable[2] = new LexicalTableEntry({{CharData::GetNonzeroChar(), 5}, 
		{CharData::GetElseChar(), 23}, {'0', 5}, {'.', 6}});
	
	m_lexicalTable[3] = new LexicalTableEntry({{CharData::GetNonzeroChar(), 30}, 
		{CharData::GetElseChar(), 23}, {'0', 30}, {'.', 6}});

	m_lexicalTable[4] = new LexicalTableEntry({{CharData::GetElseChar(), 28}, {'/', 7}, {'*', 8}});
	m_lexicalTable[5] = new LexicalTableEntry({{CharData::GetNonzeroChar(), 5}, 
		{CharData::GetElseChar(), 23}, {'0', 5}, {'.', 6}});
	
	m_lexicalTable[6] = new LexicalTableEntry({{CharData::GetNonzeroChar(), 9}, {'0', 24}});
	m_lexicalTable[7] = new LexicalTableEntry({{CharData::GetEOFChar(), 11}, 
		{CharData::GetElseChar(), 7}, {'\n', 11}});
	
	m_lexicalTable[8] = new LexicalTableEntry({{CharData::GetElseChar(), 8}, {'*', 12}});
	m_lexicalTable[9] = new LexicalTableEntry({{CharData::GetNonzeroChar(), 9}, 
		{CharData::GetElseChar(), 25}, {CharData::GetFloatPowerChar(), 15}, {'0', 10}});
	
	m_lexicalTable[10] = new LexicalTableEntry({{CharData::GetNonzeroChar(), 9}, {'0', 10}});
	m_lexicalTable[11] = new LexicalTableEntry({}, true, true, TokenType::InlineComment);
	m_lexicalTable[12] = new LexicalTableEntry({{CharData::GetElseChar(), 8}, {'/', 13}});
	m_lexicalTable[13] = new LexicalTableEntry({}, true, false, TokenType::MultiLineComment);
	m_lexicalTable[14] = new LexicalTableEntry({}, true, false, TokenType::Operator);
	m_lexicalTable[15] = new LexicalTableEntry({{CharData::GetNonzeroChar(), 17}, 
		{CharData::GetElseChar(), 30}, {'+', 16}, {'-', 16}, {'0', 19}});

	m_lexicalTable[16] = new LexicalTableEntry({{CharData::GetNonzeroChar(), 17}, {'0', 19}});
	m_lexicalTable[17] = new LexicalTableEntry({{CharData::GetNonzeroChar(), 17}, 
		{CharData::GetElseChar(), 25}, {'0', 17}});

	m_lexicalTable[18] = new LexicalTableEntry({}, true, false, TokenType::EndOfFile);
	m_lexicalTable[19] = new LexicalTableEntry({{CharData::GetNonzeroChar(), 30}, 
		{CharData::GetElseChar(), 25}, {'0', 30}});
	
	// needs additional steps to sort which operator the token is on this state
	m_lexicalTable[20] = new LexicalTableEntry({{CharData::GetElseChar(), 28}}); 
	
	m_lexicalTable[21] = new LexicalTableEntry({}, true, false, TokenType::Operator);

	// need to double check that id is not a keyword
	m_lexicalTable[22] = new LexicalTableEntry({}, true, true, TokenType::ID);
	m_lexicalTable[23] = new LexicalTableEntry({}, true, true, TokenType::Integer);
	m_lexicalTable[24] = new LexicalTableEntry({{CharData::GetNonzeroChar(), 9}, 
		{CharData::GetElseChar(), 25}, {CharData::GetFloatPowerChar(), 15}});
	
	m_lexicalTable[25] = new LexicalTableEntry({}, true, true, TokenType::Float);
	m_lexicalTable[26] = new LexicalTableEntry({{CharData::GetLetterChar(), 26}, 
		{CharData::GetNonzeroChar(), 26}, {CharData::GetElseChar(), 27}, {'0', 26}});
	
	m_lexicalTable[27] = new LexicalTableEntry({}, true, true, TokenType::InvalidIdentifier);
	m_lexicalTable[28] = new LexicalTableEntry({}, true, true, TokenType::Operator);
	m_lexicalTable[29] = new LexicalTableEntry({}, true, true, TokenType::InvalidNumber);
	m_lexicalTable[30] = new LexicalTableEntry({{CharData::GetLetterChar(), 30}, 
		{CharData::GetNonzeroChar(), 30}, {CharData::GetElseChar(), 29}, 
		{CharData::GetEOFChar(), 29}, {'0', 30}});
}

Lexer& Lexer::GetInstance()
{
	static Lexer l;
	return l;
}
