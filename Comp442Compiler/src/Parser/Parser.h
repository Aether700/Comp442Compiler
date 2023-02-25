#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <vector>

#include "AST.h"
#include "../Core/Token.h"
#include "../Core/Core.h"


typedef size_t RuleID;
static constexpr RuleID NullRule = SIZE_MAX;

enum class ErrorID
{
    Default,
    InvalidTypeSpecifier,
    InvalidFunctionHead,
    InvalidArgumentDefinition,
    InvalidArgumentProvided,
    InvalidArithExpr,
    ErroneousTokenAtFuncDef,
    InvalidStatement,
    InvalidArraySize,
    InvalidRelExpr,
    InvalidOperator,
};

enum class SemanticAction
{
    PushStopNode,
    PushUnspecifiedDimensionNode,
    ConstructIntLiteral,
    ConstructFloatLiteral,
    ConstructVisibility,
    ConstructDefaultVisibility,
    ConstructAssignStat,
    PushID,
    PushOp,
    PushType,
    EncounteredDot,
    ConstructExpr,
    ConstructAddOp,
    ConstructMultOp,
    ConstructRelOp,
    ConstructDimensions,
    ConstructVariable,
    ConstructVarDecl,
    ConstructStatBlock,
    ConstructFParams,
    ConstructAParams,
    ConstructFuncDef,
    ConstructFuncCall,
    ConstructIfStat,
    ConstructWhileStat,
    ConstructReadStat,
    ConstructWriteStat,
    ConstructReturnStat,
    ConstructDotNode,
    ConstructEncounteredDots,
    ConstructClass,
    ConstructMemVar,
};

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
    SemanticActionItem
};

class StackableItem
{
public:
    StackableItem();
    StackableItem(TokenType t);
    StackableItem(NonTerminal nonTerminal);
    StackableItem(SemanticAction action);
    ~StackableItem();
    
    StackableType GetType() const;
    TokenType GetTerminal() const;
    NonTerminal GetNonTerminal() const;
    SemanticAction GetAction() const;

private:
    union Item
    {
        Item(TokenType t);
        Item(NonTerminal nonTerminal);
        Item(SemanticAction action);
        ~Item();

        TokenType m_terminal;
        NonTerminal m_nonTerminal;
        SemanticAction m_action;
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
    Rule(const std::initializer_list<StackableItem>& right);
    
    virtual void Apply(const StackableItem& top, const Token& currToken) const;
    const std::list<StackableItem>& GetRightSide() const;

private:
    std::list<StackableItem> m_rightSide;
};

class ParsingErrorRule : public Rule
{
public:
    ParsingErrorRule(ErrorID id);

    virtual void Apply(const StackableItem& top, const Token& currToken) const override;
private:
    ErrorID m_errorID;
};

class RuleManager
{
public:
    static const Rule* GetRule(RuleID id);
    static bool IsError(RuleID id);

private:
    RuleManager();
    ~RuleManager();

    // if a rule is greater than this cutoff it is an error
    static constexpr RuleID s_errorCutoff = 111;

    static RuleManager& GetInstance();
    void InitializeRules();

    std::vector<Rule*> m_rules;
    Rule* m_defaultRule;
};

class ParsingTableEntry
{
public:
    ParsingTableEntry(const std::initializer_list<std::pair<TokenType, RuleID>>& entries);

    // returns NullRule if the provided token does not have a valid entry in the 
    // table and if no else clause was specified using TokenType::None
    RuleID GetRule(const Token& t) const;
private:
    std::unordered_map<TokenType, RuleID> m_entries;
};

class ParsingErrorData
{
public:
    ParsingErrorData(const Token& token, const StackableItem& top, ErrorID id);

    const StackableItem& GetTop() const;
    const Token& GetToken() const;
    ErrorID GetID() const;

private:
    StackableItem m_top;
    Token m_token;
    ErrorID m_id;
};

class ParsingErrorManager
{
public:
    static void WriteErrorToFile(std::ofstream& file, const ParsingErrorData& error);

private:
    ParsingErrorManager();
    ~ParsingErrorManager();

    // custom error message functions
    static void DefaultError(std::ofstream& file, const ParsingErrorData& error);
    static void InvalidTypeSpecifierError(std::ofstream& file, const ParsingErrorData& error);
    static void InvalidFunctionHeadError(std::ofstream& file, const ParsingErrorData& error);
    static void InvalidArgumentDefinitionError(std::ofstream& file, 
        const ParsingErrorData& error);
    static void InvalidArgumentProvidedError(std::ofstream& file, 
        const ParsingErrorData& error);
    static void InvalidArithExprError(std::ofstream& file, 
        const ParsingErrorData& error);
    static void ErroneousTokenAtFuncDefError(std::ofstream& file, 
        const ParsingErrorData& error);
    static void InvalidStatementError(std::ofstream& file, 
        const ParsingErrorData& error);
    static void InvalidArraySizeError(std::ofstream& file, 
        const ParsingErrorData& error);
    static void InvalidRelExprError(std::ofstream& file, 
        const ParsingErrorData& error);
    static void InvalidOperatorError(std::ofstream& file, 
        const ParsingErrorData& error);
    

    static ParsingErrorManager& GetInstance();
};

class Parser
{
    friend class Rule;
    friend class ParsingErrorRule;
public:
    // returns the root program node of the parsed program or nullptr if the program is not valid 
    static ProgramNode* Parse(const std::string& filepath);
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
    void RemoveNonTerminalsFromDerivation();
    void WriteErrorsToFile();
    Token SkipError(const Token& currToken, const StackableItem& top);
    void PopNonTerminal();

    // SemanticAction processing
    void ProcessSemanticAction(SemanticAction action);
    
    void ConstructIntLiteralAction();
    void ConstructFloatLiteralAction();
    void ConstructVisibilityAction();
    void ConstructDefaultVisibilityAction();
    void ConstructExprAction();
    void ConstructAssignStatAction();
    void ConstructVariableAction();
    void ConstructVarDeclAction();
    void ConstructFuncDefAction();
    void ConstructFuncCallAction();
    void ConstructIfStatAction();
    void ConstructWhileStatAction();
    void ConstructReadStatAction();
    void ConstructWriteStatAction();
    void ConstructReturnStatAction();
    void ConstructClassAction();
    void ConstructMemVarAction();

    // Will only construct DotNode objects to push to the semantic stack if an associated dot 
    // symbol was encountered, 
    // returns true if a dot node was constructed false otherwise.
    bool ConstructDotNodeAction();
    void ConstructEncounteredDotsAction();

    template<typename NodeType, typename... Args>
    void Push(Args... args)
    {
        m_semanticStack.push_front(new NodeType(std::forward<Args>(args)...));
    }

    template<typename NodeType>
    void ConstructBinaryOperatorNode()
    {
        ASTNode* right = m_semanticStack.front();
        m_semanticStack.pop_front();

        OperatorNode* op = dynamic_cast<OperatorNode*>(m_semanticStack.front());
        ASSERT(op != nullptr);
        m_semanticStack.pop_front();

        ASTNode* left = m_semanticStack.front();
        m_semanticStack.pop_front();

        m_semanticStack.push_front(new NodeType(left, op, right));
    }

    template<typename NodeType>
    void ConstructLoopingNode()
    {
        NodeType* targetNode = new NodeType();
        ASTNode* topNode = m_semanticStack.front();
        m_semanticStack.pop_front();
        while(dynamic_cast<StopNode*>(topNode) == nullptr)
        {
            targetNode->AddLoopingChild(topNode);
            topNode = m_semanticStack.front();
            m_semanticStack.pop_front();
        }
        delete topNode;

        m_semanticStack.push_front(targetNode);
    }

    template<typename NodeType>
    NodeType* PopTargetNodeFromSemanticStack()
    {
        ASTNode* top = m_semanticStack.front();
        m_semanticStack.pop_front();
        auto s = top->ToString();
        NodeType* targetNode = dynamic_cast<NodeType*>(top);
        ASSERT(targetNode != nullptr);
        return targetNode;
    }

    // returns index of first nonterminal or -1 if none was found
    void UpdateNextNonTerminalIndex();
    void WriteLexicalErrorToFile(const Token& t);

    std::unordered_map<NonTerminal, ParsingTableEntry*> m_parsingTable;

    ProgramNode* m_currProgramRoot;
    Token m_prevToken;
    std::list<StackableItem> m_parsingStack; // front is top of stack
    std::list<ASTNode*> m_semanticStack;
    bool m_errorFound;
    std::ofstream m_derivationFile;
    std::ofstream m_errorFile;
    std::ofstream m_astOutFile;
    std::vector<StackableItem> m_derivation;
    size_t m_nextNonTerminalIndex;
    std::list<ParsingErrorData> m_errors;
    size_t m_numDotsEncountered;
};