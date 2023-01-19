#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <utility>

typedef size_t StateID;
static constexpr StateID NullState = SIZE_MAX;

enum class TokenType
{
	None,
	
	ID,
	Integer,
	Float,
	WhiteSpace,
	InlineComment,
	MultiLineComment,
	Keyword,
	Operator,
	Punctuation,
	EndOfFile,

	InvalidCharacter,
	InvalidNumber,
	InvalidIdentifier
};

std::ostream& operator<<(std::ostream& stream, TokenType token);

class CharData
{
public:
	// returns the char associated with the representation of the transitions based on the char provided
	static char GetRepresentationChar(char c);
	static char GetPossibleChar(size_t index);
	static size_t GetNumberOfPossibleChar();
	
	static bool IsLetter(char c);
	static bool IsNonzero(char c);
	static bool IsWhitespace(char c);
	static bool IsKeyword(const std::string& str);

	// assumes the provided character is a representation char therefore 
	// chars like 'x' which is a valid input character would return false
	static bool IsPossibleChar(char c);

	// takes a lookup character and verifies that the character is valid in the language
	static bool IsValidChar(char c);

	static char GetLetterChar();
	static char GetNonzeroChar();
	static char GetWhitespaceChar();
	static char GetEOFChar();
	static char GetElseChar();
	static char GetFloatPowerChar();

private:
	static constexpr char s_letterChar = 'l';
	static constexpr char s_nonzeroChar = 'n';
	static constexpr char s_whitespaceChar = 'w';
	static constexpr char s_eofChar = '\0';
	static constexpr char s_elseChar = 'o'; // else/other char
	static constexpr char s_floatPowerChar = 'e';
	static constexpr char s_possibleCharInput[] = 
	{
		s_letterChar, s_nonzeroChar, s_whitespaceChar, s_eofChar, s_elseChar, s_floatPowerChar, '+', '-', '/', '*', 
		'=', '0', '{', '}', '[', ']', '(', ')', ';', ':', '.', '<', '>', ',', '_', '\n'
	};
	static constexpr size_t s_numCharInput = sizeof(s_possibleCharInput) / sizeof(char);
	static constexpr const char* s_keywords[] = 
	{
		"or", "and", "not", "integer", "float", "void", "class", "self", "isa", "while", 
		"if", "then", "else", "read", "write", "return", "localvar", "constructor", "attribute", 
		"function", "public", "private"
	};
	static constexpr size_t s_numKeywords = sizeof(s_keywords) / sizeof(const char*);

};

class Token
{
	friend class Lexer;
public:
	Token();
	Token(const std::string& lexeme, TokenType type, size_t line);

	const std::string& GetLexeme() const;
	TokenType GetTokenType() const;
	size_t GetLine() const;

	bool IsError() const;

private:
	TokenType m_type;
	std::string m_lexeme;
	size_t m_line;
};

class LexicalTableEntry
{
public:
	LexicalTableEntry(const std::initializer_list<std::pair<char, StateID>>& stateTransitions, bool isFinal = false, 
		bool isBackTrack = false, TokenType type = TokenType::None);

	// assumes that the provided character is a representation character
	StateID GetTransition(char c) const;
	bool IsFinal() const;
	bool IsBackTrack() const;
	TokenType GetTokenType() const;

private:
	bool m_final;
	bool m_backTrack;
	TokenType m_tokenType;
	std::unordered_map<char, StateID> m_charToStateTransition;
};

class Lexer
{
public:
	static void SetInputFile(const std::string& filepath);
	static Token GetNextToken();

private:
	Lexer();
	~Lexer();

	// returns the next state
	static StateID TryToGenerateToken(StateID currState, char lookup, 
		std::stringstream& charBuffer, Token& outToken);

	static void BackTrack(char lookup, std::stringstream& charBuffer);

	static Token GenerateErrorToken(StateID currState, char lookup, std::stringstream& charBuffer);

	// returns the next state
	static StateID DoCustomStateChange(StateID currState, StateID nextState, std::stringstream& charBuffer, 
		char lookup, char representationChar);
	static void DoCustomStateBehavior(StateID state, Token& outToken);
	static bool IsTwoCharOperator(char firstChar, char secondChar);
	
	void InitializeLexicalTable();
	
	static Lexer& GetInstance();

	size_t m_lineCounter;
	std::unordered_map<StateID, LexicalTableEntry*> m_lexicalTable;
	std::ifstream m_inputFile;
};