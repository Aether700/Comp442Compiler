#pragma once
#include <iostream>
#include <string>

enum class TokenType
{
	None,
	
	ID,
	IntegerLiteral,
	FloatLiteral,
	InlineComment,
	MultiLineComment,

	//keywords
	Or,
	And,
	Not,
	IntegerKeyword,
	FloatKeyword,
	Void,
	Class,
	Self,
	IsA,
	While,
	If,
	Then,
	Else,
	Read,
	Write,
	Return,
	LocalVar,
	Constructor,
	Attribute,
	Function,
	Public,
	Private,

	// Operators and punctuations
	Equal,
	NotEqual,
	LessThan,
	GreaterThan,
	LessOrEqual,
	GreaterOrEqual,
	Plus,
	Minus,
	Multiply,
	Divide,
	Assign,
	OpenParanthese,
	CloseParanthese,
	OpenSquareBracket,
	CloseSquareBracket,
	OpenCurlyBracket,
	CloseCurlyBracket,
	SemiColon,
	Comma,
	Dot,
	Colon,
	Arrow,
	Scope,


	EndOfFile,

	InvalidCharacter,
	InvalidNumber,
	InvalidIdentifier,
	IncompleteMultipleLineComment
};

// sends the literal string identifier of the enum TokenType to the stream
std::ostream& operator<<(std::ostream& stream, TokenType token);

// prints the "character" version of the tokens
std::string TokenTypeToStr(TokenType t);

class Token
{
	friend class Lexer;
public:
	Token();
	Token(const std::string& lexeme, TokenType type, size_t line, size_t lineStartPos);

	const std::string& GetLexeme() const;
	TokenType GetTokenType() const;
	size_t GetLine() const;
	std::string GetStrOfLine() const;

	bool IsError() const;

private:
	TokenType m_type;
	std::string m_lexeme;
	size_t m_line;
	size_t m_lineStartPos;
};
