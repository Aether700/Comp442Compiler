#include "Token.h"
#include "Core.h"
#include "Util.h"
#include "../Lexer/Lexer.h"

#include <ostream>
#include <sstream>

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

		case TokenType::IntegerLiteral:
			stream << "IntegerLiteral";
			break;

		case TokenType::FloatLiteral:
			stream << "FloatLiteral";
			break;

		case TokenType::InlineComment:
			stream << "InlineComment";
			break;

		case TokenType::MultiLineComment:
			stream << "MultiLineComment";
			break;

		//keywords
		case TokenType::Or:
			stream << "Or";
			break;

		case TokenType::And:
			stream << "And";
			break;

		case TokenType::Not:
			stream << "Not";
			break;

		case TokenType::IntegerKeyword:
			stream << "IntegerKeyword";
			break;

		case TokenType::FloatKeyword:
			stream << "FloatKeyword";
			break;

		case TokenType::Void:
			stream << "Void";
			break;

		case TokenType::Class:
			stream << "Class";
			break;

		case TokenType::Self:
			stream << "Self";
			break;

		case TokenType::IsA:
			stream << "IsA";
			break;

		case TokenType::While:
			stream << "While";
			break;

		case TokenType::If:
			stream << "If";
			break;

		case TokenType::Then:
			stream << "Then";
			break;

		case TokenType::Else:
			stream << "Else";
			break;

		case TokenType::Read:
			stream << "Read";
			break;

		case TokenType::Write:
			stream << "Write";
			break;

		case TokenType::Return:
			stream << "Return";
			break;

		case TokenType::LocalVar:
			stream << "LocalVar";
			break;

		case TokenType::Constructor:
			stream << "Constructor";
			break;

		case TokenType::Attribute:
			stream << "Attribute";
			break;

		case TokenType::Function:
			stream << "Function";
			break;

		case TokenType::Public:
			stream << "Public";
			break;

		case TokenType::Private:
			stream << "Private";
			break;


		// Operators and punctuations
		case TokenType::Equal:
			stream << "Equal";
			break;

		case TokenType::NotEqual:
			stream << "NotEqual";
			break;

		case TokenType::LessThan:
			stream << "LessThan";
			break;

		case TokenType::GreaterThan:
			stream << "GreaterThan";
			break;

		case TokenType::LessOrEqual:
			stream << "LessOrEqual";
			break;

		case TokenType::GreaterOrEqual:
			stream << "GreaterOrEqual";
			break;

		case TokenType::Plus:
			stream << "Plus";
			break;

		case TokenType::Minus:
			stream << "Minus";
			break;

		case TokenType::Multiply:
			stream << "Multiply";
			break;

		case TokenType::Divide:
			stream << "Divide";
			break;

		case TokenType::Assign:
			stream << "Assign";
			break;

		case TokenType::OpenParanthese:
			stream << "OpenParanthese";
			break;

		case TokenType::CloseParanthese:
			stream << "CloseParanthese";
			break;

		case TokenType::OpenSquareBracket:
			stream << "OpenSquareBracket";
			break;

		case TokenType::CloseSquareBracket:
			stream << "CloseSquareBracket";
			break;

		case TokenType::OpenCurlyBracket:
			stream << "OpenCurlyBracket";
			break;

		case TokenType::CloseCurlyBracket:
			stream << "CloseCurlyBracket";
			break;

		case TokenType::SemiColon:
			stream << "SemiColon";
			break;

		case TokenType::Comma:
			stream << "Comma";
			break;

		case TokenType::Dot:
			stream << "Dot";
			break;

		case TokenType::Colon:
			stream << "Colon";
			break;

		case TokenType::Arrow:
			stream << "Arrow";
			break;
	
		case TokenType::Scope:
			stream << "Scope";
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
		
		case TokenType::IncompleteMultipleLineComment:
			stream << "IncompleteMultipleLineComment";
			break;

		default:
			stream << "Unknown Token";
			DEBUG_BREAK();
			break;
	}
	return stream;
}

std::string TokenTypeToStr(TokenType t)
{
    switch(t)
	{
		case TokenType::None:
			return "None";

		case TokenType::ID:
			return "id";

		case TokenType::IntegerLiteral:
			return "integer literal";

		case TokenType::FloatLiteral:
			return "float literal";

		//keywords
		case TokenType::Or:
			return "or";

		case TokenType::And:
			return "and";

		case TokenType::Not:
			return "not";

		case TokenType::IntegerKeyword:
			return "integer";

		case TokenType::FloatKeyword:
			return "float";

		case TokenType::Void:
			return "void";

		case TokenType::Class:
			return "class";

		case TokenType::Self:

		case TokenType::IsA:
			return "isa";

		case TokenType::While:
			return "while";

		case TokenType::If:
			return "if";

		case TokenType::Then:
			return "then";

		case TokenType::Else:
			return "else";

		case TokenType::Read:
			return "read";

		case TokenType::Write:
			return "write";

		case TokenType::Return:
			return "return";

		case TokenType::LocalVar:
			return "localvar";

		case TokenType::Constructor:
			return "constructor";

		case TokenType::Attribute:
			return "attribute";

		case TokenType::Function:
			return "function";

		case TokenType::Public:
			return "public";

		case TokenType::Private:
			return "private";


		// Operators and punctuations
		case TokenType::Equal:
			return "==";

		case TokenType::NotEqual:
			return "<>";

		case TokenType::LessThan:
			return "<";

		case TokenType::GreaterThan:
			return ">";

		case TokenType::LessOrEqual:
			return "<=";

		case TokenType::GreaterOrEqual:
			return ">=";

		case TokenType::Plus:
			return "+";

		case TokenType::Minus:
			return "-";

		case TokenType::Multiply:
			return "*";

		case TokenType::Divide:
			return "/";

		case TokenType::Assign:
			return "=";

		case TokenType::OpenParanthese:
			return "(";

		case TokenType::CloseParanthese:
			return ")";

		case TokenType::OpenSquareBracket:
			return "[";

		case TokenType::CloseSquareBracket:
			return "]";

		case TokenType::OpenCurlyBracket:
			return "{";

		case TokenType::CloseCurlyBracket:
			return "}";

		case TokenType::SemiColon:
			return ";";

		case TokenType::Comma:
			return ",";

		case TokenType::Dot:
			return ".";

		case TokenType::Colon:
			return ":";

		case TokenType::Arrow:
			return "=>";
	
		case TokenType::Scope:
			return "::";

		case TokenType::EndOfFile:
			return "EOF";

		case TokenType::InlineComment:
		case TokenType::MultiLineComment:
		case TokenType::InvalidCharacter:
		case TokenType::InvalidNumber:
		case TokenType::InvalidIdentifier:		
		case TokenType::IncompleteMultipleLineComment:
        default:
			DEBUG_BREAK();
			return "[No String Convergion]";
	}
}


// Token ////////////////////////////////////////////////////////////////

Token::Token() : m_type(TokenType::None), m_line(0) { }

Token::Token(const std::string& lexeme, TokenType type, size_t line, size_t lineStartPos) 
	: m_lexeme(lexeme), m_type(type), m_line(line), m_lineStartPos(lineStartPos) { }

const std::string& Token::GetLexeme() const { return m_lexeme; }
TokenType Token::GetTokenType() const { return m_type; }
size_t Token::GetLine() const { return m_line; }

std::string Token::GetStrOfLine() const
{
	Lexer& l = Lexer::GetInstance();
	size_t prevPos = l.m_inputFile.tellg();
	l.m_inputFile.seekg(m_lineStartPos);

	std::stringstream ss;
	char c;
	l.m_inputFile.read(&c, 1);
	while (c != '\n' && !l.m_inputFile.eof())
	{
		ss << c;
		l.m_inputFile.read(&c, 1);
	}

	if (l.m_inputFile.eof())
	{
		l.m_inputFile.clear();
	}

	l.m_inputFile.seekg(prevPos);
	return TrimStr(ss.str());
}

bool Token::IsError() const
{
	return m_type == TokenType::InvalidCharacter || m_type == TokenType::InvalidNumber 
		|| m_type == TokenType::InvalidIdentifier 
        || m_type == TokenType::IncompleteMultipleLineComment;	
}