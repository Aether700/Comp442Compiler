#include "Parser.h"
#include "../Core/Token.h"
#include "../Lexer/Lexer.h"
#include "../Core/Core.h"

// StackableItem //////////////////////////////////////
StackableItem::StackableItem(TokenType t) 
    : m_type(StackableType::TerminalItem), m_item(t) { }
StackableItem::StackableItem(NonTerminal nonTerminal) 
    : m_type(StackableType::NonTerminalItem), m_item(nonTerminal) { }

StackableItem::~StackableItem()
{
    switch(m_type)
    {
    case StackableType::TerminalItem:
        m_item.m_terminal.~TokenType();
        break;

    case StackableType::NonTerminalItem:
        m_item.m_nonTerminal.~NonTerminal();
        break;

    default:
        DEBUG_BREAK();
        break;
    }
}
    
StackableType StackableItem::GetType() const { return m_type; }

TokenType StackableItem::GetTerminal() const 
{
    ASSERT(m_type == StackableType::TerminalItem);
    return m_item.m_terminal;
}

NonTerminal StackableItem::GetNonTerminal() const
{
    ASSERT(m_type == StackableType::NonTerminalItem);
    return m_item.m_nonTerminal;
}

StackableItem::Item::Item(TokenType t) : m_terminal(t) { }
StackableItem::Item::Item(NonTerminal nonTerminal) : m_nonTerminal(nonTerminal) { }
StackableItem::Item::~Item() { }

// Rule //////////////////////////////////////////////

Rule::Rule(NonTerminal left, const std::initializer_list<StackableItem>& right) 
    : m_leftSide(left), m_rightSide(right) { }
    
NonTerminal Rule::GetLeftSide() const { return m_leftSide; }
const std::list<StackableItem>& Rule::GetRightSide() const { return m_rightSide; }

// RuleManager /////////////////////////////////////////////////////////////////////////
const Rule* RuleManager::GetRule(RuleID id)
{
    if (id == NullRule)
    {
        DEBUG_BREAK();
        return nullptr;
    }
    return GetInstance().m_rules[id];
}

RuleManager::RuleManager() { InitializeRules(); }
RuleManager::~RuleManager() 
{ 
    for (Rule* r : m_rules)
    {
        delete r;
    }
}

RuleManager& RuleManager::GetInstance()
{
    static RuleManager manager;
    return manager;
}

void RuleManager::InitializeRules()
{
    // 0
    m_rules.push_back(new Rule(NonTerminal::Start,  
        { NonTerminal::ClassDeclOrFuncDefRepetition }));
    
    // 1
    m_rules.push_back(new Rule(NonTerminal::ClassDeclOrFuncDefRepetition, 
        { NonTerminal::ClassDeclOrFuncDef, NonTerminal::ClassDeclOrFuncDef }));

    // 2
    m_rules.push_back(new Rule(NonTerminal::ClassDeclOrFuncDefRepetition, { })); 

    // 3
    m_rules.push_back(new Rule(NonTerminal::ClassDeclOrFuncDef, { NonTerminal::ClassDecl })); 

    // 4
    m_rules.push_back(new Rule(NonTerminal::ClassDeclOrFuncDef, { NonTerminal::FuncDef })); 

    // 5
    m_rules.push_back(new Rule(NonTerminal::ClassDecl, { TokenType::Class, TokenType::ID, 
        NonTerminal::ClassDeclInheritance, TokenType::OpenCurlyBracket, 
        NonTerminal::ClassDeclMembDeclRepetition, TokenType::CloseCurlyBracket, 
        TokenType::SemiColon })); 

    // 6
    m_rules.push_back(new Rule(NonTerminal::ClassDeclMembDeclRepetition, 
        { NonTerminal::Visibility, NonTerminal::MemberDecl, 
        NonTerminal::ClassDeclMembDeclRepetition }));

    // 7
    m_rules.push_back(new Rule(NonTerminal::ClassDeclMembDeclRepetition, { }));

    // 8
    m_rules.push_back(new Rule(NonTerminal::ClassDeclInheritance, 
        { TokenType::IsA, TokenType::ID, NonTerminal::ClassDeclInheritanceTail }));

    // 9
    m_rules.push_back(new Rule(NonTerminal::ClassDeclInheritance, { }));
    
    // 10
    m_rules.push_back(new Rule(NonTerminal::ClassDeclInheritanceTail, 
        { TokenType::Comma, TokenType::ID, NonTerminal::ClassDeclInheritanceTail }));

    // 11
    m_rules.push_back(new Rule(NonTerminal::ClassDeclInheritanceTail, { }));

    // 12
    m_rules.push_back(new Rule(NonTerminal::Visibility, { TokenType::Public }));

    // 13
    m_rules.push_back(new Rule(NonTerminal::Visibility, { TokenType::Private }));

    // 14
    m_rules.push_back(new Rule(NonTerminal::Visibility, { }));

    // 15
    m_rules.push_back(new Rule(NonTerminal::MemberDecl, { NonTerminal::MemberFuncDecl }));

    // 16
    m_rules.push_back(new Rule(NonTerminal::MemberDecl, { NonTerminal::MemberVarDecl }));

    // 17
    m_rules.push_back(new Rule(NonTerminal::MemberFuncDecl, { TokenType::Function, 
        TokenType::ID, TokenType::Colon, TokenType::OpenParanthese, NonTerminal::FParams, 
        TokenType::CloseParanthese, TokenType::Arrow, NonTerminal::ReturnType, 
        TokenType::SemiColon }));

    // 18
    m_rules.push_back(new Rule(NonTerminal::MemberFuncDecl, { TokenType::Constructor, 
        TokenType::Colon, TokenType::OpenParanthese, NonTerminal::FParams, 
        TokenType::CloseParanthese, TokenType::SemiColon }));

    // 19
    m_rules.push_back(new Rule(NonTerminal::MemberVarDecl, { TokenType::Attribute, 
        TokenType::ID, TokenType::Colon,  NonTerminal::Type, NonTerminal::ArraySizeRepetition, 
        TokenType::SemiColon }));

    // 20
    m_rules.push_back(new Rule(NonTerminal::FuncDef, { NonTerminal::FuncHead, 
        NonTerminal::FuncBody }));

    // 21
    m_rules.push_back(new Rule(NonTerminal::FuncHead, { TokenType::Function, 
        TokenType::ID, NonTerminal::FuncHead2 }));
    
    // 22
    m_rules.push_back(new Rule(NonTerminal::FuncHead2, { TokenType::Scope, 
        NonTerminal::FuncHead3 }));

    // 23
    m_rules.push_back(new Rule(NonTerminal::FuncHead2, { TokenType::OpenParanthese, 
        NonTerminal::FParams, TokenType::CloseParanthese, TokenType::Arrow, 
        NonTerminal::ReturnType }));
    
    // 24
    m_rules.push_back(new Rule(NonTerminal::FuncHead3, { TokenType::ID, 
        TokenType::OpenParanthese, NonTerminal::FParams, TokenType::CloseParanthese, 
        TokenType::Arrow, NonTerminal::ReturnType }));

    // 25
    m_rules.push_back(new Rule(NonTerminal::FuncHead3, { TokenType::Constructor, 
        TokenType::OpenParanthese, NonTerminal::FParams, TokenType::CloseParanthese }));

    // 26
    m_rules.push_back(new Rule(NonTerminal::FuncBody, { TokenType::OpenCurlyBracket, 
        NonTerminal::LocalVarDeclOrStmtRepetition, TokenType::CloseCurlyBracket }));

    // 27
    m_rules.push_back(new Rule(NonTerminal::LocalVarDeclOrStmtRepetition, {
        NonTerminal::LocalVarDeclOrStmt, NonTerminal::LocalVarDeclOrStmtRepetition }));

    // 28
    m_rules.push_back(new Rule(NonTerminal::LocalVarDeclOrStmtRepetition, { }));

    // 29
    m_rules.push_back(new Rule(NonTerminal::LocalVarDeclOrStmt, { NonTerminal::LocalVarDecl }));

    // 30
    m_rules.push_back(new Rule(NonTerminal::LocalVarDeclOrStmt, { NonTerminal::Statement }));

    // 31
    m_rules.push_back(new Rule(NonTerminal::LocalVarDecl, { TokenType::LocalVar, TokenType::ID, 
        TokenType::Colon, NonTerminal::Type, NonTerminal::LocalVarDecl2, 
        TokenType::SemiColon }));

    // 32
    m_rules.push_back(new Rule(NonTerminal::LocalVarDecl2, { 
        NonTerminal::ArraySizeRepetition }));

    // 33
    m_rules.push_back(new Rule(NonTerminal::LocalVarDecl2, { 
        TokenType::OpenParanthese, NonTerminal::AParams, TokenType::CloseParanthese }));

    // 34
    m_rules.push_back(new Rule(NonTerminal::Statement, { NonTerminal::SimpleStatement, 
        TokenType::SemiColon }));

    // 35
    m_rules.push_back(new Rule(NonTerminal::Statement, { TokenType::If, 
        TokenType::OpenParanthese, NonTerminal::RelExpr, TokenType::CloseParanthese, 
        TokenType::Then, NonTerminal::StatBlock, TokenType::Else, NonTerminal::StatBlock, 
        TokenType::SemiColon }));

    // 36
    m_rules.push_back(new Rule(NonTerminal::Statement, { TokenType::While, 
        TokenType::OpenParanthese, NonTerminal::RelExpr, TokenType::CloseParanthese, 
        NonTerminal::StatBlock, TokenType::SemiColon }));

    // 37
    m_rules.push_back(new Rule(NonTerminal::Statement, { TokenType::Read, 
        TokenType::OpenParanthese, NonTerminal::Variable, TokenType::CloseParanthese, 
        TokenType::SemiColon }));

    // 38
    m_rules.push_back(new Rule(NonTerminal::Statement, { TokenType::Write, 
        TokenType::OpenParanthese, NonTerminal::Expr, TokenType::CloseParanthese, 
        TokenType::SemiColon }));

    // 39
    m_rules.push_back(new Rule(NonTerminal::Statement, { TokenType::Return, 
        TokenType::OpenParanthese, NonTerminal::Expr, TokenType::CloseParanthese, 
        TokenType::SemiColon }));

    // 40
    m_rules.push_back(new Rule(NonTerminal::SimpleStatement, { TokenType::ID, 
        NonTerminal::SimpleStatement2 }));
    
    // 41
    m_rules.push_back(new Rule(NonTerminal::SimpleStatement2, { NonTerminal::Indice, 
        NonTerminal::SimpleStatement3 }));

    // 42
    m_rules.push_back(new Rule(NonTerminal::SimpleStatement2, { TokenType::OpenParanthese, 
        NonTerminal::AParams, TokenType::CloseParanthese, NonTerminal::SimpleStatement4 }));

    // 43
    m_rules.push_back(new Rule(NonTerminal::SimpleStatement3, { TokenType::Dot, 
        NonTerminal::SimpleStatement }));

    // 44
    m_rules.push_back(new Rule(NonTerminal::SimpleStatement3, { TokenType::Assign, 
        NonTerminal::Expr }));

    // 45
    m_rules.push_back(new Rule(NonTerminal::SimpleStatement4, { TokenType::Dot, 
        NonTerminal::SimpleStatement }));

    // 46
    m_rules.push_back(new Rule(NonTerminal::SimpleStatement4, { }));

    // 47
    m_rules.push_back(new Rule(NonTerminal::StatementRepetition, { NonTerminal::Statement, 
        NonTerminal::StatementRepetition }));

    // 48
    m_rules.push_back(new Rule(NonTerminal::StatementRepetition, { }));

    // 49
    m_rules.push_back(new Rule(NonTerminal::StatBlock, { TokenType::OpenCurlyBracket, 
        NonTerminal::StatementRepetition, TokenType::CloseCurlyBracket }));

    // 50
    m_rules.push_back(new Rule(NonTerminal::StatBlock, { NonTerminal::Statement }));

    // 51
    m_rules.push_back(new Rule(NonTerminal::StatBlock, { }));

    // 52
    m_rules.push_back(new Rule(NonTerminal::Expr, { NonTerminal::ArithExpr, 
        NonTerminal::Expr2 }));

    // 53
    m_rules.push_back(new Rule(NonTerminal::Expr2, { NonTerminal::RelOp, 
        NonTerminal::ArithExpr }));

    // 54
    m_rules.push_back(new Rule(NonTerminal::Expr2, { }));

    // 55
    m_rules.push_back(new Rule(NonTerminal::RelExpr, { NonTerminal::ArithExpr, 
        NonTerminal::RelOp, NonTerminal::ArithExpr }));

    // 56
    m_rules.push_back(new Rule(NonTerminal::ArithExpr, { NonTerminal::Term, 
        NonTerminal::ArithExpr2 }));

    // 57
    m_rules.push_back(new Rule(NonTerminal::ArithExpr2, { NonTerminal::AddOp, 
        NonTerminal::Term, NonTerminal::ArithExpr2 }));

    // 58
    m_rules.push_back(new Rule(NonTerminal::ArithExpr2, { }));

    // 59
    m_rules.push_back(new Rule(NonTerminal::Sign, { TokenType::Plus }));

    // 60
    m_rules.push_back(new Rule(NonTerminal::Sign, { TokenType::Minus }));

    // 61
    m_rules.push_back(new Rule(NonTerminal::Term, { NonTerminal::Factor, 
        NonTerminal::Term2 }));
    
    // 62
    m_rules.push_back(new Rule(NonTerminal::Term2, { NonTerminal::MultOp, 
        NonTerminal::Factor, NonTerminal::Term2 }));

    // 63
    m_rules.push_back(new Rule(NonTerminal::Term2, { }));

    // 64
    m_rules.push_back(new Rule(NonTerminal::Factor, { NonTerminal::VarOrFuncCall }));

    // 65
    m_rules.push_back(new Rule(NonTerminal::Factor, { TokenType::IntegerLiteral }));

    // 66
    m_rules.push_back(new Rule(NonTerminal::Factor, { TokenType::FloatLiteral }));

    // 67
    m_rules.push_back(new Rule(NonTerminal::Factor, { TokenType::OpenParanthese, 
        NonTerminal::ArithExpr, TokenType::CloseParanthese }));

    // 68
    m_rules.push_back(new Rule(NonTerminal::Factor, { TokenType::Not, NonTerminal::Factor }));

    // 69
    m_rules.push_back(new Rule(NonTerminal::Factor, { NonTerminal::Sign, 
        NonTerminal::Factor }));

    // 70
    m_rules.push_back(new Rule(NonTerminal::VarOrFuncCall, { TokenType::ID, 
        NonTerminal::VarOrFuncCall2 }));

    // 71
    m_rules.push_back(new Rule(NonTerminal::VarOrFuncCall2, { NonTerminal::Indice, 
        NonTerminal::VarOrFuncCall3 }));

    // 72
    m_rules.push_back(new Rule(NonTerminal::VarOrFuncCall2, { TokenType::OpenParanthese, 
        NonTerminal::AParams, TokenType::CloseParanthese, NonTerminal::VarOrFuncCall3 }));

    // 73
    m_rules.push_back(new Rule(NonTerminal::VarOrFuncCall3, { TokenType::Dot, 
        NonTerminal::VarOrFuncCall }));

    // 74
    m_rules.push_back(new Rule(NonTerminal::VarOrFuncCall3, { }));

    // 75
    m_rules.push_back(new Rule(NonTerminal::Variable, { TokenType::ID, 
        NonTerminal::Variable2 }));

    // 76
    m_rules.push_back(new Rule(NonTerminal::Variable2, { NonTerminal::Indice, 
        NonTerminal::Variable3 }));

    // 77
    m_rules.push_back(new Rule(NonTerminal::Variable2, { TokenType::OpenParanthese, 
        NonTerminal::AParams, TokenType::CloseParanthese, TokenType::Dot, 
        NonTerminal::Variable }));

    // 78
    m_rules.push_back(new Rule(NonTerminal::Variable3, { TokenType::Dot, 
        NonTerminal::Variable }));
    
    // 79
    m_rules.push_back(new Rule(NonTerminal::Variable3, { }));

    // 80
    m_rules.push_back(new Rule(NonTerminal::Indice, { TokenType::OpenSquareBracket, 
        NonTerminal::ArithExpr, TokenType::CloseSquareBracket, NonTerminal::Indice }));

    // 81
    m_rules.push_back(new Rule(NonTerminal::Indice, { }));

    // 82
    m_rules.push_back(new Rule(NonTerminal::ArraySize, { TokenType::OpenSquareBracket, 
        NonTerminal::ArraySize2 }));

    // 83
    m_rules.push_back(new Rule(NonTerminal::ArraySize2, { TokenType::IntegerLiteral, 
        TokenType::CloseSquareBracket }));

    // 84
    m_rules.push_back(new Rule(NonTerminal::ArraySize2, { TokenType::CloseSquareBracket }));

    // 85
    m_rules.push_back(new Rule(NonTerminal::ArraySizeRepetition, { NonTerminal::ArraySize, 
        NonTerminal::ArraySizeRepetition }));

    // 86
    m_rules.push_back(new Rule(NonTerminal::ArraySizeRepetition, { }));

    // 87
    m_rules.push_back(new Rule(NonTerminal::Type, { TokenType::IntegerKeyword }));

    // 88
    m_rules.push_back(new Rule(NonTerminal::Type, { TokenType::FloatKeyword }));

    // 89
    m_rules.push_back(new Rule(NonTerminal::Type, { TokenType::ID }));

    // 90
    m_rules.push_back(new Rule(NonTerminal::ReturnType, { NonTerminal::Type }));

    // 91
    m_rules.push_back(new Rule(NonTerminal::ReturnType, { TokenType::Void }));

    // 92
    m_rules.push_back(new Rule(NonTerminal::FParams, { TokenType::ID, TokenType::Colon, 
        NonTerminal::Type, NonTerminal::ArraySizeRepetition, NonTerminal::FParamsTail }));

    // 93
    m_rules.push_back(new Rule(NonTerminal::FParams, { }));

    // 94
    m_rules.push_back(new Rule(NonTerminal::AParams, { NonTerminal::Expr, 
        NonTerminal::AParamsTail }));

    // 95
    m_rules.push_back(new Rule(NonTerminal::AParams, { }));

    // 96
    m_rules.push_back(new Rule(NonTerminal::FParamsTail, { TokenType::Comma, 
        TokenType::ID, TokenType::Colon, NonTerminal::Type, NonTerminal::ArraySizeRepetition, 
        NonTerminal::FParamsTail }));

    // 97
    m_rules.push_back(new Rule(NonTerminal::FParamsTail, { }));

    // 98
    m_rules.push_back(new Rule(NonTerminal::AParamsTail, { TokenType::Comma, 
        NonTerminal::Expr, NonTerminal::AParamsTail }));

    // 99
    m_rules.push_back(new Rule(NonTerminal::AParamsTail, { }));

    // 100
    m_rules.push_back(new Rule(NonTerminal::RelOp, { TokenType::Equal }));

    // 101
    m_rules.push_back(new Rule(NonTerminal::RelOp, { TokenType::NotEqual }));

    // 102
    m_rules.push_back(new Rule(NonTerminal::RelOp, { TokenType::LessThan }));

    // 103
    m_rules.push_back(new Rule(NonTerminal::RelOp, { TokenType::GreaterThan }));

    // 104
    m_rules.push_back(new Rule(NonTerminal::RelOp, { TokenType::LessOrEqual }));

    // 105
    m_rules.push_back(new Rule(NonTerminal::RelOp, { TokenType::GreaterOrEqual }));

    // 106
    m_rules.push_back(new Rule(NonTerminal::AddOp, { TokenType::Plus }));

    // 107
    m_rules.push_back(new Rule(NonTerminal::AddOp, { TokenType::Minus }));

    // 108
    m_rules.push_back(new Rule(NonTerminal::AddOp, { TokenType::Or }));

    // 109
    m_rules.push_back(new Rule(NonTerminal::MultOp, { TokenType::Multiply }));

    // 110
    m_rules.push_back(new Rule(NonTerminal::MultOp, { TokenType::Divide }));

    // 111
    m_rules.push_back(new Rule(NonTerminal::MultOp, { TokenType::And }));
}

// ParsingTableEntry ///////////////////////////////////////////////////////////////////
ParsingTableEntry::ParsingTableEntry(
    const std::initializer_list<std::pair<TokenType, RuleID>>& entries)
{
    for (auto& pair : entries)
    {
        m_entries[pair.first] = pair.second;
    }
}

RuleID ParsingTableEntry::GetRule(const Token& t) const
{
    auto it = m_entries.find(t.GetTokenType());
    if (it == m_entries.end())
    {
        return NullRule;
    }
    return it->second;
}

// Parser ////////////////////////////////////////////
void Parser::Parse(const std::string& filepath)
{
    Lexer::SetInputFile(filepath);

    // Begin parsing here
}

Parser::Parser() { InitializeParsingTable(); }

Parser::~Parser()
{
    for (auto& pair : m_parsingTable)
    {
        delete pair.second;
    }
}

Parser& Parser::GetInstance()
{
    static Parser p;
    return p;
}

void Parser::InitializeParsingTable()
{
    m_parsingTable[NonTerminal::Start] = new ParsingTableEntry({{TokenType::EndOfFile, 0}, 
        {TokenType::Function, 0}, {TokenType::Class, 0}});

    m_parsingTable[NonTerminal::ClassDeclOrFuncDefRepetition] 
        = new ParsingTableEntry({{TokenType::EndOfFile, 2}, {TokenType::Function, 1}, 
        {TokenType::Class, 1}});

    m_parsingTable[NonTerminal::ClassDeclOrFuncDef] 
        = new ParsingTableEntry({{TokenType::Function, 4}, {TokenType::Class, 3}});
    
    m_parsingTable[NonTerminal::ClassDecl] = new ParsingTableEntry({{TokenType::Class, 5}});
    
    m_parsingTable[NonTerminal::ClassDeclMembDeclRepetition] 
        = new ParsingTableEntry({{TokenType::CloseCurlyBracket, 7}, 
        {TokenType::Constructor, 6}, {TokenType::Function, 6}, {TokenType::Attribute, 6}, 
        {TokenType::Private, 6}, {TokenType::Public, 6}});

    m_parsingTable[NonTerminal::ClassDeclInheritance] 
        = new ParsingTableEntry({{TokenType::OpenCurlyBracket, 9}, {TokenType::IsA, 8}});
           
    m_parsingTable[NonTerminal::ClassDeclInheritanceTail] 
        = new ParsingTableEntry({{TokenType::Comma, 10}, {TokenType::OpenCurlyBracket, 11}});

    m_parsingTable[NonTerminal::Visibility] 
        = new ParsingTableEntry({{TokenType::Constructor, 14}, {TokenType::Function, 14}, 
        {TokenType::Attribute, 14}, {TokenType::Private, 13}, {TokenType::Public, 12}});

    m_parsingTable[NonTerminal::MemberDecl] 
        = new ParsingTableEntry({{TokenType::Constructor, 15}, 
        {TokenType::Function, 15}, {TokenType::Attribute, 16}});

    m_parsingTable[NonTerminal::MemberFuncDecl] 
        = new ParsingTableEntry({{TokenType::Constructor, 18}, {TokenType::Function, 17}});

    m_parsingTable[NonTerminal::MemberVarDecl] 
        = new ParsingTableEntry({{TokenType::Attribute, 19}});

    m_parsingTable[NonTerminal::FuncDef] 
        = new ParsingTableEntry({{TokenType::Function, 20}});

    m_parsingTable[NonTerminal::FuncHead] 
        = new ParsingTableEntry({{TokenType::Function, 21}});

    m_parsingTable[NonTerminal::FuncHead2] 
        = new ParsingTableEntry({{TokenType::OpenParanthese, 23}, {TokenType::Scope, 22}});

    m_parsingTable[NonTerminal::FuncHead3] 
        = new ParsingTableEntry({{TokenType::ID, 24}, {TokenType::Constructor, 25}});

    m_parsingTable[NonTerminal::FuncBody] 
        = new ParsingTableEntry({{TokenType::OpenCurlyBracket, 26}});

    m_parsingTable[NonTerminal::MemberVarDecl] 
        = new ParsingTableEntry({{TokenType::Attribute, 19}});

    m_parsingTable[NonTerminal::LocalVarDeclOrStmtRepetition] 
        = new ParsingTableEntry({{TokenType::ID, 27}, {TokenType::CloseCurlyBracket, 28}, 
        {TokenType::Return, 27}, {TokenType::Write, 27}, {TokenType::Read, 27}, 
        {TokenType::While, 27}, {TokenType::If, 27}, {TokenType::LocalVar, 27}});

    m_parsingTable[NonTerminal::LocalVarDeclOrStmt] 
        = new ParsingTableEntry({{TokenType::ID, 30}, {TokenType::Return, 30}, 
        {TokenType::Write, 30}, {TokenType::Read, 30}, {TokenType::While, 30}, 
        {TokenType::If, 30}, {TokenType::LocalVar, 29}});

    m_parsingTable[NonTerminal::LocalVarDecl] 
        = new ParsingTableEntry({{TokenType::LocalVar, 31}});

    m_parsingTable[NonTerminal::LocalVarDecl2] 
        = new ParsingTableEntry({{TokenType::OpenSquareBracket, 32}, 
        {TokenType::OpenParanthese, 33}, {TokenType::SemiColon, 32}});

    m_parsingTable[NonTerminal::Statement] 
        = new ParsingTableEntry({{TokenType::ID, 34}, {TokenType::Return, 39}, 
        {TokenType::Write, 38}, {TokenType::Read, 37}, {TokenType::While, 36}, 
        {TokenType::If, 35}});

    m_parsingTable[NonTerminal::SimpleStatement] 
        = new ParsingTableEntry({{TokenType::ID, 40}});

    m_parsingTable[NonTerminal::SimpleStatement2] 
        = new ParsingTableEntry({{TokenType::OpenSquareBracket, 41}, 
        {TokenType::Dot, 41}, {TokenType::OpenParanthese, 42}, {TokenType::Assign, 41}});

    m_parsingTable[NonTerminal::SimpleStatement3] 
        = new ParsingTableEntry({{TokenType::Dot, 43}, {TokenType::Assign, 44}});

    m_parsingTable[NonTerminal::SimpleStatement4] 
        = new ParsingTableEntry({{TokenType::Dot, 45}, {TokenType::SemiColon, 46}});

    m_parsingTable[NonTerminal::StatementRepetition] 
        = new ParsingTableEntry({{TokenType::ID, 47}, {TokenType::CloseCurlyBracket, 48}, 
        {TokenType::Return, 47}, {TokenType::Write, 47}, {TokenType::Read, 47}, 
        {TokenType::While, 47}, {TokenType::If, 47}});

    m_parsingTable[NonTerminal::StatBlock] 
        = new ParsingTableEntry({{TokenType::ID, 50}, {TokenType::OpenCurlyBracket, 49}, 
        {TokenType::SemiColon, 51}, {TokenType::Return, 50}, {TokenType::Write, 50}, 
        {TokenType::Read, 50}, {TokenType::While, 50}, {TokenType::Else, 51}, 
        {TokenType::If, 50}});

}