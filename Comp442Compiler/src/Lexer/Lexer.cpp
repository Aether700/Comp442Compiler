#include "Lexer.h"
#include <sstream>

// Token ////////////////////////////////////////////////////////////////

Token::Token() : m_type(TokenType::None) { }

Token::Token(const std::string& lexeme, TokenType type) : m_lexeme(lexeme), m_type(type) { }

const std::string& Token::GetLexeme() const { return m_lexeme; }
TokenType Token::GetTokenType() const { return m_type; }

// LexicalTableEntry //////////////////////////////////////////////////////////////////////////

LexicalTableEntry::LexicalTableEntry(const std::initializer_list<int>& stateTransitions, bool isFinal, 
	bool isBackTrack, TokenType type) : m_backTrack(isBackTrack), m_final(isFinal), m_tokenType(type)
{
	size_t currCharTypeCount = 0; // the CharacterType::Unknown is used to decide what to do in an "else" case
	for (int stateID : stateTransitions)
	{
		if (currCharTypeCount >= (size_t)CharacterType::Count)
		{
			break;
		}

		if (stateID >= 0)
		{
			CharacterType charType = (CharacterType)currCharTypeCount;
			m_charToStateTransition[charType] = (StateID)stateID;
		}
		currCharTypeCount++;
	}
}

StateID LexicalTableEntry::GetTransition(CharacterType charType) const
{
	auto it = m_charToStateTransition.find(charType);
	if (it == m_charToStateTransition.end())
	{
		// check if an "else" transition has been specified
		it = m_charToStateTransition.find(CharacterType::Unknown);
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

Token Lexer::GetNextToken(std::ifstream& file)
{
	Lexer& l = GetInstance();
	StateID currState = 0;
	Token t = Token();
	std::stringstream charBuffer;
	while (t.GetTokenType() == TokenType::None)
	{
		char lookup;
		file.read(&lookup, 1);
		if (file.eof())
		{
			TryToGenerateToken(file, currState, CharacterType::EndOfFile, charBuffer, t);
			break;
		}
		CharacterType charType = GetCharacterType(lookup);
		while (charType == CharacterType::Unknown)
		{
			file.read(&lookup, 1);
			charType = GetCharacterType(lookup);
		}
		charBuffer << lookup;
		StateID nextState = TryToGenerateToken(file, currState, charType, charBuffer, t);
		
		currState = nextState;
	}
	return Token(t);
}

CharacterType Lexer::GetCharacterType(char c)
{
	if (IsLetter(c))
	{
		return CharacterType::Letter;
	}
	else if (IsDigit(c))
	{
		return CharacterType::Digit;
	}
	else if (c == '\n')
	{
		return CharacterType::NewLine;
	}
	else if (c == '\t' || c == ' ')
	{
		return CharacterType::NewLine;
	}
	return CharacterType::Unknown;
}

bool Lexer::IsLetter(char c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool Lexer::IsDigit(char c)
{
	return c >= '0' && c <= '9';
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

StateID Lexer::TryToGenerateToken(std::ifstream& file, StateID currState, CharacterType charType, 
	std::stringstream& charBuffer, Token& outToken)
{
	Lexer& l = GetInstance();
	StateID nextState = l.m_lexicalTable[currState]->GetTransition(charType);
	if (nextState == NullState)
	{
		return currState;
	}

	LexicalTableEntry* entry = l.m_lexicalTable[nextState];

	if (entry->IsFinal())
	{
		outToken = Token(charBuffer.str(), entry->GetTokenType());
		if (entry->IsBackTrack())
		{
			BackTrack(file, charBuffer);
		}
	}
	return nextState;
}

void Lexer::BackTrack(std::ifstream& file, std::stringstream& charBuffer)
{
	// bring back stream pos
	size_t currPos = file.tellg();
	file.seekg(currPos - 1);

	std::string bufferCopy = charBuffer.str();
	charBuffer.str(bufferCopy.substr(0, bufferCopy.length() - 1));
}

void Lexer::InitializeLexicalTable()
{
	/*
	Letter, 
	Digit,
	NewLine,
	Space
	*/
	m_lexicalTable[0] = new LexicalTableEntry({-1, 1, -1}, false, false);
	m_lexicalTable[1] = new LexicalTableEntry({ 2, 1,  1}, false,  false);
	m_lexicalTable[2] = new LexicalTableEntry({}, true,  true, TokenType::ID);
	m_lexicalTable[3] = new LexicalTableEntry({4, -1, -1, -1, 3}, false,  false);
	m_lexicalTable[4] = new LexicalTableEntry({}, true,  true, TokenType::Comment); // temp token type
}

Lexer& Lexer::GetInstance()
{
	static Lexer l;
	return l;
}
