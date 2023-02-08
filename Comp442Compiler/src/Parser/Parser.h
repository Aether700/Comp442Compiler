#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <vector>

#include "../Core/Token.h"

typedef size_t RuleID;
static constexpr RuleID NullRule = SIZE_MAX;

enum class NonTerminal
{
    None,
    Start,
    ClassDeclOrFuncDefRepetition,
    ClassDeclOrFuncDef,
    ClassDecl,
    ClassDeclMembDeclRepetition,
    ClassDeclInheritance,
    ClassDeclInheritanceTail,
    Visibility,
    MemberDecl,
    MemberFuncDecl,
    MemberVarDecl,
    FuncDef,
    FuncHead,
    FuncHead2,
    FuncHead3,
    FuncBody,
    LocalVarDeclOrStmtRepetition,
    LocalVarDeclOrStmt,
    LocalVarDecl,
    LocalVarDecl2,
    Statement,
    SimpleStatement,
    SimpleStatement2,
    SimpleStatement3,
    SimpleStatement4,
    StatementRepetition,
    StatBlock,
    Expr,
    Expr2,
    RelExpr,
    ArithExpr,
    ArithExpr2,
    Sign,
    Term,
    Term2,
    Factor,
    VarOrFuncCall,
    VarOrFuncCall2,
    VarOrFuncCall3,
    Variable,
    Variable2,
    Variable3,
    Indice,
    ArraySize,
    ArraySize2,
    ArraySizeRepetition,
    Type,
    ReturnType,
    FParams,
    AParams,
    FParamsTail,
    AParamsTail,
    RelOp,
    AddOp,
    MultOp
};

std::ostream& operator<<(std::ostream& stream, NonTerminal n);

enum class StackableType
{
    TerminalItem,
    NonTerminalItem,
    SemanticAction
};

class StackableItem
{
public:
    StackableItem();
    StackableItem(TokenType t);
    StackableItem(NonTerminal nonTerminal);
    ~StackableItem();
    
    StackableType GetType() const;
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
    StackableType m_type;
};

std::ostream& operator<<(std::ostream& stream, const StackableItem& item);

// Uses the TokenType::None as the epsilon token
class SetManager
{
public:
    // returns true if the provided terminal is in the first set of the provided non terminal
    static bool IsInFirstSet(NonTerminal n, TokenType t);
    static bool IsInFirstSet(const StackableItem& n, TokenType t);
    static bool IsInFirstSet(TokenType n, TokenType t);
    

    // returns true if the provided terminal is in the follow set of the provided non terminal
    static bool IsInFollowSet(NonTerminal n, TokenType t);
    static bool IsInFollowSet(const StackableItem& n, TokenType t);

private:
    SetManager();
    static SetManager& GetInstance();

    void InitializeSets();
    void InitializeFirstSets();
    void InitializeFollowSets();

    std::unordered_map<NonTerminal, std::list<TokenType>> m_firstSets;
    std::unordered_map<NonTerminal, std::list<TokenType>> m_followSets;
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
    static bool Parse(const std::string& filepath);
private:
    Parser();
    ~Parser();
    static Parser& GetInstance();
    static void Reset(const std::string& filepath);
    static Token GetNextToken();
    static bool TokenIsIgnored(TokenType type);

    void InitializeParsingTable();
    void PushToStack(const Rule* r);
    void WriteDerivationToFile();
    Token SkipError(const Token& currToken, const StackableItem& top);
    void PopNonTerminal();

    // returns index of first nonterminal or -1 if none was found
    void UpdateNextNonTerminalIndex();

    std::unordered_map<NonTerminal, ParsingTableEntry*> m_parsingTable;
    std::list<StackableItem> m_parsingStack; // front is top of stack
    bool m_errorFound;
    std::ofstream m_derivationFile;
    std::vector<StackableItem> m_derivation;
    size_t m_nextNonTerminalIndex;
};