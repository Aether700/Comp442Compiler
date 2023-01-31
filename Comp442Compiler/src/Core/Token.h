#pragma once
#include <iostream>
#include <string>

enum class TokenType
{
	None,
	
	ID,
	IntegerLiteral,
	FloatLiteral,
	WhiteSpace,
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

std::ostream& operator<<(std::ostream& stream, TokenType token);

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
