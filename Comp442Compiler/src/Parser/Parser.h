#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <list>

#include "../Core/Token.h"

typedef size_t RuleID;
static constexpr RuleID NullRule = SIZE_MAX;

enum class NonTerminal
{
    None,
    Start
};

class StackableItem
{
public:
    StackableItem(TokenType t);
    StackableItem(NonTerminal nonTerminal);
    ~StackableItem();
    
    bool IsTerminal() const;
    TokenType GetTerminal() const;
    NonTerminal GetNonTerminal() const;

private:
    union Item
    {
        Item(TokenType t);
        Item(NonTerminal nonTerminal);
        ~Item();

        TokenType m_terminal;
        NonTerminal m_nonTerminal;
    } m_item;
    bool m_isTerminal;
};

class Rule
{
public:
    Rule(NonTerminal left, const std::initializer_list<StackableItem>& right);
    
    NonTerminal GetLeftSide() const;
    const std::list<StackableItem>& GetRightSide() const;

private:
    NonTerminal m_leftSide;
    std::list<StackableItem> m_rightSide;
};

class RuleManager
{
public:
    static const Rule* GetRule(RuleID id);

private:
    RuleManager();
    ~RuleManager();

    static RuleManager& GetInstance();
    void InitializeRules();

    std::vector<Rule*> m_rules;
};

class ParsingTableEntry
{
public:
    ParsingTableEntry(const std::initializer_list<std::pair<TokenType, RuleID>>& entries);

    // returns NullRule if the provided token does not have a valid entry in the table
    RuleID GetRule(const Token& t) const;
private:
    std::unordered_map<TokenType, RuleID> m_entries;
};

class Parser
{
public:
    static void Parse(const std::string& filepath);
private:
    Parser();
    ~Parser();
    static Parser& GetInstance();

    void InitializeParsingTable();

    std::unordered_map<NonTerminal, ParsingTableEntry*> m_parsingTable;
};