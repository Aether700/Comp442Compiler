#include "Parser.h"
#include "../Core/Token.h"
#include "../Lexer/Lexer.h"
#include "../Core/Core.h"

// StackableItem //////////////////////////////////////
StackableItem::StackableItem(TokenType t) : m_isTerminal(true), m_item(t) { }
StackableItem::StackableItem(NonTerminal nonTerminal) 
    : m_isTerminal(false), m_item(nonTerminal) { }

StackableItem::~StackableItem()
{
    if (IsTerminal())
    {
        m_item.m_terminal.~TokenType();
    }
    else
    {
        m_item.m_nonTerminal.~NonTerminal();
    }
}
    
bool StackableItem::IsTerminal() const { return m_isTerminal; }

TokenType StackableItem::GetTerminal() const 
{
    if (!IsTerminal())
    {
        DEBUG_BREAK();
        return TokenType::None;
    }
    return m_item.m_terminal;
}

NonTerminal StackableItem::GetNonTerminal() const
{
    if (IsTerminal())
    {
        DEBUG_BREAK();
        return NonTerminal::None;
    }
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
    // temp rules for testing
    m_rules[0] = new Rule(NonTerminal::Start, { TokenType::And, NonTerminal::Start });
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