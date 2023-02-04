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
    m_rules.push_back(new Rule(NonTerminal::ClassDecl, { TokenType::Class, TokenType::ID, 
        NonTerminal::ClassDeclInheritance, TokenType::OpenCurlyBracket, 
        NonTerminal::ClassDeclMembDeclRepetition, TokenType::CloseCurlyBracket, 
        TokenType::SemiColon })); 

    // 4
    m_rules.push_back(new Rule(NonTerminal::ClassDeclMembDeclRepetition, 
        { NonTerminal::Visibility, NonTerminal::MemberDecl, 
        NonTerminal::ClassDeclMembDeclRepetition }));

    // 5
    m_rules.push_back(new Rule(NonTerminal::ClassDeclMembDeclRepetition, { }));

    // 6
    m_rules.push_back(new Rule(NonTerminal::ClassDeclInheritance, 
        { TokenType::IsA, TokenType::ID, NonTerminal::ClassDeclInheritanceTail }));

    // 7
    m_rules.push_back(new Rule(NonTerminal::ClassDeclInheritance, { }));
    
    // 8
    m_rules.push_back(new Rule(NonTerminal::ClassDeclInheritanceTail, 
        { TokenType::Comma, TokenType::ID, NonTerminal::ClassDeclInheritanceTail }));

    // 9
    m_rules.push_back(new Rule(NonTerminal::ClassDeclInheritanceTail, { }));

    // 10
    m_rules.push_back(new Rule(NonTerminal::Visibility, { TokenType::Public }));

    // 11
    m_rules.push_back(new Rule(NonTerminal::Visibility, { TokenType::Private }));

    // 12
    m_rules.push_back(new Rule(NonTerminal::Visibility, { }));

    // 13
    m_rules.push_back(new Rule(NonTerminal::MemberDecl, { NonTerminal::MemberFuncDecl }));

    // 14
    m_rules.push_back(new Rule(NonTerminal::MemberDecl, { NonTerminal::MemberVarDecl }));

    // 15
    m_rules.push_back(new Rule(NonTerminal::MemberFuncDecl, { TokenType::Function, 
        TokenType::ID, TokenType::Colon, TokenType::OpenParanthese, NonTerminal::FParams, 
        TokenType::CloseParanthese, TokenType::Arrow, NonTerminal::ReturnType, 
        TokenType::SemiColon }));

    // 16
    m_rules.push_back(new Rule(NonTerminal::MemberFuncDecl, { TokenType::Constructor, 
        TokenType::Colon, TokenType::OpenParanthese, NonTerminal::FParams, 
        TokenType::CloseParanthese, TokenType::SemiColon }));

    // 17
    m_rules.push_back(new Rule(NonTerminal::MemberVarDecl, { TokenType::Attribute, 
        TokenType::ID, TokenType::Colon,  NonTerminal::Type, NonTerminal::ArraySizeRepetition, 
        TokenType::SemiColon }));

    // 18
    m_rules.push_back(new Rule(NonTerminal::FuncDef, { NonTerminal::FuncHead, 
        NonTerminal::FuncBody }));

    // 19
    m_rules.push_back(new Rule(NonTerminal::FuncHead, { TokenType::Function, 
        TokenType::ID, NonTerminal::FuncHead2 }));
    
    // 20
    m_rules.push_back(new Rule(NonTerminal::FuncHead2, { TokenType::Scope, 
        NonTerminal::FuncHead3 }));

    // 21
    m_rules.push_back(new Rule(NonTerminal::FuncHead2, { TokenType::OpenParanthese, 
        NonTerminal::FParams, TokenType::CloseParanthese, TokenType::Arrow, 
        NonTerminal::ReturnType }));
    
    // 22
    m_rules.push_back(new Rule(NonTerminal::FuncHead3, { TokenType::ID, 
        TokenType::OpenParanthese, NonTerminal::FParams, TokenType::CloseParanthese, 
        TokenType::Arrow, NonTerminal::ReturnType }));

    // 23
    m_rules.push_back(new Rule(NonTerminal::FuncHead3, { TokenType::Constructor, 
        TokenType::OpenParanthese, NonTerminal::FParams, TokenType::CloseParanthese }));

    // 24
    m_rules.push_back(new Rule(NonTerminal::FuncBody, { TokenType::OpenCurlyBracket, 
        NonTerminal::LocalVarDeclOrStmtRepetition, TokenType::CloseCurlyBracket }));

    // 25
    m_rules.push_back(new Rule(NonTerminal::LocalVarDeclOrStmtRepetition, {
        NonTerminal::LocalVarDeclOrStmt, NonTerminal::LocalVarDeclOrStmtRepetition }));

    // 26
    m_rules.push_back(new Rule(NonTerminal::LocalVarDeclOrStmtRepetition, { }));

    // 27
    m_rules.push_back(new Rule(NonTerminal::LocalVarDeclOrStmt, { NonTerminal::LocalVarDecl }));

    // 28
    m_rules.push_back(new Rule(NonTerminal::LocalVarDeclOrStmt, { NonTerminal::Statement }));

    // 29
    m_rules.push_back(new Rule(NonTerminal::LocalVarDecl, { TokenType::LocalVar, TokenType::ID, 
        TokenType::Colon, NonTerminal::Type, NonTerminal::LocalVarDecl2, 
        TokenType::SemiColon }));

    // 30
    m_rules.push_back(new Rule(NonTerminal::LocalVarDecl2, { 
        NonTerminal::ArraySizeRepetition }));

    // 31
    m_rules.push_back(new Rule(NonTerminal::LocalVarDecl2, { 
        TokenType::OpenParanthese, NonTerminal::AParams, TokenType::CloseParanthese }));

    // 32
    m_rules.push_back(new Rule(NonTerminal::Statement, { NonTerminal::SimpleStatement, 
        TokenType::SemiColon }));

    // 33
    m_rules.push_back(new Rule(NonTerminal::Statement, { TokenType::If, 
        TokenType::OpenParanthese, NonTerminal::RelExpr, TokenType::CloseParanthese, 
        TokenType::Then, NonTerminal::StatBlock, TokenType::Else, NonTerminal::StatBlock, 
        TokenType::SemiColon }));

    // 34
    m_rules.push_back(new Rule(NonTerminal::Statement, { TokenType::While, 
        TokenType::OpenParanthese, NonTerminal::RelExpr, TokenType::CloseParanthese, 
        NonTerminal::StatBlock, TokenType::SemiColon }));

    // 35
    m_rules.push_back(new Rule(NonTerminal::Statement, { TokenType::Read, 
        TokenType::OpenParanthese, NonTerminal::Variable, TokenType::CloseParanthese, 
        TokenType::SemiColon }));

    // 36
    m_rules.push_back(new Rule(NonTerminal::Statement, { TokenType::Write, 
        TokenType::OpenParanthese, NonTerminal::Expr, TokenType::CloseParanthese, 
        TokenType::SemiColon }));

    // 37
    m_rules.push_back(new Rule(NonTerminal::Statement, { TokenType::Return, 
        TokenType::OpenParanthese, NonTerminal::Expr, TokenType::CloseParanthese, 
        TokenType::SemiColon }));

    // 38
    m_rules.push_back(new Rule(NonTerminal::SimpleStatement, { TokenType::ID, 
        NonTerminal::SimpleStatement2 }));
    
    // 39
    m_rules.push_back(new Rule(NonTerminal::SimpleStatement2, { NonTerminal::Indice, 
        NonTerminal::SimpleStatement3 }));

    // 40
    m_rules.push_back(new Rule(NonTerminal::SimpleStatement2, { TokenType::OpenParanthese, 
        NonTerminal::AParams, TokenType::CloseParanthese, NonTerminal::SimpleStatement4 }));

    // 41
    m_rules.push_back(new Rule(NonTerminal::SimpleStatement3, { TokenType::Dot, 
        NonTerminal::SimpleStatement }));

    // 42
    m_rules.push_back(new Rule(NonTerminal::SimpleStatement3, { TokenType::Assign, 
        NonTerminal::Expr }));

    // 43
    m_rules.push_back(new Rule(NonTerminal::SimpleStatement4, { TokenType::Dot, 
        NonTerminal::SimpleStatement }));

    // 44
    m_rules.push_back(new Rule(NonTerminal::SimpleStatement4, { }));

    Next rule -> StatementRepetition (following working grammar for website)
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
    // temp table for testing
    m_parsingTable[NonTerminal::Start] = new ParsingTableEntry({{TokenType::And, 0}});
}