#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>


typedef size_t StateID;
static constexpr StateID NullState = SIZE_MAX;

enum class TokenType
{
	None,
	ID,
	IntLiteral
};

enum class CharacterType
{
	Unknown,
	Letter, 
	Digit,
	Count
};

class Token
{
public:
	Token();
	Token(const std::string& lexeme, TokenType type);

	const std::string& GetLexeme() const;
	TokenType GetTokenType() const;

private:
	TokenType m_type;
	std::string m_lexeme;
};

class LexicalTableEntry
{
public:
	LexicalTableEntry(const std::initializer_list<int>& stateTransitions, bool isFinal, 
		bool isBackTrack, TokenType type = TokenType::None);

	StateID GetTransition(CharacterType charType) const;
	bool IsFinal() const;
	bool IsBackTrack() const;
	TokenType GetTokenType() const;

private:
	bool m_final;
	bool m_backTrack;
	TokenType m_tokenType;
	std::unordered_map<CharacterType, StateID> m_charToStateTransition;
};

class Lexer
{
public:
	static Token GetNextToken(std::ifstream& file);

	static CharacterType GetCharacterType(char c);

	static bool IsLetter(char c);
	static bool IsDigit(char c);

private:
	Lexer();
	~Lexer();

	void InitializeLexicalTable();

	static Lexer& GetInstance();

	std::unordered_map<StateID, LexicalTableEntry*> m_lexicalTable;
};