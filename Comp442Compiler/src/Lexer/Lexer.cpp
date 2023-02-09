#include "Lexer.h"
#include "../Core/Core.h"
#include <sstream>
#include <string.h>
#include <iostream>

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
	else if (IsValidRepresentationChar(c))
	{
		return c;
	}
	return s_elseChar;
}

char CharData::GetPossibleChar(size_t index) { return s_possibleCharInput[index]; }
size_t CharData::GetNumberOfPossibleChar() { return s_numCharInput; }
bool CharData::IsLetter(char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'd') 
	|| (c >= 'f' && c <= 'z'); }
bool CharData::IsNonzero(char c) { return c >= '1' && c <= '9'; }
bool CharData::IsWhitespace(char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\v' || c == '\f'; }

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

bool CharData::IsValidRepresentationChar(char c)
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
	return IsLetter(c) || IsNonzero(c) || IsWhitespace(c) || IsValidRepresentationChar(c);
}

TokenType CharData::GetKeywordType(const std::string& str)
{
	for (size_t i = 0; i < s_numKeywords; i++)
	{
		if (str == s_keywords[i])
		{
			return  (TokenType)(i + s_keywordToTokenOffset);
		}
	}
	return TokenType::None;
}

TokenType CharData::GetOperatorPunctuationType(const std::string& str)
{
	if (str == "==")
	{
		return TokenType::Equal;
	}
	else if (str == "<>")
	{
		return TokenType::NotEqual;
	}
	else if (str == "<")
	{
		return TokenType::LessThan;
	}
	else if (str == ">")
	{
		return TokenType::GreaterThan;
	}
	else if (str == "<=")
	{
		return TokenType::LessOrEqual;
	}
	else if (str == ">=")
	{
		return TokenType::GreaterOrEqual;
	}
	else if (str == "+")
	{
		return TokenType::Plus;
	}
	else if (str == "-")
	{
		return TokenType::Minus;
	}
	else if (str == "*")
	{
		return TokenType::Multiply;
	}
	else if (str == "/")
	{
		return TokenType::Divide;
	}
	else if (str == "=")
	{
		return TokenType::Assign;
	}
	else if (str == "(")
	{
		return TokenType::OpenParanthese;
	}
	else if (str == ")")
	{
		return TokenType::CloseParanthese;
	}
	else if (str == "[")
	{
		return TokenType::OpenSquareBracket;
	}
	else if (str == "]")
	{
		return TokenType::CloseSquareBracket;
	}
	else if (str == "{")
	{
		return TokenType::OpenCurlyBracket;
	}
	else if (str == "}")
	{
		return TokenType::CloseCurlyBracket;
	}
	else if (str == ";")
	{
		return TokenType::SemiColon;
	}
	else if (str == ",")
	{
		return TokenType::Comma;
	}
	else if (str == ".")
	{
		return TokenType::Dot;
	}
	else if (str == ":")
	{
		return TokenType::Colon;
	}
	else if (str == "=>")
	{
		return TokenType::Arrow;
	}
	else if (str == "::")
	{
		return TokenType::Scope;
	}

	return TokenType::None;
}

char CharData::GetLetterChar() { return s_letterChar; }
char CharData::GetNonzeroChar() { return s_nonzeroChar; }
char CharData::GetWhitespaceChar() { return s_whitespaceChar; }
char CharData::GetEOFChar() { return s_eofChar; }
char CharData::GetElseChar() { return s_elseChar; }
char CharData::GetFloatPowerChar() { return s_floatPowerChar; }
char CharData::GetNewLineChar() { return s_newlineChar; }

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
	l.m_multiLineCommentsOpened = 0;
	l.m_startLineOfMultiLineComment = 0;
	l.m_lastChar = '\0';
	l.m_justOpenedOrClosedMultiLineComment = false;
	l.m_startOfLastLinePos = 0;
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
			if (IsInBlockComment())
			{
				l.m_inputFile.clear();
				l.m_multiLineCommentsOpened = 0; // set to 0 now that error has been recorded
				t = Token(charBuffer.str(), TokenType::IncompleteMultipleLineComment, 
					l.m_startLineOfMultiLineComment, l.m_startOfLastLinePos);
				continue;
			}
		}
		else if (lookup == CharData::GetNewLineChar())
		{
			l.m_lineCounter++;
			l.m_startOfLastLinePos = l.m_inputFile.tellg();
		}

		if (currState != 0 || !(CharData::IsWhitespace(lookup) 
			|| lookup == CharData::GetNewLineChar()))
		{
			charBuffer << lookup;
		}


		StateID nextState = TryToGenerateToken(currState, lookup, charBuffer, t);		
		currState = nextState;
		l.m_lastChar = lookup;
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
		// last fail safe error.
		std::cout << "[Debug]: Error encountered at state " << currState 
			<< " when receiving representation character " << representationChar 
			<< " | lookup char: " << lookup << "\n";
		outToken = Token(charBuffer.str(), TokenType::InvalidIdentifier, l.m_lineCounter, 
			l.m_startOfLastLinePos);
		return currState;
	}

	// handle muliple line comment logic
	HandleMultilineCommentLogic(currState, lookup);

	LexicalTableEntry* entry = l.m_lexicalTable[nextState];

	if (entry->IsFinal())
	{
		if (IsInBlockComment())
		{
			return 8;
		}

		if (entry->IsBackTrack())
		{
			BackTrack(lookup, charBuffer);
		}
		outToken = Token(charBuffer.str(), entry->GetTokenType(), l.m_lineCounter, 
			l.m_startOfLastLinePos);
		DoCustomStateBehavior(nextState, outToken);
	}
	return nextState;
}

void Lexer::BackTrack(char lookup, std::stringstream& charBuffer)
{
	Lexer& l = GetInstance();
	// bring back stream pos
	l.m_inputFile.putback(lookup);
	std::string bufferCopy = charBuffer.str();
	l.m_lastChar = bufferCopy[bufferCopy.length() - 2];

	// update line counter if needed
	if (lookup == CharData::GetNewLineChar())
	{
		l.m_lineCounter--;
	}

	// remove character from buffer
	charBuffer.str(bufferCopy.substr(0, bufferCopy.length() - 1));
}

void Lexer::HandleMultilineCommentLogic(StateID currState, char lookup)
{
	Lexer& l = GetInstance();
	if (currState == 7) // if in an inline comment
	{
		l.m_justOpenedOrClosedMultiLineComment = false;
		return;
	}

	if (GetLastChar() == '/' && lookup == '*' && !l.m_justOpenedOrClosedMultiLineComment)
	{
		if (!IsInBlockComment())
		{
			l.m_startLineOfMultiLineComment = l.m_lineCounter;
		}
		l.m_multiLineCommentsOpened++;
		l.m_justOpenedOrClosedMultiLineComment = true;
	}
	else if (IsInBlockComment() && GetLastChar() == '*' 
		&& lookup == '/' && !l.m_justOpenedOrClosedMultiLineComment)
	{
		l.m_multiLineCommentsOpened--;
		l.m_justOpenedOrClosedMultiLineComment = true;
	}
	else
	{
		l.m_justOpenedOrClosedMultiLineComment = false;
	}
}


StateID Lexer::DoCustomStateChange(StateID currState, StateID nextState, std::stringstream& charBuffer,
	 char lookup, char representationChar)
{
	switch(currState)
	{
	case 0:
		if (!CharData::IsValidChar(lookup))
		{
			return 31;
		}
		break;

	case 20:
		if (IsTwoCharOperator(GetLastChar(), lookup))
		{
			return 21;
		}
		break;

	case 30:
		if (representationChar == CharData::GetEOFChar())
		{
			BackTrack(lookup, charBuffer);
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
			outToken.m_type = CharData::GetKeywordType(outToken.GetLexeme());
		}
		break;

	case 14:
	case 21:
	case 28:
		outToken.m_type = CharData::GetOperatorPunctuationType(outToken.GetLexeme());
		break;

	case 13:
		outToken.m_line = GetInstance().m_startLineOfMultiLineComment;
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

bool Lexer::IsInBlockComment() { return GetInstance().m_multiLineCommentsOpened > 0; }


char Lexer::GetLastChar() { return GetInstance().m_lastChar; }

void Lexer::InitializeLexicalTable()
{
	m_lexicalTable[0] = new LexicalTableEntry({{CharData::GetLetterChar(), 1}, 
		{CharData::GetNonzeroChar(), 2}, {CharData::GetWhitespaceChar(), 0}, {CharData::GetEOFChar(), 18}, 
		{CharData::GetElseChar(), 26}, {'e', 1}, {'+', 14}, {'-', 14}, {'/', 4}, {'*', 14}, {'=', 20}, 
		{'0', 3}, {'{', 14}, {'}', 14}, {'[', 14}, {']', 14},  {'(', 14}, {')', 14}, {';', 14}, 
		{':', 20}, {'.', 14}, {'<', 20}, {'>', 20}, {',', 14}, {CharData::GetNewLineChar(), 0}});

	m_lexicalTable[1] = new LexicalTableEntry({{CharData::GetLetterChar(), 1}, 
		{CharData::GetNonzeroChar(), 1}, {CharData::GetElseChar(), 22}, {CharData::GetFloatPowerChar(), 1}, 
		{'0', 1}, {'_', 1},});
	
	m_lexicalTable[2] = new LexicalTableEntry({{CharData::GetLetterChar(), 30}, 
		{CharData::GetNonzeroChar(), 5}, {CharData::GetElseChar(), 23}, 
		{CharData::GetFloatPowerChar(), 30}, {'0', 5}, {'.', 6}, {'_', 30}});
	
	m_lexicalTable[3] = new LexicalTableEntry({{CharData::GetLetterChar(), 30}, 
		{CharData::GetNonzeroChar(), 30}, {CharData::GetElseChar(), 23}, 
		{CharData::GetFloatPowerChar(), 30}, {'0', 30}, {'.', 6}, {'_', 30}});

	m_lexicalTable[4] = new LexicalTableEntry({{CharData::GetElseChar(), 28}, {'/', 7}, {'*', 8}});
	m_lexicalTable[5] = new LexicalTableEntry({{CharData::GetLetterChar(), 30}, 
		{CharData::GetNonzeroChar(), 5}, {CharData::GetElseChar(), 23}, 
		{CharData::GetFloatPowerChar(), 30}, {'0', 5}, {'.', 6}, {'_', 30}});
	
	m_lexicalTable[6] = new LexicalTableEntry({{CharData::GetNonzeroChar(), 9}, 
		{CharData::GetElseChar(), 30}, {'0', 24}});

	m_lexicalTable[7] = new LexicalTableEntry({{CharData::GetEOFChar(), 11}, 
		{CharData::GetElseChar(), 7}, {CharData::GetNewLineChar(), 11}});
	
	m_lexicalTable[8] = new LexicalTableEntry({{CharData::GetElseChar(), 8}, {'*', 12}});
	m_lexicalTable[9] = new LexicalTableEntry({{CharData::GetLetterChar(), 30}, 
		{CharData::GetNonzeroChar(), 9}, {CharData::GetElseChar(), 25}, 
		{CharData::GetFloatPowerChar(), 15}, {'0', 10}, {'.', 30}, {'_', 30}});
	
	m_lexicalTable[10] = new LexicalTableEntry({{CharData::GetNonzeroChar(), 9}, 
		{CharData::GetElseChar(), 30}, {'0', 10}});

	m_lexicalTable[11] = new LexicalTableEntry({}, true, true, TokenType::InlineComment);
	m_lexicalTable[12] = new LexicalTableEntry({{CharData::GetElseChar(), 8}, {'/', 13}});
	m_lexicalTable[13] = new LexicalTableEntry({}, true, false, TokenType::MultiLineComment);
	m_lexicalTable[14] = new LexicalTableEntry({}, true, false, TokenType::None);
	m_lexicalTable[15] = new LexicalTableEntry({{CharData::GetNonzeroChar(), 17}, 
		{CharData::GetElseChar(), 30}, {CharData::GetFloatPowerChar(), 30},
		{'+', 16}, {'-', 16}, {'0', 19}});

	m_lexicalTable[16] = new LexicalTableEntry({{CharData::GetNonzeroChar(), 17}, 
		{CharData::GetElseChar(), 30}, {'0', 19}});
	m_lexicalTable[17] = new LexicalTableEntry({{CharData::GetLetterChar(), 30},
		{CharData::GetNonzeroChar(), 17}, {CharData::GetElseChar(), 25}, 
		{CharData::GetFloatPowerChar(), 30}, {'0', 17}, {'_', 30}});

	m_lexicalTable[18] = new LexicalTableEntry({}, true, false, TokenType::EndOfFile);
	m_lexicalTable[19] = new LexicalTableEntry({{CharData::GetLetterChar(), 30}, 
		{CharData::GetNonzeroChar(), 30}, {CharData::GetElseChar(), 25}, 
		{CharData::GetFloatPowerChar(), 30}, {'0', 30}, {'_', 30}});
	
	// needs additional steps to sort which operator the token is on this state
	m_lexicalTable[20] = new LexicalTableEntry({{CharData::GetElseChar(), 28}}); 
	
	m_lexicalTable[21] = new LexicalTableEntry({}, true, false, TokenType::None);

	// need to double check that id is not a keyword
	m_lexicalTable[22] = new LexicalTableEntry({}, true, true, TokenType::ID);
	m_lexicalTable[23] = new LexicalTableEntry({}, true, true, TokenType::IntegerLiteral);
	m_lexicalTable[24] = new LexicalTableEntry({{CharData::GetLetterChar(), 30},
		{CharData::GetNonzeroChar(), 9}, {CharData::GetElseChar(), 25}, 
		{CharData::GetFloatPowerChar(), 15}, {'0', 10}, {'.', 30}, {'_', 30}});
	
	m_lexicalTable[25] = new LexicalTableEntry({}, true, true, TokenType::FloatLiteral);
	m_lexicalTable[26] = new LexicalTableEntry({{CharData::GetLetterChar(), 26}, 
		{CharData::GetNonzeroChar(), 26}, {CharData::GetElseChar(), 27}, {'e', 26}, {'0', 26}, {'.', 26}});
	
	m_lexicalTable[27] = new LexicalTableEntry({}, true, true, TokenType::InvalidIdentifier);
	m_lexicalTable[28] = new LexicalTableEntry({}, true, true, TokenType::None);
	m_lexicalTable[29] = new LexicalTableEntry({}, true, true, TokenType::InvalidNumber);
	m_lexicalTable[30] = new LexicalTableEntry({{CharData::GetLetterChar(), 30}, 
		{CharData::GetNonzeroChar(), 30}, {CharData::GetElseChar(), 29}, 
		{CharData::GetEOFChar(), 29}, {CharData::GetFloatPowerChar(), 30}, 
		{'0', 30}, {'.', 30}, {'_', 30}});

	m_lexicalTable[31] = new LexicalTableEntry({}, true, false, TokenType::InvalidCharacter);
}

Lexer& Lexer::GetInstance()
{
	static Lexer l;
	return l;
}
