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
	size_t currCharTypeCount = 1; // start at 1 to skip the CharacterType::Unknown
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
		return NullState;
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
		CharacterType charType = GetCharacterType(lookup);
		while (charType == CharacterType::Unknown)
		{
			file.read(&lookup, 1);
			charType = GetCharacterType(lookup);
		}
		charBuffer << lookup;
		StateID nextState = l.m_lexicalTable[currState]->GetTransition(charType);
		LexicalTableEntry* entry = l.m_lexicalTable[nextState];
		if (entry->IsFinal())
		{
			t = Token(charBuffer.str(), entry->GetTokenType());
			if (entry->IsBackTrack())
			{
				size_t currPos = file.tellg();
				file.seekg(currPos - 1);
			}
		}

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

void Lexer::InitializeLexicalTable()
{
	m_lexicalTable[0] = new LexicalTableEntry({1, -1}, false, false);
	m_lexicalTable[1] = new LexicalTableEntry({2,  3}, true,  false, TokenType::ID);
	m_lexicalTable[2] = new LexicalTableEntry({2,  3}, true,  false, TokenType::ID);
	m_lexicalTable[3] = new LexicalTableEntry({2,  3}, true,  false, TokenType::ID);
}

Lexer& Lexer::GetInstance()
{
	static Lexer l;
	return l;
}
