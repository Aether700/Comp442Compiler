#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <utility>

#include "../Core/Token.h"

typedef size_t StateID;
static constexpr StateID NullState = SIZE_MAX;

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
	static bool IsValidRepresentationChar(char c);

	// takes a lookup character and verifies that the character is valid in the language
	static bool IsValidChar(char c);

	// returns the TokenType of the provided string if the string is a keyword or TokenType::None if it is not
	static TokenType GetKeywordType(const std::string& str);

	// returns the TokenType of the provided string if the string is an operator of a punctuation 
	// or TokenType::None if it is not
	static TokenType GetOperatorPunctuationType(const std::string& str);

	static char GetLetterChar();
	static char GetNonzeroChar();
	static char GetWhitespaceChar();
	static char GetEOFChar();
	static char GetElseChar();
	static char GetFloatPowerChar();
	static char GetNewLineChar();

private:
	static constexpr char s_letterChar = 'l';
	static constexpr char s_nonzeroChar = 'n';
	static constexpr char s_whitespaceChar = 'w';
	static constexpr char s_eofChar = '\0';
	static constexpr char s_elseChar = 'o'; // else/other char
	static constexpr char s_floatPowerChar = 'e';
	static constexpr char s_newlineChar = '\n';
	static constexpr char s_possibleCharInput[] = 
	{
		s_letterChar, s_nonzeroChar, s_whitespaceChar, s_eofChar, s_elseChar, s_floatPowerChar, '+', '-', '/', '*', 
		'=', '0', '{', '}', '[', ']', '(', ')', ';', ':', '.', '<', '>', ',', '_', s_newlineChar
	};
	static constexpr size_t s_numCharInput = sizeof(s_possibleCharInput) / sizeof(char);
	static constexpr const char* s_keywords[] = 
	{
		"or", "and", "not", "integer", "float", "void", "class", "self", "isa", "while", 
		"if", "then", "else", "read", "write", "return", "localvar", "constructor", "attribute", 
		"function", "public", "private"
	};
	static constexpr size_t s_keywordToTokenOffset = 6;
	static constexpr size_t s_numKeywords = sizeof(s_keywords) / sizeof(const char*);

};

class LexicalTableEntry
{
public:
	LexicalTableEntry(const std::initializer_list<std::pair<char, StateID>>& stateTransitions, 
		bool isFinal = false, bool isBackTrack = false, TokenType type = TokenType::None);

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
	friend class Token;
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
	static void HandleMultilineCommentLogic(StateID currState, char lookup);

	// returns the next state
	static StateID DoCustomStateChange(StateID currState, StateID nextState, std::stringstream& charBuffer, 
		char lookup, char representationChar);
	static void DoCustomStateBehavior(StateID state, Token& outToken);
	static bool IsTwoCharOperator(char firstChar, char secondChar);
	static bool IsInBlockComment();


	//returns the last char added fully processed by the lexer
	static char GetLastChar();
	
	void InitializeLexicalTable();
	
	static Lexer& GetInstance();

	size_t m_lineCounter;
	size_t m_startOfLastLinePos;
	size_t m_prevStartOfLastLinePos;
	std::unordered_map<StateID, LexicalTableEntry*> m_lexicalTable;
	std::ifstream m_inputFile;
	size_t m_multiLineCommentsOpened;
	size_t m_startLineOfMultiLineComment;
	char m_lastChar;
	bool m_justOpenedOrClosedMultiLineComment;
};