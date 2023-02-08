#include "Token.h"
#include "Core.h"

#include <ostream>

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
		|| m_type == TokenType::InvalidIdentifier 
        || m_type == TokenType::IncompleteMultipleLineComment;	
}