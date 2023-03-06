#include "Parser.h"
#include "../Lexer/Lexer.h"
#include "../Core/Util.h"

std::ostream& operator<<(std::ostream& stream, NonTerminal n)
{
    switch(n)
    {
    case NonTerminal::None:
        stream << "<None>";
        break;
    
    case NonTerminal::Start:
        stream << "<Start>";
        break;

    case NonTerminal::ClassDeclOrFuncDefRepetition:
        stream << "<ClassDeclOrFuncDefRepetition>";
        break;

    case NonTerminal::ClassDeclOrFuncDef:
        stream << "<ClassDeclOrFuncDef>";
        break;

    case NonTerminal::ClassDecl:
        stream << "<ClassDecl>";
        break;

    case NonTerminal::ClassDeclMembDeclRepetition:
        stream << "<ClassDeclMembDeclRepetition>";
        break;

    case NonTerminal::ClassDeclInheritance:
        stream << "<ClassDeclInheritance>";
        break;

    case NonTerminal::ClassDeclInheritanceTail:
        stream << "<ClassDeclInheritanceTail>";
        break;

    case NonTerminal::Visibility:
        stream << "<Visibility>";
        break;

    case NonTerminal::MemberDecl:
        stream << "<MemberDecl>";
        break;

    case NonTerminal::MemberFuncDecl:
        stream << "<MemberFuncDecl>";
        break;

    case NonTerminal::MemberVarDecl:
        stream << "<MemberVarDecl>";
        break;

    case NonTerminal::FuncDef:
        stream << "<FuncDef>";
        break;

    case NonTerminal::FuncHead:
        stream << "<FuncHead>";
        break;

    case NonTerminal::FuncHead2:
        stream << "<FuncHead2>";
        break;

    case NonTerminal::FuncHead3:
        stream << "<FuncHead3>";
        break;

    case NonTerminal::FuncBody:
        stream << "<FuncBody>";
        break;

    case NonTerminal::LocalVarDeclOrStmtRepetition:
        stream << "<LocalVarDeclOrStmtRepetition>";
        break;

    case NonTerminal::LocalVarDeclOrStmt:
        stream << "<LocalVarDeclOrStmt>";
        break;

    case NonTerminal::LocalVarDecl:
        stream << "<LocalVarDecl>";
        break;

    case NonTerminal::LocalVarDecl2:
        stream << "<LocalVarDecl2>";
        break;

    case NonTerminal::Statement:
        stream << "<Statement>";
        break;

    case NonTerminal::SimpleStatement:
        stream << "<SimpleStatement>";
        break;

    case NonTerminal::SimpleStatement2:
        stream << "<SimpleStatement2>";
        break;

    case NonTerminal::SimpleStatement3:
        stream << "<SimpleStatement3>";
        break;

    case NonTerminal::SimpleStatement4:
        stream << "<SimpleStatement4>";
        break;

    case NonTerminal::StatementRepetition:
        stream << "<StatementRepetition>";
        break;

    case NonTerminal::StatBlock:
        stream << "<StatBlock>";
        break;

    case NonTerminal::Expr:
        stream << "<Expr>";
        break;

    case NonTerminal::Expr2:
        stream << "<Expr2>";
        break;

    case NonTerminal::RelExpr:
        stream << "<RelExpr>";
        break;

    case NonTerminal::ArithExpr:
        stream << "<ArithExpr>";
        break;

    case NonTerminal::ArithExpr2:
        stream << "<ArithExpr2>";
        break;

    case NonTerminal::Sign:
        stream << "<Sign>";
        break;

    case NonTerminal::Term:
        stream << "<Term>";
        break;

    case NonTerminal::Term2:
        stream << "<Term2>";
        break;

    case NonTerminal::Factor:
        stream << "<Factor>";
        break;

    case NonTerminal::VarOrFuncCall:
        stream << "<VarOrFuncCall>";
        break;

    case NonTerminal::VarOrFuncCall2:
        stream << "<VarOrFuncCall2>";
        break;

    case NonTerminal::VarOrFuncCall3:
        stream << "<VarOrFuncCall3>";
        break;

    case NonTerminal::Variable:
        stream << "<Variable>";
        break;

    case NonTerminal::Variable2:
        stream << "<Variable2>";
        break;

    case NonTerminal::Variable3:
        stream << "<Variable3>";
        break;

    case NonTerminal::Indice:
        stream << "<Indice>";
        break;

    case NonTerminal::ArraySize:
        stream << "<ArraySize>";
        break;

    case NonTerminal::ArraySize2:
        stream << "<ArraySize2>";
        break;

    case NonTerminal::ArraySizeRepetition:
        stream << "<ArraySizeRepetition>";
        break;

    case NonTerminal::Type:
        stream << "<Type>";
        break;

    case NonTerminal::ReturnType:
        stream << "<ReturnType>";
        break;

    case NonTerminal::FParams:
        stream << "<FParams>";
        break;

    case NonTerminal::AParams:
        stream << "<AParams>";
        break;

    case NonTerminal::FParamsTail:
        stream << "<FParamsTail>";
        break;

    case NonTerminal::AParamsTail:
        stream << "<AParamsTail>";
        break;

    case NonTerminal::RelOp:
        stream << "<RelOp>";
        break;

    case NonTerminal::AddOp:
        stream << "<AddOp>";
        break;

    case NonTerminal::MultOp:
        stream << "<MultOp>";
        break;

    default:
        stream << "Unknown Non Terminal";
        DEBUG_BREAK();
        break;
    }

    return stream;
}

// StackableItem //////////////////////////////////////
StackableItem::StackableItem() 
    : m_type(StackableType::NonTerminalItem), m_item(NonTerminal::None) { }
StackableItem::StackableItem(TokenType t) 
    : m_type(StackableType::TerminalItem), m_item(t) { }
StackableItem::StackableItem(NonTerminal nonTerminal) 
    : m_type(StackableType::NonTerminalItem), m_item(nonTerminal) { }
StackableItem::StackableItem(SemanticAction action)
    : m_type(StackableType::SemanticActionItem), m_item(action) { }

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

    case StackableType::SemanticActionItem:
        m_item.m_action.~SemanticAction();
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

SemanticAction StackableItem::GetAction() const
{
    ASSERT(m_type == StackableType::SemanticActionItem);
    return m_item.m_action;
}

StackableItem::Item::Item(TokenType t) : m_terminal(t) { }
StackableItem::Item::Item(NonTerminal nonTerminal) : m_nonTerminal(nonTerminal) { }
StackableItem::Item::Item(SemanticAction action) : m_action(action) { }
StackableItem::Item::~Item() { }

std::ostream& operator<<(std::ostream& stream, const StackableItem& item)
{
    switch(item.GetType())
    {
    case StackableType::NonTerminalItem:
        stream << item.GetNonTerminal();
        break;

    case StackableType::TerminalItem:
        stream << '\'' << TokenTypeToStr(item.GetTerminal()) << '\'';
        break;

    case StackableType::SemanticActionItem:
        // don't print semantic actions
        break;

    default:
        stream << "Unknown StackableItem";
        DEBUG_BREAK();
        break;
    }

    return stream;
}

// SetManager /////////////////////////////////////////////////////////////////
bool SetManager::IsInFirstSet(NonTerminal n, TokenType t)
{
    auto& set = GetInstance().m_firstSets[n];
    for (TokenType curr : set)
    {
        if (curr == t)
        {
            return true;
        }
    }
    return false;
}

bool SetManager::IsInFirstSet(const StackableItem& n, TokenType t) 
{
    if (n.GetType() == StackableType::NonTerminalItem)
    {
        return IsInFirstSet(n.GetNonTerminal(), t);
    }
    else if (n.GetType() == StackableType::TerminalItem)
    {
        return IsInFirstSet(n.GetTerminal(), t);
    }
    return false;
}

bool SetManager::IsInFirstSet(TokenType n, TokenType t) { return n == t; }

bool SetManager::IsInFollowSet(NonTerminal n, TokenType t)
{
    auto& set = GetInstance().m_followSets[n];
    for (TokenType curr : set)
    {
        if (curr == t)
        {
            return true;
        }
    }
    return false;
}

bool SetManager::IsInFollowSet(const StackableItem& n, TokenType t)
{
    if (n.GetType() == StackableType::NonTerminalItem)
    {
        return IsInFollowSet(n.GetNonTerminal(), t);
    }
    return false;
}

SetManager::SetManager() { InitializeSets(); }

SetManager& SetManager::GetInstance()
{
    static SetManager manager;
    return manager;
}

void SetManager::InitializeSets()
{
    InitializeFirstSets();
    InitializeFollowSets();
}

void SetManager::InitializeFirstSets()
{
    m_firstSets[NonTerminal::Start] = {TokenType::Class, TokenType::Function, 
        TokenType::None};

    m_firstSets[NonTerminal::ClassDeclOrFuncDefRepetition] 
        = {TokenType::Class, TokenType::Function, TokenType::None};
    
    m_firstSets[NonTerminal::ClassDeclOrFuncDef] = {TokenType::Class, TokenType::Function};
    m_firstSets[NonTerminal::ClassDecl] = {TokenType::Class};
    m_firstSets[NonTerminal::ClassDeclMembDeclRepetition] = 
        {TokenType::Public, TokenType::Private, TokenType::Function, 
        TokenType::Constructor, TokenType::Attribute, TokenType::None};
    
    m_firstSets[NonTerminal::ClassDeclInheritance] = {TokenType::IsA, TokenType::None};
    m_firstSets[NonTerminal::ClassDeclInheritanceTail] = {TokenType::Comma, TokenType::None};
    m_firstSets[NonTerminal::Visibility] = {TokenType::Public, TokenType::Private, 
        TokenType::None};
    
    m_firstSets[NonTerminal::MemberDecl] = {TokenType::Function, 
        TokenType::Constructor, TokenType::Attribute};
    
    m_firstSets[NonTerminal::MemberFuncDecl] = {TokenType::Function, TokenType::Constructor};
    m_firstSets[NonTerminal::MemberVarDecl] = {TokenType::Attribute};
    m_firstSets[NonTerminal::Start] = {TokenType::Class, TokenType::Function};
    m_firstSets[NonTerminal::FuncDef] = {TokenType::Function};
    m_firstSets[NonTerminal::FuncHead] = {TokenType::Function};
    m_firstSets[NonTerminal::FuncHead2] = {TokenType::Scope, TokenType::OpenParanthese};
    m_firstSets[NonTerminal::FuncHead3] = {TokenType::ID, TokenType::Constructor};
    m_firstSets[NonTerminal::FuncBody] = {TokenType::OpenCurlyBracket};
    m_firstSets[NonTerminal::LocalVarDeclOrStmtRepetition] = {TokenType::LocalVar, 
        TokenType::If, TokenType::While, TokenType::Read, TokenType::Write, 
        TokenType::Return, TokenType::ID, TokenType::None};

    m_firstSets[NonTerminal::LocalVarDeclOrStmt] = {TokenType::LocalVar, 
        TokenType::If, TokenType::While, TokenType::Read, TokenType::Write, 
        TokenType::Return, TokenType::ID};

    m_firstSets[NonTerminal::LocalVarDecl] = {TokenType::LocalVar};
    m_firstSets[NonTerminal::LocalVarDecl2] = {TokenType::OpenParanthese, 
        TokenType::OpenSquareBracket, TokenType::None};

    m_firstSets[NonTerminal::Statement] = {TokenType::If, TokenType::While, TokenType::Read, 
        TokenType::Write, TokenType::Return, TokenType::ID};

    m_firstSets[NonTerminal::SimpleStatement] = {TokenType::ID};
    m_firstSets[NonTerminal::SimpleStatement2] = {TokenType::OpenParanthese, 
        TokenType::OpenSquareBracket, TokenType::Dot, TokenType::Assign};

    m_firstSets[NonTerminal::SimpleStatement3] = {TokenType::Dot, TokenType::Assign};
    m_firstSets[NonTerminal::SimpleStatement4] = {TokenType::Dot, TokenType::None};

    m_firstSets[NonTerminal::StatementRepetition] = {TokenType::If, TokenType::While, 
        TokenType::Read, TokenType::Write, TokenType::Return, TokenType::ID, TokenType::None};

    m_firstSets[NonTerminal::StatBlock] = {TokenType::OpenCurlyBracket, TokenType::If, 
        TokenType::While, TokenType::Read, TokenType::Write, TokenType::Return, 
        TokenType::ID, TokenType::None};

    m_firstSets[NonTerminal::Expr] = {TokenType::IntegerLiteral, TokenType::FloatLiteral, 
        TokenType::OpenParanthese, TokenType::Not, TokenType::ID, TokenType::Plus, 
        TokenType::Minus};

    m_firstSets[NonTerminal::Expr2] = {TokenType::Equal, TokenType::NotEqual, 
        TokenType::LessThan, TokenType::GreaterThan, TokenType::LessOrEqual, 
        TokenType::GreaterOrEqual, TokenType::None};

    m_firstSets[NonTerminal::RelExpr] = {TokenType::IntegerLiteral, TokenType::FloatLiteral, 
        TokenType::OpenParanthese, TokenType::Not, TokenType::ID, TokenType::Plus, 
        TokenType::Minus};

    m_firstSets[NonTerminal::ArithExpr] = {TokenType::IntegerLiteral, TokenType::FloatLiteral, 
        TokenType::OpenParanthese, TokenType::Not, TokenType::ID, TokenType::Plus, 
        TokenType::Minus};

    m_firstSets[NonTerminal::ArithExpr2] = {TokenType::Plus, 
        TokenType::Minus, TokenType::Or, TokenType::None};

    m_firstSets[NonTerminal::Sign] = {TokenType::Plus, TokenType::Minus};
    m_firstSets[NonTerminal::Term] = {TokenType::IntegerLiteral, TokenType::FloatLiteral, 
        TokenType::OpenParanthese, TokenType::Not, TokenType::ID, TokenType::Plus, 
        TokenType::Minus};

    m_firstSets[NonTerminal::Term2] = {TokenType::Multiply, 
        TokenType::Divide, TokenType::And};
    
    m_firstSets[NonTerminal::Factor] = {TokenType::IntegerLiteral, TokenType::FloatLiteral, 
        TokenType::OpenParanthese, TokenType::Not, TokenType::ID, TokenType::Plus, 
        TokenType::Minus};

    m_firstSets[NonTerminal::VarOrFuncCall] = {TokenType::ID};
    m_firstSets[NonTerminal::VarOrFuncCall2] = {TokenType::OpenParanthese, 
        TokenType::OpenSquareBracket, TokenType::Dot, TokenType::None};

    m_firstSets[NonTerminal::VarOrFuncCall3] = {TokenType::Dot, TokenType::None};
    m_firstSets[NonTerminal::Variable] = {TokenType::ID};
    m_firstSets[NonTerminal::Variable2] = {TokenType::OpenParanthese, 
        TokenType::OpenSquareBracket, TokenType::Dot, TokenType::None};

    m_firstSets[NonTerminal::Variable3] = {TokenType::Dot, TokenType::None};
    m_firstSets[NonTerminal::Indice] = {TokenType::OpenSquareBracket, TokenType::None};
    m_firstSets[NonTerminal::ArraySize] = {TokenType::OpenSquareBracket};
    m_firstSets[NonTerminal::ArraySize2] = {TokenType::IntegerLiteral, 
        TokenType::CloseSquareBracket};

    m_firstSets[NonTerminal::ArraySizeRepetition] = {TokenType::OpenSquareBracket, 
        TokenType::None};
    m_firstSets[NonTerminal::Type] = {TokenType::IntegerKeyword, TokenType::FloatKeyword, 
        TokenType::ID};

    m_firstSets[NonTerminal::ReturnType] = {TokenType::IntegerKeyword, 
        TokenType::FloatKeyword, TokenType::ID, TokenType::Void};

    m_firstSets[NonTerminal::FParams] = {TokenType::ID, TokenType::None};
    m_firstSets[NonTerminal::AParams] = {TokenType::IntegerLiteral, TokenType::FloatLiteral, 
        TokenType::OpenParanthese, TokenType::Not, TokenType::ID, TokenType::Plus, 
        TokenType::Minus, TokenType::None};

    m_firstSets[NonTerminal::FParamsTail] = {TokenType::Comma, TokenType::None};
    m_firstSets[NonTerminal::AParamsTail] = {TokenType::Comma, TokenType::None};
    
    m_firstSets[NonTerminal::RelOp] = {TokenType::Equal, TokenType::NotEqual, 
        TokenType::LessThan, TokenType::GreaterThan, TokenType::LessOrEqual, 
        TokenType::GreaterOrEqual};

    m_firstSets[NonTerminal::AddOp] = {TokenType::Plus, TokenType::Minus, TokenType::Or};
    m_firstSets[NonTerminal::MultOp] = {TokenType::Multiply, 
        TokenType::Divide, TokenType::And};
}

void SetManager::InitializeFollowSets()
{
    m_followSets[NonTerminal::Start] = {TokenType::EndOfFile};
    m_followSets[NonTerminal::ClassDeclOrFuncDefRepetition] = {TokenType::EndOfFile};
    m_followSets[NonTerminal::ClassDeclOrFuncDef] = {TokenType::Class, TokenType::Function};
    m_followSets[NonTerminal::ClassDecl] = {TokenType::Class, TokenType::Function};
    m_followSets[NonTerminal::ClassDeclMembDeclRepetition] = {TokenType::CloseCurlyBracket};
    m_followSets[NonTerminal::ClassDeclInheritance] = {TokenType::OpenCurlyBracket};
    m_followSets[NonTerminal::ClassDeclInheritanceTail] = {TokenType::OpenCurlyBracket};
    m_followSets[NonTerminal::Visibility] = {TokenType::Function, 
        TokenType::Constructor, TokenType::Attribute};

    m_followSets[NonTerminal::MemberDecl] = {TokenType::Public, TokenType::Private, 
        TokenType::Function, TokenType::Constructor, TokenType::Attribute, 
        TokenType::CloseCurlyBracket};

    m_followSets[NonTerminal::MemberFuncDecl] = {TokenType::Public, TokenType::Private, 
        TokenType::Function, TokenType::Constructor, TokenType::Attribute, 
        TokenType::CloseCurlyBracket};

    m_followSets[NonTerminal::MemberVarDecl] = {TokenType::Public, TokenType::Private, 
        TokenType::Function, TokenType::Constructor, TokenType::Attribute, 
        TokenType::CloseCurlyBracket};

    m_followSets[NonTerminal::FuncDef] = {TokenType::Class, TokenType::Function};
    m_followSets[NonTerminal::FuncHead] = {TokenType::OpenCurlyBracket};
    m_followSets[NonTerminal::FuncHead2] = {TokenType::OpenCurlyBracket};
    m_followSets[NonTerminal::FuncHead3] = {TokenType::OpenCurlyBracket};
    m_followSets[NonTerminal::FuncBody] = {TokenType::Class, TokenType::Function};
    m_followSets[NonTerminal::LocalVarDeclOrStmtRepetition] = {TokenType::CloseCurlyBracket};
    m_followSets[NonTerminal::LocalVarDeclOrStmt] = {TokenType::LocalVar, TokenType::If,
        TokenType::While, TokenType::Read, TokenType::Write, TokenType::Return, 
        TokenType::ID, TokenType::CloseCurlyBracket};    

    m_followSets[NonTerminal::LocalVarDecl] = {TokenType::LocalVar, TokenType::If,
        TokenType::While, TokenType::Read, TokenType::Write, TokenType::Return, 
        TokenType::ID, TokenType::CloseCurlyBracket};    
    
    m_followSets[NonTerminal::LocalVarDecl2] = {TokenType::SemiColon};   
    m_followSets[NonTerminal::LocalVarDeclOrStmt] = {TokenType::Else, TokenType::SemiColon,
        TokenType::LocalVar, TokenType::If, TokenType::While, TokenType::Read, 
        TokenType::Write, TokenType::Return, TokenType::ID, TokenType::CloseCurlyBracket};    

    m_followSets[NonTerminal::SimpleStatement] = {TokenType::SemiColon};
    m_followSets[NonTerminal::SimpleStatement2] = {TokenType::SemiColon};
    m_followSets[NonTerminal::SimpleStatement3] = {TokenType::SemiColon};
    m_followSets[NonTerminal::SimpleStatement4] = {TokenType::SemiColon};
    m_followSets[NonTerminal::StatementRepetition] = {TokenType::CloseCurlyBracket};
    m_followSets[NonTerminal::StatBlock] = {TokenType::Else, TokenType::SemiColon};
    m_followSets[NonTerminal::Expr] = {TokenType::Comma, TokenType::SemiColon, 
        TokenType::CloseParanthese};

    m_followSets[NonTerminal::Expr2] = {TokenType::Comma, TokenType::SemiColon, 
        TokenType::CloseParanthese};

    m_followSets[NonTerminal::RelExpr] = {TokenType::CloseParanthese};
    m_followSets[NonTerminal::ArithExpr] = {TokenType::Equal, TokenType::NotEqual, 
        TokenType::LessThan, TokenType::GreaterThan, TokenType::LessOrEqual, 
        TokenType::GreaterOrEqual, TokenType::CloseSquareBracket, TokenType::Comma, 
        TokenType::SemiColon, TokenType::CloseParanthese};

    m_followSets[NonTerminal::ArithExpr2] = {TokenType::Equal, TokenType::NotEqual, 
        TokenType::LessThan, TokenType::GreaterThan, TokenType::LessOrEqual, 
        TokenType::GreaterOrEqual, TokenType::CloseSquareBracket, TokenType::Comma, 
        TokenType::SemiColon, TokenType::CloseParanthese};

    m_followSets[NonTerminal::Sign] = {TokenType::IntegerLiteral, TokenType::FloatLiteral, 
        TokenType::OpenParanthese, TokenType::Not, TokenType::ID, TokenType::Plus, 
        TokenType::Minus};

    m_followSets[NonTerminal::Term] = {TokenType::Plus, TokenType::Minus, TokenType::Or, 
        TokenType::Equal, TokenType::NotEqual, TokenType::LessThan, TokenType::GreaterThan, 
        TokenType::LessOrEqual, TokenType::GreaterOrEqual, TokenType::CloseSquareBracket, 
        TokenType::Comma, TokenType::SemiColon, TokenType::CloseParanthese};

    m_followSets[NonTerminal::Term2] = {TokenType::Plus, TokenType::Minus, TokenType::Or, 
        TokenType::Equal, TokenType::NotEqual, TokenType::LessThan, TokenType::GreaterThan, 
        TokenType::LessOrEqual, TokenType::GreaterOrEqual, TokenType::CloseSquareBracket, 
        TokenType::Comma, TokenType::SemiColon, TokenType::CloseParanthese};

    m_followSets[NonTerminal::Factor] = {TokenType::Multiply, TokenType::Divide, 
        TokenType::And, TokenType::Plus, TokenType::Minus, TokenType::Or, 
        TokenType::Equal, TokenType::NotEqual, TokenType::LessThan, TokenType::GreaterThan, 
        TokenType::LessOrEqual, TokenType::GreaterOrEqual, TokenType::CloseSquareBracket, 
        TokenType::Comma, TokenType::SemiColon, TokenType::CloseParanthese};

    m_followSets[NonTerminal::VarOrFuncCall] = {TokenType::Multiply, TokenType::Divide, 
        TokenType::And, TokenType::Plus, TokenType::Minus, TokenType::Or, 
        TokenType::Equal, TokenType::NotEqual, TokenType::LessThan, TokenType::GreaterThan, 
        TokenType::LessOrEqual, TokenType::GreaterOrEqual, TokenType::CloseSquareBracket, 
        TokenType::Comma, TokenType::SemiColon, TokenType::CloseParanthese};

    m_followSets[NonTerminal::VarOrFuncCall2] = {TokenType::Multiply, TokenType::Divide, 
        TokenType::And, TokenType::Plus, TokenType::Minus, TokenType::Or, 
        TokenType::Equal, TokenType::NotEqual, TokenType::LessThan, TokenType::GreaterThan, 
        TokenType::LessOrEqual, TokenType::GreaterOrEqual, TokenType::CloseSquareBracket, 
        TokenType::Comma, TokenType::SemiColon, TokenType::CloseParanthese};

    m_followSets[NonTerminal::VarOrFuncCall3] = {TokenType::Multiply, TokenType::Divide, 
        TokenType::And, TokenType::Plus, TokenType::Minus, TokenType::Or, 
        TokenType::Equal, TokenType::NotEqual, TokenType::LessThan, TokenType::GreaterThan, 
        TokenType::LessOrEqual, TokenType::GreaterOrEqual, TokenType::CloseSquareBracket, 
        TokenType::Comma, TokenType::SemiColon, TokenType::CloseParanthese};

    m_followSets[NonTerminal::Variable] = {TokenType::CloseParanthese};
    m_followSets[NonTerminal::Variable2] = {TokenType::CloseParanthese};
    m_followSets[NonTerminal::Variable3] = {TokenType::CloseParanthese};

    m_followSets[NonTerminal::Indice] = {TokenType::Multiply, TokenType::Divide, 
        TokenType::And, TokenType::Plus, TokenType::Minus, TokenType::Or, 
        TokenType::Equal, TokenType::NotEqual, TokenType::LessThan, TokenType::GreaterThan, 
        TokenType::LessOrEqual, TokenType::GreaterOrEqual, TokenType::CloseSquareBracket, 
        TokenType::Assign, TokenType::Dot, TokenType::Comma, TokenType::SemiColon, 
        TokenType::CloseParanthese};

    m_followSets[NonTerminal::ArraySize] = {TokenType::OpenSquareBracket, 
        TokenType::SemiColon, TokenType::Comma, TokenType::CloseParanthese};

    m_followSets[NonTerminal::ArraySize2] = {TokenType::OpenSquareBracket, 
        TokenType::SemiColon, TokenType::Comma, TokenType::CloseParanthese};

    m_followSets[NonTerminal::ArraySizeRepetition] = {TokenType::SemiColon, 
        TokenType::Comma, TokenType::CloseParanthese};

    m_followSets[NonTerminal::Type] = {TokenType::OpenCurlyBracket, TokenType::OpenParanthese, 
        TokenType::OpenSquareBracket, TokenType::Comma, TokenType::SemiColon, 
        TokenType::CloseParanthese};

    m_followSets[NonTerminal::ReturnType] = {TokenType::OpenCurlyBracket, 
        TokenType::SemiColon};

    m_followSets[NonTerminal::FParams] = {TokenType::CloseParanthese};
    m_followSets[NonTerminal::AParams] = {TokenType::CloseParanthese};
    m_followSets[NonTerminal::FParamsTail] = {TokenType::CloseParanthese};
    m_followSets[NonTerminal::AParamsTail] = {TokenType::CloseParanthese};
    m_followSets[NonTerminal::RelOp] = {TokenType::IntegerLiteral, TokenType::FloatLiteral, 
        TokenType::OpenParanthese, TokenType::Not, TokenType::ID, 
        TokenType::Plus, TokenType::Minus};

    m_followSets[NonTerminal::AddOp] = {TokenType::IntegerLiteral, TokenType::FloatLiteral, 
        TokenType::OpenParanthese, TokenType::Not, TokenType::ID, 
        TokenType::Plus, TokenType::Minus};

    m_followSets[NonTerminal::MultOp] = {TokenType::IntegerLiteral, TokenType::FloatLiteral, 
        TokenType::OpenParanthese, TokenType::Not, TokenType::ID, 
        TokenType::Plus, TokenType::Minus};
}


// Rule //////////////////////////////////////////////

Rule::Rule(const std::initializer_list<StackableItem>& right) 
    : m_rightSide(right) { }
    
void Rule::Apply(const StackableItem& top, const Token& currToken) const
{
    Parser::GetInstance().PushToStack(this);
}

const std::list<StackableItem>& Rule::GetRightSide() const { return m_rightSide; }

// ParsingErrorRule //////////////////////////////////////////////////

ParsingErrorRule::ParsingErrorRule(ErrorID id) 
    : Rule({}), m_errorID(id) { }

void ParsingErrorRule::Apply(const StackableItem& top, const Token& currToken) const
{
    Parser& p = Parser::GetInstance();
    p.m_errors.emplace_front(currToken, top, m_errorID);
}

// RuleManager /////////////////////////////////////////////////////////////////////////
const Rule* RuleManager::GetRule(RuleID id)
{
    if (id == NullRule)
    {
        return GetInstance().m_defaultRule;
    }
    return GetInstance().m_rules[id];
}

bool RuleManager::IsError(RuleID id) { return  id > s_errorCutoff; }

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
    m_defaultRule = new ParsingErrorRule(ErrorID::Default);

    // 0 Start
    m_rules.push_back(new Rule({ NonTerminal::ClassDeclOrFuncDefRepetition }));
    
    // 1 ClassDeclOrFuncDefRepetition
    m_rules.push_back(new Rule({ NonTerminal::ClassDeclOrFuncDef, 
        NonTerminal::ClassDeclOrFuncDefRepetition }));

    // 2 ClassDeclOrFuncDefRepetition
    m_rules.push_back(new Rule({ })); 

    // 3 ClassDeclOrFuncDef
    m_rules.push_back(new Rule({ NonTerminal::ClassDecl })); 

    // 4 ClassDeclOrFuncDef
    m_rules.push_back(new Rule({ NonTerminal::FuncDef })); 

    // 5 ClassDecl
    m_rules.push_back(new Rule({ TokenType::Class, TokenType::ID, SemanticAction::PushID,
        NonTerminal::ClassDeclInheritance, TokenType::OpenCurlyBracket, 
        SemanticAction::PushStopNode, NonTerminal::ClassDeclMembDeclRepetition, 
        TokenType::CloseCurlyBracket, SemanticAction::ConstructClass, 
        TokenType::SemiColon })); 

    // 6 ClassDeclMembDeclRepetition
    m_rules.push_back(new Rule({ NonTerminal::Visibility, NonTerminal::MemberDecl, 
        NonTerminal::ClassDeclMembDeclRepetition }));

    // 7 ClassDeclMembDeclRepetition
    m_rules.push_back(new Rule({ }));

    // 8 ClassDeclInheritance
    m_rules.push_back(new Rule({ SemanticAction::PushStopNode, TokenType::IsA, 
        TokenType::ID, SemanticAction::PushID, NonTerminal::ClassDeclInheritanceTail }));

    // 9 ClassDeclInheritance
    m_rules.push_back(new Rule({ SemanticAction::PushStopNode, 
        SemanticAction::ConstructInheritanceList }));
    
    // 10 ClassDeclInheritanceTail
    m_rules.push_back(new Rule({ TokenType::Comma, TokenType::ID, SemanticAction::PushID,
        NonTerminal::ClassDeclInheritanceTail }));

    // 11 ClassDeclInheritanceTail
    m_rules.push_back(new Rule({ SemanticAction::ConstructInheritanceList }));

    // 12 Visibility
    m_rules.push_back(new Rule({ TokenType::Public, SemanticAction::ConstructVisibility }));

    // 13 Visibility
    m_rules.push_back(new Rule({ TokenType::Private, SemanticAction::ConstructVisibility }));

    // 14 Visibility
    m_rules.push_back(new Rule({ SemanticAction::ConstructDefaultVisibility }));

    // 15 MemberDecl
    m_rules.push_back(new Rule({ NonTerminal::MemberFuncDecl }));

    // 16 MemberDecl
    m_rules.push_back(new Rule({ NonTerminal::MemberVarDecl }));

    // 17 MemberFuncDecl
    m_rules.push_back(new Rule({ TokenType::Function, 
        TokenType::ID, SemanticAction::PushID, TokenType::Colon, TokenType::OpenParanthese, 
        SemanticAction::PushStopNode, NonTerminal::FParams, SemanticAction::ConstructFParams, 
        TokenType::CloseParanthese, TokenType::Arrow, NonTerminal::ReturnType, 
        SemanticAction::ConstructMemFuncDecl, TokenType::SemiColon }));

    // 18 MemberFuncDecl
    m_rules.push_back(new Rule({ TokenType::Constructor, 
        TokenType::Colon, TokenType::OpenParanthese, SemanticAction::PushStopNode, 
        NonTerminal::FParams, SemanticAction::ConstructFParams,
        TokenType::CloseParanthese, SemanticAction::ConstructConstructorDecl, 
        TokenType::SemiColon }));

    // 19 MemberVarDecl
    m_rules.push_back(new Rule({ TokenType::Attribute, 
        TokenType::ID, SemanticAction::PushID, TokenType::Colon,  NonTerminal::Type, 
        SemanticAction::PushStopNode, NonTerminal::ArraySizeRepetition, 
        SemanticAction::ConstructMemVar, TokenType::SemiColon }));

    // 20 FuncDef
    m_rules.push_back(new Rule({ NonTerminal::FuncHead, NonTerminal::FuncBody }));

    // 21 FuncHead
    m_rules.push_back(new Rule({ TokenType::Function, 
        TokenType::ID, SemanticAction::PushID, NonTerminal::FuncHead2 }));
    
    // 22 FuncHead2
    m_rules.push_back(new Rule({ TokenType::Scope, NonTerminal::FuncHead3 }));

    // 23 FuncHead2
    m_rules.push_back(new Rule({ TokenType::OpenParanthese, SemanticAction::PushStopNode,
        NonTerminal::FParams, SemanticAction::ConstructFParams, TokenType::CloseParanthese, 
        TokenType::Arrow, NonTerminal::ReturnType, SemanticAction::PushFreeFuncMarker }));
    
    // 24 FuncHead3
    m_rules.push_back(new Rule({ TokenType::ID, SemanticAction::PushID,
        TokenType::OpenParanthese, SemanticAction::PushStopNode, NonTerminal::FParams, 
        SemanticAction::ConstructFParams, TokenType::CloseParanthese, 
        TokenType::Arrow, NonTerminal::ReturnType, SemanticAction::PushMemFuncMarker }));

    // 25 FuncHead3
    m_rules.push_back(new Rule({ TokenType::Constructor, 
        TokenType::OpenParanthese, SemanticAction::PushStopNode, NonTerminal::FParams, 
        SemanticAction::ConstructFParams, TokenType::CloseParanthese, 
        SemanticAction::PushConstructorMarker }));

    // 26 FuncBody
    m_rules.push_back(new Rule({ TokenType::OpenCurlyBracket, 
        SemanticAction::PushStopNode, NonTerminal::LocalVarDeclOrStmtRepetition, 
        SemanticAction::ConstructStatBlock, TokenType::CloseCurlyBracket, 
        SemanticAction::ConstructFuncDef }));

    // 27 LocalVarDeclOrStmtRepetition
    m_rules.push_back(new Rule({ NonTerminal::LocalVarDeclOrStmt, 
        NonTerminal::LocalVarDeclOrStmtRepetition }));

    // 28 LocalVarDeclOrStmtRepetition
    m_rules.push_back(new Rule({ }));

    // 29 LocalVarDeclOrStmt
    m_rules.push_back(new Rule({ NonTerminal::LocalVarDecl }));

    // 30 LocalVarDeclOrStmt
    m_rules.push_back(new Rule({ NonTerminal::Statement }));

    // 31 LocalVarDecl
    m_rules.push_back(new Rule({ TokenType::LocalVar, TokenType::ID, 
        SemanticAction::PushID, TokenType::Colon, NonTerminal::Type, 
        NonTerminal::LocalVarDecl2, SemanticAction::ConstructVarDecl, 
        TokenType::SemiColon }));

    // 32 LocalVarDecl2
    m_rules.push_back(new Rule({ SemanticAction::PushStopNode, 
        NonTerminal::ArraySizeRepetition }));

    // 33 LocalVarDecl2
    m_rules.push_back(new Rule({ TokenType::OpenParanthese, SemanticAction::PushStopNode, 
        NonTerminal::AParams, SemanticAction::ConstructAParams, TokenType::CloseParanthese }));

    // 34 Statement
    m_rules.push_back(new Rule({ NonTerminal::SimpleStatement, 
        TokenType::SemiColon }));

    // 35 Statement
    m_rules.push_back(new Rule({ TokenType::If, 
        TokenType::OpenParanthese, NonTerminal::RelExpr, TokenType::CloseParanthese, 
        TokenType::Then, NonTerminal::StatBlock, TokenType::Else, NonTerminal::StatBlock, 
        SemanticAction::ConstructIfStat, TokenType::SemiColon }));

    // 36 Statement
    m_rules.push_back(new Rule({ TokenType::While, 
        TokenType::OpenParanthese, NonTerminal::RelExpr, TokenType::CloseParanthese, 
        NonTerminal::StatBlock, SemanticAction::ConstructWhileStat, 
        TokenType::SemiColon }));

    // 37 Statement
    m_rules.push_back(new Rule({ TokenType::Read, 
        TokenType::OpenParanthese, NonTerminal::Variable, TokenType::CloseParanthese, 
        SemanticAction::ConstructReadStat, TokenType::SemiColon }));

    // 38 Statement
    m_rules.push_back(new Rule({ TokenType::Write, 
        TokenType::OpenParanthese, NonTerminal::Expr, TokenType::CloseParanthese, 
        SemanticAction::ConstructWriteStat, TokenType::SemiColon }));

    // 39 Statement
    m_rules.push_back(new Rule({ TokenType::Return, 
        TokenType::OpenParanthese, NonTerminal::Expr, TokenType::CloseParanthese, 
        SemanticAction::ConstructReturnStat, TokenType::SemiColon }));

    // 40 SimpleStatement
    m_rules.push_back(new Rule({ TokenType::ID, SemanticAction::PushID, 
        NonTerminal::SimpleStatement2 }));
    
    // 41 SimpleStatement2
    m_rules.push_back(new Rule({ SemanticAction::PushStopNode, NonTerminal::Indice, 
        SemanticAction::ConstructVariable, NonTerminal::SimpleStatement3 }));

    // 42 SimpleStatement2
    m_rules.push_back(new Rule({ TokenType::OpenParanthese, 
        SemanticAction::PushStopNode, NonTerminal::AParams, SemanticAction::ConstructAParams, 
        TokenType::CloseParanthese, SemanticAction::ConstructFuncCall, 
        NonTerminal::SimpleStatement4 }));

    // 43 SimpleStatement3
    m_rules.push_back(new Rule({ TokenType::Dot, SemanticAction::EncounteredDot, 
        NonTerminal::SimpleStatement, SemanticAction::ConstructDotNode }));

    // 44 SimpleStatement3
    m_rules.push_back(new Rule({ TokenType::Assign, SemanticAction::ConstructEncounteredDots, 
        NonTerminal::Expr, SemanticAction::ConstructAssignStat }));

    // 45 SimpleStatement4
    m_rules.push_back(new Rule({ TokenType::Dot, SemanticAction::EncounteredDot, 
        NonTerminal::SimpleStatement, SemanticAction::ConstructDotNode }));

    // 46 SimpleStatement4
    m_rules.push_back(new Rule({ }));

    // 47 StatementRepetition
    m_rules.push_back(new Rule({ NonTerminal::Statement, NonTerminal::StatementRepetition }));

    // 48 StatementRepetition
    m_rules.push_back(new Rule({ }));

    // 49 StatBlock
    m_rules.push_back(new Rule({ SemanticAction::PushStopNode, TokenType::OpenCurlyBracket, 
        NonTerminal::StatementRepetition, SemanticAction::ConstructStatBlock, 
        TokenType::CloseCurlyBracket }));

    // 50 StatBlock
    m_rules.push_back(new Rule({ SemanticAction::PushStopNode, NonTerminal::Statement, 
        SemanticAction::ConstructStatBlock }));

    // 51 StatBlock
    m_rules.push_back(new Rule({ SemanticAction::PushStopNode, 
        SemanticAction::ConstructStatBlock}));

    // 52 Expr
    m_rules.push_back(new Rule({ NonTerminal::ArithExpr, NonTerminal::Expr2 }));

    // 53 Expr2
    m_rules.push_back(new Rule({ NonTerminal::RelOp, NonTerminal::ArithExpr, 
        SemanticAction::ConstructRelOp, SemanticAction::ConstructExpr }));

    // 54 Expr2
    m_rules.push_back(new Rule({ SemanticAction::ConstructExpr }));

    // 55 RelExpr
    m_rules.push_back(new Rule({ NonTerminal::ArithExpr, 
        NonTerminal::RelOp, NonTerminal::ArithExpr, SemanticAction::ConstructRelOp, 
        SemanticAction::ConstructExpr }));

    // 56 ArithExpr
    m_rules.push_back(new Rule({ NonTerminal::Term, NonTerminal::ArithExpr2 }));

    // 57 ArithExpr2
    m_rules.push_back(new Rule({ NonTerminal::AddOp, NonTerminal::Term, 
        SemanticAction::ConstructAddOp, NonTerminal::ArithExpr2 }));

    // 58 ArithExpr2
    m_rules.push_back(new Rule({ }));

    // 59 Sign
    m_rules.push_back(new Rule({ TokenType::Plus, SemanticAction::PushSign }));

    // 60 Sign
    m_rules.push_back(new Rule({ TokenType::Minus, SemanticAction::PushSign }));

    // 61 Term
    m_rules.push_back(new Rule({ NonTerminal::Factor, 
        NonTerminal::Term2 }));
    
    // 62 Term2
    m_rules.push_back(new Rule({ NonTerminal::MultOp, 
        NonTerminal::Factor, SemanticAction::ConstructMultOp, NonTerminal::Term2 }));

    // 63 Term2
    m_rules.push_back(new Rule({ }));

    // 64 Factor
    m_rules.push_back(new Rule({ NonTerminal::VarOrFuncCall }));

    // 65 Factor
    m_rules.push_back(new Rule({ TokenType::IntegerLiteral, 
        SemanticAction::ConstructIntLiteral }));

    // 66 Factor
    m_rules.push_back(new Rule({ TokenType::FloatLiteral, 
        SemanticAction::ConstructFloatLiteral }));

    // 67 Factor
    m_rules.push_back(new Rule({ TokenType::OpenParanthese, 
        NonTerminal::ArithExpr, TokenType::CloseParanthese }));

    // 68 Factor
    m_rules.push_back(new Rule({ TokenType::Not, SemanticAction::PushNot, 
        NonTerminal::Factor, SemanticAction::ConstructModifiedExpr }));

    // 69 Factor
    m_rules.push_back(new Rule({ NonTerminal::Sign, 
        NonTerminal::Factor, SemanticAction::ConstructModifiedExpr }));

    // 70 VarOrFuncCall
    m_rules.push_back(new Rule({ TokenType::ID, SemanticAction::PushID, 
        NonTerminal::VarOrFuncCall2 }));

    // 71 VarOrFuncCall2
    m_rules.push_back(new Rule({ SemanticAction::PushStopNode, NonTerminal::Indice, 
        SemanticAction::ConstructVariable, NonTerminal::VarOrFuncCall3 }));

    // 72 VarOrFuncCall2
    m_rules.push_back(new Rule({ TokenType::OpenParanthese, SemanticAction::PushStopNode,
        NonTerminal::AParams, SemanticAction::ConstructAParams, TokenType::CloseParanthese, 
        SemanticAction::ConstructFuncCall, NonTerminal::VarOrFuncCall3 }));

    // 73 VarOrFuncCall3
    m_rules.push_back(new Rule({ TokenType::Dot, SemanticAction::EncounteredDot, 
        NonTerminal::VarOrFuncCall, SemanticAction::ConstructDotNode }));

    // 74 VarOrFuncCall3
    m_rules.push_back(new Rule({ }));

    // 75 Variable
    m_rules.push_back(new Rule({ TokenType::ID, SemanticAction::PushID, 
        NonTerminal::Variable2 }));

    // 76 Variable2
    m_rules.push_back(new Rule({ SemanticAction::PushStopNode, NonTerminal::Indice, 
        SemanticAction::ConstructVariable, NonTerminal::Variable3 }));

    // 77 Variable2
    m_rules.push_back(new Rule({ TokenType::OpenParanthese, 
        SemanticAction::PushStopNode, NonTerminal::AParams, SemanticAction::ConstructAParams, 
        TokenType::CloseParanthese, SemanticAction::ConstructFuncCall, 
        TokenType::Dot, SemanticAction::EncounteredDot, NonTerminal::Variable, 
        SemanticAction::ConstructDotNode }));

    // 78 Variable3
    m_rules.push_back(new Rule({ TokenType::Dot, SemanticAction::EncounteredDot, 
        NonTerminal::Variable, SemanticAction::ConstructDotNode }));
    
    // 79 Variable3
    m_rules.push_back(new Rule({ }));

    // 80 Indice
    m_rules.push_back(new Rule({ TokenType::OpenSquareBracket, 
        NonTerminal::ArithExpr, TokenType::CloseSquareBracket, NonTerminal::Indice }));

    // 81 Indice
    m_rules.push_back(new Rule({ SemanticAction::ConstructDimensions }));

    // 82 ArraySize
    m_rules.push_back(new Rule({ TokenType::OpenSquareBracket, 
        NonTerminal::ArraySize2 }));

    // 83 ArraySize2
    m_rules.push_back(new Rule({ TokenType::IntegerLiteral, 
        SemanticAction::ConstructIntLiteral, TokenType::CloseSquareBracket }));

    // 84 ArraySize2
    m_rules.push_back(new Rule({ SemanticAction::PushUnspecifiedDimensionNode, 
        TokenType::CloseSquareBracket }));

    // 85 ArraySizeRepetition
    m_rules.push_back(new Rule({ NonTerminal::ArraySize, 
        NonTerminal::ArraySizeRepetition }));

    // 86 ArraySizeRepetition
    m_rules.push_back(new Rule({ SemanticAction::ConstructDimensions }));

    // 87 Type
    m_rules.push_back(new Rule({ TokenType::IntegerKeyword, SemanticAction::PushType }));

    // 88 Type
    m_rules.push_back(new Rule({ TokenType::FloatKeyword, SemanticAction::PushType }));

    // 89 Type
    m_rules.push_back(new Rule({ TokenType::ID, SemanticAction::PushType }));

    // 90 ReturnType
    m_rules.push_back(new Rule({ NonTerminal::Type }));

    // 91 ReturnType
    m_rules.push_back(new Rule({ TokenType::Void, SemanticAction::PushType }));

    // 92 FParams
    m_rules.push_back(new Rule({ TokenType::ID, SemanticAction::PushID, TokenType::Colon, 
        NonTerminal::Type, SemanticAction::PushStopNode, 
        NonTerminal::ArraySizeRepetition, SemanticAction::ConstructFParam, 
        NonTerminal::FParamsTail }));

    // 93 FParams
    m_rules.push_back(new Rule({ }));

    // 94 AParams
    m_rules.push_back(new Rule({ NonTerminal::Expr, NonTerminal::AParamsTail }));

    // 95 AParams
    m_rules.push_back(new Rule({ }));

    // 96 FParamsTail
    m_rules.push_back(new Rule({ TokenType::Comma, 
        TokenType::ID, SemanticAction::PushID, TokenType::Colon, NonTerminal::Type, 
        SemanticAction::PushStopNode, NonTerminal::ArraySizeRepetition, 
        SemanticAction::ConstructFParam, NonTerminal::FParamsTail }));

    // 97 FParamsTail
    m_rules.push_back(new Rule({ }));

    // 98 AParamsTail
    m_rules.push_back(new Rule({ TokenType::Comma, 
        NonTerminal::Expr, NonTerminal::AParamsTail }));

    // 99 AParamsTail
    m_rules.push_back(new Rule({ }));

    // 100 RelOp
    m_rules.push_back(new Rule({ TokenType::Equal, SemanticAction::PushOp }));

    // 101 RelOp
    m_rules.push_back(new Rule({ TokenType::NotEqual, SemanticAction::PushOp }));

    // 102 RelOp
    m_rules.push_back(new Rule({ TokenType::LessThan, SemanticAction::PushOp }));

    // 103 RelOp
    m_rules.push_back(new Rule({ TokenType::GreaterThan, SemanticAction::PushOp }));

    // 104 RelOp
    m_rules.push_back(new Rule({ TokenType::LessOrEqual, SemanticAction::PushOp }));

    // 105 RelOp
    m_rules.push_back(new Rule({ TokenType::GreaterOrEqual, SemanticAction::PushOp }));

    // 106 AddOp
    m_rules.push_back(new Rule({ TokenType::Plus, SemanticAction::PushOp }));

    // 107 AddOp
    m_rules.push_back(new Rule({ TokenType::Minus, SemanticAction::PushOp }));

    // 108 AddOp
    m_rules.push_back(new Rule({ TokenType::Or, SemanticAction::PushOp }));

    // 109 MultOp
    m_rules.push_back(new Rule({ TokenType::Multiply, SemanticAction::PushOp }));

    // 110 MultOp
    m_rules.push_back(new Rule({ TokenType::Divide, SemanticAction::PushOp }));

    // 111 MultOp
    m_rules.push_back(new Rule({ TokenType::And, SemanticAction::PushOp }));
    
    ////////////////////////////////////////////////////////////////////////////
    // Error Cutoff ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    // 112
    m_rules.push_back(new ParsingErrorRule(ErrorID::InvalidTypeSpecifier));
    
    // 113
    m_rules.push_back(new ParsingErrorRule(ErrorID::InvalidFunctionHead));
    
    // 114
    m_rules.push_back(new ParsingErrorRule(ErrorID::InvalidArgumentDefinition));
    
    // 115
    m_rules.push_back(new ParsingErrorRule(ErrorID::InvalidArithExpr));
    
    // 116 
    m_rules.push_back(new ParsingErrorRule(ErrorID::ErroneousTokenAtFuncDef));
    
    // 117
    m_rules.push_back(new ParsingErrorRule(ErrorID::InvalidStatement));

    // 118
    m_rules.push_back(new ParsingErrorRule(ErrorID::InvalidArgumentProvided));

    // 119
    m_rules.push_back(new ParsingErrorRule(ErrorID::InvalidArraySize));

    // 120
    m_rules.push_back(new ParsingErrorRule(ErrorID::InvalidRelExpr));

    // 121
    m_rules.push_back(new ParsingErrorRule(ErrorID::InvalidOperator));
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
        // check if there is an else case
        it = m_entries.find(TokenType::None);
        if (it == m_entries.end())
        {
            return NullRule;
        }
    }
    return it->second;
}

// ErrorData //////////////////////////////////////////////////////////
ParsingErrorData::ParsingErrorData(const Token& token, const StackableItem& top, ErrorID id) 
    : m_token(token), m_top(top), m_id(id) { }

const StackableItem& ParsingErrorData::GetTop() const { return m_top; }
const Token& ParsingErrorData::GetToken() const { return m_token; }
ErrorID ParsingErrorData::GetID() const { return m_id; }

std::ostream& operator<<(std::ostream& stream, const ParsingErrorData& data)
{
    const Token& token = data.GetToken();
    stream << "Unexpected token \"" << token.GetLexeme() << "\". Expected " 
        << data.GetTop() << " at line " << token.GetLine() << ": \"" 
        << token.GetStrOfLine() << "\""; 
    return stream;
}

// ParsingErrorManager ////////////////////////////////////////////////////////

void ParsingErrorManager::WriteErrorToFile(std::ofstream& file, const ParsingErrorData& error)
{
    switch(error.GetID())
    {
    case ErrorID::Default:
        DefaultError(file, error);
        break;

    case ErrorID::InvalidTypeSpecifier:
        InvalidTypeSpecifierError(file, error);
        break;
    
    case ErrorID::InvalidFunctionHead:
        InvalidFunctionHeadError(file, error);
        break;

    case ErrorID::InvalidArgumentDefinition:
        InvalidArgumentDefinitionError(file, error);
        break;

    case ErrorID::InvalidArgumentProvided:
        InvalidArgumentProvidedError(file, error);
        break;

    case ErrorID::InvalidArithExpr:
        InvalidArithExprError(file, error);
        break;

    case ErrorID::ErroneousTokenAtFuncDef:
        ErroneousTokenAtFuncDefError(file, error);
        break;

    case ErrorID::InvalidStatement:
        InvalidStatementError(file, error);
        break;

    case ErrorID::InvalidArraySize:
        InvalidArraySizeError(file, error);
        break;

    case ErrorID::InvalidRelExpr:
        InvalidRelExprError(file, error);
        break;

    case ErrorID::InvalidOperator:
        InvalidOperatorError(file, error);
        break;

    default:
        DEBUG_BREAK();
        break;
    }
    file << "\n";
}

ParsingErrorManager::ParsingErrorManager() { }
ParsingErrorManager::~ParsingErrorManager() { }

void ParsingErrorManager::DefaultError(std::ofstream& file, const ParsingErrorData& error)
{
    const Token& token = error.GetToken();
    file << "Unexpected token \"" << token.GetLexeme() << "\". Expected " 
        << error.GetTop() << " at line " << token.GetLine() << ": \"" 
        << token.GetStrOfLine() << "\""; 
}

void ParsingErrorManager::InvalidTypeSpecifierError(std::ofstream& file, 
    const ParsingErrorData& error)
{
    const Token& token = error.GetToken();
    file << "Invalid type specifier \"" << token.GetLexeme() << "\" found at line " 
        << token.GetLine() << ": \"" << token.GetStrOfLine() << "\"" ; 
}

void ParsingErrorManager::InvalidFunctionHeadError(std::ofstream& file, 
    const ParsingErrorData& error)
{
    const Token& token = error.GetToken();
    file << "Invalid function head found at line " << token.GetLine() 
        << ": \"" << token.GetStrOfLine() << "\". Unexpected token \"" 
        << token.GetLexeme() << "\" encountered"; 
}

void ParsingErrorManager::InvalidArgumentDefinitionError(std::ofstream& file, 
        const ParsingErrorData& error)
{
    const Token& token = error.GetToken();
    file << "Invalid argument definition encountered at line " << token.GetLine() 
        << ": \"" << token.GetStrOfLine() << "\". Unexpected token \"" 
        << token.GetLexeme() << "\" encountered"; 
}

void ParsingErrorManager::InvalidArgumentProvidedError(std::ofstream& file, 
    const ParsingErrorData& error)
{
    const Token& token = error.GetToken();
    file << "Invalid argument provided at line " << token.GetLine() 
        << ": \"" << token.GetStrOfLine() << "\". Unexpected token \"" 
        << token.GetLexeme() << "\" encountered";
}

void ParsingErrorManager::InvalidArithExprError(std::ofstream& file, 
        const ParsingErrorData& error)
{
    const Token& token = error.GetToken();
    file << "Invalid arithmetic expresion provided at line " << token.GetLine() 
        << ": \"" << token.GetStrOfLine() << "\". Unexpected token \"" 
        << token.GetLexeme() << "\" encountered"; 
}

void ParsingErrorManager::ErroneousTokenAtFuncDefError(std::ofstream& file, 
    const ParsingErrorData& error)
{
    const Token& token = error.GetToken();
    file << "Erroneous token found at line " << token.GetLine() 
        << ": \"" << token.GetStrOfLine() << "\". Expected a function definition but found: \""
        << token.GetLexeme() << "\""; 
}

void ParsingErrorManager::InvalidStatementError(std::ofstream& file, 
        const ParsingErrorData& error)
{
    const Token& token = error.GetToken();
    file << "Invalid statement found at line " << token.GetLine() 
        << ": \"" << token.GetStrOfLine() << "\". Unexpected token \"" 
        << token.GetLexeme() << "\" encountered";
}

void ParsingErrorManager::InvalidArraySizeError(std::ofstream& file, 
    const ParsingErrorData& error)
{
    const Token& token = error.GetToken();
    file << "Invalid array size specified at line " << token.GetLine() 
        << ": \"" << token.GetStrOfLine() << "\". Unexpected token \"" 
        << token.GetLexeme() << "\" encountered";
}

void ParsingErrorManager::InvalidRelExprError(std::ofstream& file, 
    const ParsingErrorData& error)
{
    const Token& token = error.GetToken();
    file << "Invalid relation expression provided at line " << token.GetLine() 
        << ": \"" << token.GetStrOfLine() << "\". Unexpected token \"" 
        << token.GetLexeme() << "\" encountered";
}

void ParsingErrorManager::InvalidOperatorError(std::ofstream& file, 
    const ParsingErrorData& error)
{
    const Token& token = error.GetToken();
    file << "Invalid operator \"" << token.GetLexeme() << "\" encountered at line " 
        << token.GetLine() << ": \"" << token.GetStrOfLine() <<"\". The provided operator " 
        << "is either unknown or invalid in this context.";
}

ParsingErrorManager& ParsingErrorManager::GetInstance()
{
    static ParsingErrorManager manager;
    return manager;
}

// Parser ////////////////////////////////////////////
ProgramNode* Parser::Parse(const std::string& filepath)
{
    Reset(filepath);

    Parser& p = GetInstance();
    p.m_parsingStack.push_front(TokenType::EndOfFile);
    p.m_parsingStack.push_front(NonTerminal::Start);

    Token currToken = GetNextToken();
    p.m_currProgramRoot = new ProgramNode();
    
    while (!(p.m_parsingStack.front().GetType() == StackableType::TerminalItem 
        && p.m_parsingStack.front().GetTerminal() == TokenType::EndOfFile))
    {
        const StackableItem& top = p.m_parsingStack.front();

        if (top.GetType() == StackableType::TerminalItem)
        {
            if (currToken.GetTokenType() == top.GetTerminal())
            {
                p.m_parsingStack.pop_front();
                p.m_prevToken = currToken;
                currToken = GetNextToken();
                p.m_derivationFile << "\n"; // skip line since we read a terminal
            }
            else
            {
                // use default error rule
                RuleManager::GetRule(NullRule)->Apply(top, currToken);
                p.m_errorFound = true;
                if (currToken.GetTokenType() == TokenType::EndOfFile)
                {
                    break;
                }
                currToken = p.SkipError(currToken, top);
            }
        }
        else if (top.GetType() == StackableType::NonTerminalItem)
        {
            RuleID rule = p.m_parsingTable[top.GetNonTerminal()]->GetRule(currToken);
            if (!RuleManager::IsError(rule))
            {
                NonTerminal topSymbol = top.GetNonTerminal();
                p.PopNonTerminal();
                RuleManager::GetRule(rule)->Apply(topSymbol, currToken);
                p.WriteDerivationToFile();
            }
            else
            {
                p.m_errorFound = true;
                RuleManager::GetRule(rule)->Apply(top, currToken);
                currToken = p.SkipError(currToken, top);
            }
        }
        else if (top.GetType() == StackableType::SemanticActionItem)
        {
            p.ProcessSemanticAction(top.GetAction());
        }

    }

    p.WriteErrorsToFile();

    if (currToken.GetTokenType() != TokenType::EndOfFile || p.m_errorFound)
    {
        delete p.m_currProgramRoot;
        p.m_currProgramRoot = nullptr;
        return nullptr;
    }

    p.m_astOutFile << p.m_currProgramRoot->ToString();

    // update derivation to remove all epsilon derivations
    p.RemoveNonTerminalsFromDerivation();
    p.WriteDerivationToFile();
    
    ProgramNode* finalProgramRoot = p.m_currProgramRoot;
    p.m_currProgramRoot = nullptr;
    return finalProgramRoot;
}

Parser::Parser() : m_currProgramRoot(nullptr)
{ 
    InitializeParsingTable(); 
}

Parser::~Parser()
{
    for (auto& pair : m_parsingTable)
    {
        delete pair.second;
    }

    for (ASTNode* node : m_semanticStack)
    {
        delete node;
    }
}

Parser& Parser::GetInstance()
{
    static Parser p;
    return p;
}

void Parser::Reset(const std::string& filepath)
{
    Lexer::SetInputFile(filepath);

    Parser& p = GetInstance();
    p.m_parsingStack.clear();
    p.m_errorFound = false;

    std::string simpleName = SimplifyFilename(filepath);

    p.m_derivationFile.close();
    p.m_errorFile.close();
    p.m_astOutFile.close();
    p.m_derivationFile = std::ofstream(simpleName + ".outderivation");
    p.m_errorFile = std::ofstream(simpleName + ".outsyntaxerrors");
    p.m_astOutFile = std::ofstream(simpleName + ".astout");
    p.m_derivationFile << NonTerminal::Start << "\n";

    p.m_errors.clear();
    p.m_derivation.clear();
    p.m_derivation.emplace_back(NonTerminal::Start);
    p.m_derivation.emplace_back(TokenType::EndOfFile);
    p.m_nextNonTerminalIndex = 0;
    p.m_numDotsEncountered = 0;
}

Token Parser::GetNextToken()
{
    Token nextToken = Lexer::GetNextToken();
    while (TokenIsIgnored(nextToken.GetTokenType()) || nextToken.IsError())
    {
        if (nextToken.IsError())
        {
            GetInstance().WriteLexicalErrorToFile(nextToken);
        }
        nextToken = Lexer::GetNextToken();
    }
    return nextToken;
}

bool Parser::TokenIsIgnored(TokenType type)
{
    return type == TokenType::InlineComment 
        || type == TokenType::MultiLineComment;
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
        = new ParsingTableEntry({{TokenType::Function, 21}, {TokenType::None, 113}});

    m_parsingTable[NonTerminal::FuncHead2] 
        = new ParsingTableEntry({{TokenType::OpenParanthese, 23}, {TokenType::Scope, 22}, 
        {TokenType::None, 113}, {TokenType::None, 113}});

    m_parsingTable[NonTerminal::FuncHead3] 
        = new ParsingTableEntry({{TokenType::ID, 24}, {TokenType::Constructor, 25}, 
        {TokenType::None, 113}});

    m_parsingTable[NonTerminal::FuncBody] 
        = new ParsingTableEntry({{TokenType::OpenCurlyBracket, 26}, {TokenType::None, 116}});

    m_parsingTable[NonTerminal::MemberVarDecl] 
        = new ParsingTableEntry({{TokenType::Attribute, 19}});

    m_parsingTable[NonTerminal::LocalVarDeclOrStmtRepetition] 
        = new ParsingTableEntry({{TokenType::ID, 27}, {TokenType::CloseCurlyBracket, 28}, 
        {TokenType::Return, 27}, {TokenType::Write, 27}, {TokenType::Read, 27}, 
        {TokenType::While, 27}, {TokenType::If, 27}, {TokenType::LocalVar, 27}, 
        {TokenType::None, 117}});

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
        = new ParsingTableEntry({{TokenType::ID, 40}, {TokenType::None, 117}});

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
        {TokenType::While, 47}, {TokenType::If, 47}, {TokenType::None, 117}});

    m_parsingTable[NonTerminal::StatBlock] 
        = new ParsingTableEntry({{TokenType::ID, 50}, {TokenType::OpenCurlyBracket, 49}, 
        {TokenType::SemiColon, 51}, {TokenType::Return, 50}, {TokenType::Write, 50}, 
        {TokenType::Read, 50}, {TokenType::While, 50}, {TokenType::Else, 51}, 
        {TokenType::If, 50}});

    m_parsingTable[NonTerminal::Expr] 
        = new ParsingTableEntry({{TokenType::Minus, 52}, {TokenType::Plus, 52}, 
        {TokenType::ID, 52}, {TokenType::IntegerLiteral, 52}, {TokenType::OpenParanthese, 52}, 
        {TokenType::Not, 52}, {TokenType::FloatLiteral, 52}});

    m_parsingTable[NonTerminal::Expr2] 
        = new ParsingTableEntry({{TokenType::GreaterOrEqual, 53}, {TokenType::LessOrEqual, 53}, 
        {TokenType::GreaterThan, 53}, {TokenType::LessThan, 53}, {TokenType::NotEqual, 53}, 
        {TokenType::Equal, 53}, {TokenType::Comma, 54}, {TokenType::CloseParanthese, 54}, 
        {TokenType::SemiColon, 54}, {TokenType::None, 121}});

    m_parsingTable[NonTerminal::RelExpr] 
        = new ParsingTableEntry({{TokenType::Minus, 55}, {TokenType::Plus, 55}, 
        {TokenType::ID, 55}, {TokenType::IntegerLiteral, 55}, {TokenType::OpenParanthese, 55}, 
        {TokenType::Not, 55}, {TokenType::FloatLiteral, 55}, {TokenType::None, 120}});

    m_parsingTable[NonTerminal::ArithExpr] 
        = new ParsingTableEntry({{TokenType::Minus, 56}, {TokenType::Plus, 56}, 
        {TokenType::ID, 56}, {TokenType::IntegerLiteral, 56}, {TokenType::OpenParanthese, 56}, 
        {TokenType::Not, 56}, {TokenType::FloatLiteral, 56}, {TokenType::None, 115}});

    m_parsingTable[NonTerminal::ArithExpr2] 
        = new ParsingTableEntry({{TokenType::Or, 57}, {TokenType::Minus, 57}, 
        {TokenType::Plus, 57}, {TokenType::GreaterOrEqual, 58}, {TokenType::LessOrEqual, 58}, 
        {TokenType::GreaterThan, 58}, {TokenType::LessThan, 58}, {TokenType::NotEqual, 58}, 
        {TokenType::Equal, 58}, {TokenType::Comma, 58}, {TokenType::CloseSquareBracket, 58}, 
        {TokenType::CloseParanthese, 58}, {TokenType::SemiColon, 58}, 
        {TokenType::None, 121}});   

    m_parsingTable[NonTerminal::Sign] 
        = new ParsingTableEntry({{TokenType::Minus, 60}, {TokenType::Plus, 59}});

    m_parsingTable[NonTerminal::Term] 
        = new ParsingTableEntry({{TokenType::Minus, 61}, {TokenType::Plus, 61}, 
        {TokenType::ID, 61}, {TokenType::IntegerLiteral, 61}, {TokenType::OpenParanthese, 61}, 
        {TokenType::Not, 61}, {TokenType::FloatLiteral, 61}}); 

    m_parsingTable[NonTerminal::Term2] 
        = new ParsingTableEntry({{TokenType::And, 62}, {TokenType::Divide, 62}, 
        {TokenType::Multiply, 62}, {TokenType::Or, 63}, {TokenType::Minus, 63}, 
        {TokenType::Plus, 63}, {TokenType::GreaterOrEqual, 63}, {TokenType::LessOrEqual, 63}, 
        {TokenType::GreaterThan, 63}, {TokenType::LessThan, 63}, {TokenType::NotEqual, 63}, 
        {TokenType::Equal, 63}, {TokenType::Comma, 63}, {TokenType::CloseSquareBracket, 63}, 
        {TokenType::CloseParanthese, 63}, {TokenType::SemiColon, 63}, 
        {TokenType::None, 121}});

    m_parsingTable[NonTerminal::Factor] 
        = new ParsingTableEntry({{TokenType::Minus, 69}, {TokenType::Plus, 69}, 
        {TokenType::ID, 64}, {TokenType::IntegerLiteral, 65}, {TokenType::OpenParanthese, 67}, 
        {TokenType::Not, 68}, {TokenType::FloatLiteral, 66}});

    m_parsingTable[NonTerminal::VarOrFuncCall] 
        = new ParsingTableEntry({{TokenType::ID, 70}});

    m_parsingTable[NonTerminal::VarOrFuncCall2] 
        = new ParsingTableEntry({{TokenType::And, 71}, {TokenType::Divide, 71}, 
        {TokenType::Multiply, 71}, {TokenType::Or, 71}, {TokenType::Minus, 71}, 
        {TokenType::Plus, 71}, {TokenType::GreaterOrEqual, 71}, {TokenType::LessOrEqual, 71}, 
        {TokenType::GreaterThan, 71}, {TokenType::LessThan, 71}, {TokenType::NotEqual, 71}, 
        {TokenType::Equal, 71}, {TokenType::Comma, 71}, {TokenType::CloseSquareBracket, 71}, 
        {TokenType::OpenSquareBracket, 71}, {TokenType::Dot, 71}, 
        {TokenType::CloseParanthese, 71}, {TokenType::OpenParanthese, 72}, 
        {TokenType::SemiColon, 71}});

    m_parsingTable[NonTerminal::VarOrFuncCall3] 
        = new ParsingTableEntry({{TokenType::And, 74}, {TokenType::Divide, 74}, 
        {TokenType::Multiply, 74}, {TokenType::Or, 74}, {TokenType::Minus, 74}, 
        {TokenType::Plus, 74}, {TokenType::GreaterOrEqual, 74}, {TokenType::LessOrEqual, 74}, 
        {TokenType::GreaterThan, 74}, {TokenType::LessThan, 74}, {TokenType::NotEqual, 74}, 
        {TokenType::Equal, 74}, {TokenType::Comma, 74}, {TokenType::CloseSquareBracket, 74}, 
        {TokenType::Dot, 73}, {TokenType::CloseParanthese, 74}, {TokenType::SemiColon, 74}});

    m_parsingTable[NonTerminal::Variable] 
        = new ParsingTableEntry({{TokenType::ID, 75}});

    m_parsingTable[NonTerminal::Variable2] 
        = new ParsingTableEntry({{TokenType::OpenSquareBracket, 76}, {TokenType::Dot, 76}, 
        {TokenType::CloseParanthese, 76}, {TokenType::OpenParanthese, 77}});

    m_parsingTable[NonTerminal::Variable3] 
        = new ParsingTableEntry({{TokenType::Dot, 78}, {TokenType::CloseParanthese, 79}});

    m_parsingTable[NonTerminal::Indice] 
        = new ParsingTableEntry({{TokenType::And, 81}, {TokenType::Divide, 81}, 
        {TokenType::Multiply, 81}, {TokenType::Or, 81}, {TokenType::Minus, 81}, 
        {TokenType::Plus, 81}, {TokenType::GreaterOrEqual, 81}, {TokenType::LessOrEqual, 81}, 
        {TokenType::GreaterThan, 81}, {TokenType::LessThan, 81}, {TokenType::NotEqual, 81}, 
        {TokenType::Equal, 81}, {TokenType::Comma, 81}, {TokenType::CloseSquareBracket, 81}, 
        {TokenType::OpenSquareBracket, 80}, {TokenType::Dot, 81}, 
        {TokenType::CloseParanthese, 81}, {TokenType::Assign, 81}, 
        {TokenType::SemiColon, 81}});

    m_parsingTable[NonTerminal::ArraySize] 
        = new ParsingTableEntry({{TokenType::OpenSquareBracket, 82}, {TokenType::None, 119}});

    m_parsingTable[NonTerminal::ArraySize2] 
        = new ParsingTableEntry({{TokenType::CloseSquareBracket, 84}, 
        {TokenType::IntegerLiteral, 83}, {TokenType::None, 119}});

    m_parsingTable[NonTerminal::ArraySizeRepetition] 
        = new ParsingTableEntry({{TokenType::Comma, 86}, {TokenType::OpenSquareBracket, 85}, 
        {TokenType::CloseParanthese, 86}, {TokenType::SemiColon, 86}, {TokenType::None, 119}});

    m_parsingTable[NonTerminal::Type] 
        = new ParsingTableEntry({{TokenType::ID, 89}, {TokenType::FloatKeyword, 88}, 
        {TokenType::IntegerKeyword, 87}, {TokenType::None, 112}});

    m_parsingTable[NonTerminal::ReturnType] 
        = new ParsingTableEntry({{TokenType::ID, 90}, {TokenType::Void, 91}, 
        {TokenType::FloatKeyword, 90}, {TokenType::IntegerKeyword, 90}, 
        {TokenType::None, 112}});

    m_parsingTable[NonTerminal::FParams] 
        = new ParsingTableEntry({{TokenType::ID, 92}, {TokenType::CloseParanthese, 93}, 
        {TokenType::None, 114}});

    m_parsingTable[NonTerminal::AParams] 
        = new ParsingTableEntry({{TokenType::Minus, 94}, {TokenType::Plus, 94}, 
        {TokenType::ID, 94}, {TokenType::IntegerLiteral, 94}, {TokenType::CloseParanthese, 95}, 
        {TokenType::OpenParanthese, 94}, {TokenType::Not, 94}, {TokenType::FloatLiteral, 94}, 
        {TokenType::None, 118}});

    m_parsingTable[NonTerminal::FParamsTail] 
        = new ParsingTableEntry({{TokenType::Comma, 96}, {TokenType::CloseParanthese, 97}, 
        {TokenType::None, 112}});
    
    m_parsingTable[NonTerminal::AParamsTail] 
        = new ParsingTableEntry({{TokenType::Comma, 98}, {TokenType::CloseParanthese, 99}, 
        {TokenType::None, 118}});

    m_parsingTable[NonTerminal::RelOp] 
        = new ParsingTableEntry({{TokenType::GreaterOrEqual, 105}, 
        {TokenType::LessOrEqual, 104}, {TokenType::GreaterThan, 103},
        {TokenType::LessThan, 102}, {TokenType::NotEqual, 101}, {TokenType::Equal, 100}, 
        {TokenType::None, 121}});

    m_parsingTable[NonTerminal::AddOp] 
        = new ParsingTableEntry({{TokenType::Or, 108}, {TokenType::Minus, 107}, 
        {TokenType::Plus, 106}, {TokenType::None, 121}});

    m_parsingTable[NonTerminal::MultOp] 
        = new ParsingTableEntry({{TokenType::And, 111}, {TokenType::Divide, 110}, 
        {TokenType::Multiply, 109}, {TokenType::None, 121}});
}

void Parser::PushToStack(const Rule* r)
{
    const std::list<StackableItem>& rightSide = r->GetRightSide();
    for (auto it = rightSide.rbegin(); it != rightSide.rend(); it++)
    {
        m_parsingStack.push_front(*it);    
    }

    size_t shift = 0;
    for (const StackableItem& item : rightSide)
    {
        m_derivation.insert(m_derivation.begin() + m_nextNonTerminalIndex + shift, item);
        shift++;
    }

    if (m_nextNonTerminalIndex >= shift)
    {
        m_nextNonTerminalIndex -= shift;
    }
    else
    {
        m_nextNonTerminalIndex = 0;
    }
    UpdateNextNonTerminalIndex();
}

void Parser::WriteDerivationToFile()
{
    m_derivationFile << "-> ";
    for (StackableItem& item : m_derivation)
    {
        m_derivationFile << item << " ";
    }
    m_derivationFile << "\n";
}

void Parser::RemoveNonTerminalsFromDerivation()
{
    for (size_t i = m_nextNonTerminalIndex; i < m_derivation.size(); i++)
    {
        if (m_derivation[i].GetType() == StackableType::NonTerminalItem)
        {
            m_derivation.erase(m_derivation.begin() + i);
            i--;
        }
    }
}

void Parser::WriteErrorsToFile()
{
    for (auto it = m_errors.rbegin(); it != m_errors.rend(); it++)
    {
        ParsingErrorManager::WriteErrorToFile(m_errorFile, *it);
    }
}

Token Parser::SkipError(const Token& currToken, const StackableItem& top)
{
    if (currToken.GetTokenType() == TokenType::EndOfFile 
        || SetManager::IsInFollowSet(top, currToken.GetTokenType()))
    {
        if (top.GetType() == StackableType::NonTerminalItem)
        {
            PopNonTerminal();
        }
        else
        {
            m_parsingStack.pop_front();
        }
    }
    else
    {
        Token nextToken = currToken;
        while((!SetManager::IsInFirstSet(top, nextToken.GetTokenType()) 
            && (!SetManager::IsInFirstSet(top, TokenType::None) 
            || !SetManager::IsInFollowSet(top, nextToken.GetTokenType()))) 
            && nextToken.GetTokenType() != TokenType::EndOfFile)
        {
            nextToken = GetNextToken();
        }
        return nextToken;
    }
    return currToken;
}

void Parser::PopNonTerminal()
{
    m_parsingStack.pop_front();
    m_derivation.erase(m_derivation.begin() + m_nextNonTerminalIndex);
}

void Parser::ProcessSemanticAction(SemanticAction action)
{
    ASSERT(m_parsingStack.front().GetType() == StackableType::SemanticActionItem);
    m_parsingStack.pop_front();

    // do not process semantic actions for invalid programs
    if (m_errorFound)
    {
        return;
    }

    switch(action)
    {
    case SemanticAction::PushStopNode:
        Push<StopNode>(); 
        break;

    case SemanticAction::PushUnspecifiedDimensionNode:
        Push<UnspecificedDimensionNode>(); 
        break;

    case SemanticAction::ConstructIntLiteral:
        ConstructIntLiteralAction();
        break;

    case SemanticAction::ConstructFloatLiteral:
        ConstructFloatLiteralAction();
        break;

    case SemanticAction::ConstructVisibility:
        ConstructVisibilityAction();
        break;

    case SemanticAction::ConstructDefaultVisibility:
        ConstructDefaultVisibilityAction();
        break;

    case SemanticAction::PushID:
        Push<IDNode>(m_prevToken); 
        break;

    case SemanticAction::PushOp:
        Push<OperatorNode>(m_prevToken); 
        break;

    case SemanticAction::PushSign:
        Push<SignNode>(m_prevToken); 
        break;

    case SemanticAction::PushNot:
        Push<NotNode>(); 
        break;

    case SemanticAction::PushType:
        Push<TypeNode>(m_prevToken); 
        break;

    case SemanticAction::PushFreeFuncMarker:
        Push<FreeFuncMarkerNode>(); 
        break;

    case SemanticAction::PushMemFuncMarker:
        Push<MemFuncMarkerNode>(); 
        break;

    case SemanticAction::PushConstructorMarker:
        Push<ConstructorMarkerNode>(); 
        break;

    case SemanticAction::EncounteredDot:
        m_numDotsEncountered++;
        break;

    case SemanticAction::ConstructAssignStat:
        ConstructAssignStatAction();
        break;

    case SemanticAction::ConstructExpr:
        ConstructExprAction();
        break;

    case SemanticAction::ConstructModifiedExpr:
        ConstructModifiedExprAction();
        break;

    case SemanticAction::ConstructAddOp:
        ConstructBinaryOperatorNode<AddOpNode>();
        break;

    case SemanticAction::ConstructMultOp:
        ConstructBinaryOperatorNode<MultOpNode>();
        break;

    case SemanticAction::ConstructRelOp:
        ConstructBinaryOperatorNode<RelOpNode>();
        break;

    case SemanticAction::ConstructDimensions:
        ConstructLoopingNode<DimensionNode>();
        break;

    case SemanticAction::ConstructVariable:
        ConstructVariableAction();
        break;

    case SemanticAction::ConstructVarDecl:
        ConstructVarDeclAction();
        break;

    case SemanticAction::ConstructStatBlock:
        ConstructLoopingNode<StatBlockNode>();
        break;

    case SemanticAction::ConstructFParam:
        ConstructFParamAction();
        break;

    case SemanticAction::ConstructFParams:
        ConstructLoopingNode<FParamListNode>();
        break;

    case SemanticAction::ConstructAParams:
        ConstructLoopingNode<AParamListNode>();
        break;

    case SemanticAction::ConstructFuncDef:
        ConstructFuncDefAction();
        break;

    case SemanticAction::ConstructFuncCall:
        ConstructFuncCallAction();
        break;

    case SemanticAction::ConstructIfStat:
        ConstructIfStatAction();
        break;

    case SemanticAction::ConstructWhileStat:
        ConstructWhileStatAction();
        break;

    case SemanticAction::ConstructReadStat:
        ConstructReadStatAction();
        break;

    case SemanticAction::ConstructWriteStat:
        ConstructWriteStatAction();
        break;

    case SemanticAction::ConstructReturnStat:
        ConstructReturnStatAction();
        break;

    case SemanticAction::ConstructClass:
        ConstructClassAction();
        break;

    case SemanticAction::ConstructMemVar:
        ConstructMemVarAction();
        break;

    case SemanticAction::ConstructMemFuncDecl:
        ConstructMemFuncDeclAction();
        break;

    case SemanticAction::ConstructConstructorDecl:
        ConstructConstructorDeclAction();
        break;

    case SemanticAction::ConstructDotNode:
        if (ConstructDotNodeAction())
        {
            m_numDotsEncountered--;
        }
        break;

    case SemanticAction::ConstructEncounteredDots:
        ConstructEncounteredDotsAction();
        break;

    case SemanticAction::ConstructInheritanceList:
        ConstructLoopingNode<InheritanceListNode>();
        break;

    default:
        DEBUG_BREAK();
        break;
    }
}

void Parser::ConstructIntLiteralAction()
{
    m_semanticStack.push_front(new LiteralNode(new IDNode(m_prevToken), 
        new TypeNode(Token("integer",TokenType::IntegerKeyword, SIZE_MAX))));
}

void Parser::ConstructFloatLiteralAction()
{
    m_semanticStack.push_front(new LiteralNode(new IDNode(m_prevToken), 
        new TypeNode(Token("float", TokenType::FloatKeyword, SIZE_MAX))));
}

void Parser::ConstructVisibilityAction()
{
    m_semanticStack.push_front(new VisibilityNode(m_prevToken.GetLexeme()));
}

void Parser::ConstructDefaultVisibilityAction()
{
    m_semanticStack.push_front(new DefaultVisibilityNode());
}

void Parser::ConstructExprAction()
{
    ASTNode* topNode = m_semanticStack.front();
    m_semanticStack.pop_front();
    m_semanticStack.push_front(new ExprNode(topNode));
}

void Parser::ConstructModifiedExprAction()
{
    ASTNode* expr = m_semanticStack.front();
    m_semanticStack.pop_front();

    ASTNode* modifier = m_semanticStack.front();
    m_semanticStack.pop_front();

    m_semanticStack.push_front(new ModifiedExpr(modifier, expr));
}

void Parser::ConstructAssignStatAction()
{
    ExprNode* right = PopTargetNodeFromSemanticStack<ExprNode>();

    ASTNode* left = m_semanticStack.front();
    m_semanticStack.pop_front();

    m_semanticStack.push_front(new AssignStatNode(left, right));
}

void Parser::ConstructVariableAction()
{
    DimensionNode* dim = PopTargetNodeFromSemanticStack<DimensionNode>();
    IDNode* id = PopTargetNodeFromSemanticStack<IDNode>();
    m_semanticStack.push_front(new VariableNode(id, dim));
}

void Parser::ConstructVarDeclAction()
{
    ASTNode* lastNode = m_semanticStack.front();
    m_semanticStack.pop_front();

    TypeNode* type = PopTargetNodeFromSemanticStack<TypeNode>();
    IDNode* id = PopTargetNodeFromSemanticStack<IDNode>();

    if (dynamic_cast<DimensionNode*>(lastNode) != nullptr)
    {
        m_semanticStack.push_front(new VarDeclNode(id, type, (DimensionNode*)lastNode));
    }
    else if (dynamic_cast<AParamListNode*>(lastNode) != nullptr)
    {
        m_semanticStack.push_front(new VarDeclNode(id, type, (AParamListNode*)lastNode));
    }
    else
    {
        DEBUG_BREAK();
    }
}

void Parser::ConstructFParamAction()
{
    DimensionNode* dimension = PopTargetNodeFromSemanticStack<DimensionNode>();
    TypeNode* type = PopTargetNodeFromSemanticStack<TypeNode>();
    IDNode* id = PopTargetNodeFromSemanticStack<IDNode>();

    m_semanticStack.push_front(new FParamNode(id, type, dimension));
}

void Parser::ConstructFuncDefAction()
{
    StatBlockNode* body = PopTargetNodeFromSemanticStack<StatBlockNode>();
    ASTNode* marker = m_semanticStack.front();
    m_semanticStack.pop_front();
    
    if (dynamic_cast<FreeFuncMarkerNode*>(marker) != nullptr)
    {
        ConstructFreeFuncDefAction(body);
    }
    else if (dynamic_cast<MemFuncMarkerNode*>(marker) != nullptr)
    {
        ConstructMemFuncDefAction(body);
    }
    else if (dynamic_cast<ConstructorMarkerNode*>(marker) != nullptr)
    {
        ConstructConstructorDefAction(body);
    }
    else
    {
        DEBUG_BREAK();
    }
    delete marker;
}

void Parser::ConstructFreeFuncDefAction(StatBlockNode* body)
{
    TypeNode* returnType = PopTargetNodeFromSemanticStack<TypeNode>();
    FParamListNode* params = PopTargetNodeFromSemanticStack<FParamListNode>();
    IDNode* id = PopTargetNodeFromSemanticStack<IDNode>();
    m_currProgramRoot->GetFunctionList()->AddFunc(new FunctionDefNode(id, returnType, 
        params, body));
}

void Parser::ConstructMemFuncDefAction(StatBlockNode* body)
{
    TypeNode* returnType = PopTargetNodeFromSemanticStack<TypeNode>();
    FParamListNode* params = PopTargetNodeFromSemanticStack<FParamListNode>();
    IDNode* id = PopTargetNodeFromSemanticStack<IDNode>();
    IDNode* classID = PopTargetNodeFromSemanticStack<IDNode>();

    m_currProgramRoot->GetFunctionList()->AddFunc(new MemFuncDefNode(classID, id, 
        returnType, params, body));
}

void Parser::ConstructConstructorDefAction(StatBlockNode* body)
{
    FParamListNode* params = PopTargetNodeFromSemanticStack<FParamListNode>();
    IDNode* classID = PopTargetNodeFromSemanticStack<IDNode>();
    m_currProgramRoot->GetFunctionList()->AddFunc(
        new ConstructorDefNode(classID, params, body));
}

void Parser::ConstructFuncCallAction()
{
    AParamListNode* aparam = PopTargetNodeFromSemanticStack<AParamListNode>();
    IDNode* id = PopTargetNodeFromSemanticStack<IDNode>();
    m_semanticStack.push_front(new FuncCallNode(id, aparam));
}

void Parser::ConstructIfStatAction()
{
    StatBlockNode* elseBlock = PopTargetNodeFromSemanticStack<StatBlockNode>();
    StatBlockNode* ifBlock = PopTargetNodeFromSemanticStack<StatBlockNode>();
    ExprNode* condition = PopTargetNodeFromSemanticStack<ExprNode>();
    m_semanticStack.push_front(new IfStatNode(condition, ifBlock, elseBlock));
}

void Parser::ConstructWhileStatAction()
{
    StatBlockNode* loopBlock = PopTargetNodeFromSemanticStack<StatBlockNode>();
    ExprNode* condition = PopTargetNodeFromSemanticStack<ExprNode>();
    m_semanticStack.push_front(new WhileStatNode(condition, loopBlock));
}

void Parser::ConstructReadStatAction()
{
    ASTNode* var = m_semanticStack.front();
    m_semanticStack.pop_front();
    m_semanticStack.push_front(new ReadStatNode(var));
}

void Parser::ConstructWriteStatAction()
{
    ExprNode* expr = PopTargetNodeFromSemanticStack<ExprNode>();
    m_semanticStack.push_front(new WriteStatNode(expr));
}

void Parser::ConstructReturnStatAction()
{
    ExprNode* expr = PopTargetNodeFromSemanticStack<ExprNode>();
    m_semanticStack.push_front(new ReturnStatNode(expr));
}

void Parser::ConstructClassAction()
{
    std::list<MemVarNode*> memVar;
    std::list<ConstructorDeclNode*> constructors;
    std::list<MemFuncDeclNode*> memFunc;

    ASTNode* top = m_semanticStack.front();
    m_semanticStack.pop_front();
    while(dynamic_cast<StopNode*>(top) == nullptr)
    {
        if (dynamic_cast<MemVarNode*>(top) != nullptr)
        {
            memVar.push_back((MemVarNode*)top);
        }
        else if (dynamic_cast<ConstructorDeclNode*>(top) != nullptr)
        {
            constructors.push_back((ConstructorDeclNode*)top);
        }
        else if (dynamic_cast<MemFuncDeclNode*>(top) != nullptr)
        {
            memFunc.push_back((MemFuncDeclNode*)top);
        }
        else
        {
            DEBUG_BREAK();
        }

        top = m_semanticStack.front();
        m_semanticStack.pop_front();
    }
    delete top;

    InheritanceListNode* inheritanceList = PopTargetNodeFromSemanticStack
        <InheritanceListNode>();
    IDNode* id = PopTargetNodeFromSemanticStack<IDNode>();
    ClassDefNode* classDef = new ClassDefNode(id, inheritanceList);

    for (MemVarNode* var : memVar)
    {
        classDef->AddVarDecl(var);
    }

    for (ConstructorDeclNode* constructor : constructors)
    {
        classDef->AddConstructor(constructor);
    }

    for (MemFuncDeclNode* funcDecl : memFunc)
    {
        classDef->AddFuncDecl(funcDecl);
    }

    m_currProgramRoot->GetClassList()->AddClass(classDef);
}

void Parser::ConstructMemVarAction()
{
    DimensionNode* dimension = PopTargetNodeFromSemanticStack<DimensionNode>();
    TypeNode* type = PopTargetNodeFromSemanticStack<TypeNode>();
    IDNode* id = PopTargetNodeFromSemanticStack<IDNode>();
    VisibilityNode* visibility = PopTargetNodeFromSemanticStack<VisibilityNode>();

    m_semanticStack.push_front(new MemVarNode(visibility, id, type, dimension));
}

void Parser::ConstructMemFuncDeclAction()
{
    TypeNode* returnType = PopTargetNodeFromSemanticStack<TypeNode>();
    FParamListNode* params = PopTargetNodeFromSemanticStack<FParamListNode>();
    IDNode* id = PopTargetNodeFromSemanticStack<IDNode>();
    VisibilityNode* visibility = PopTargetNodeFromSemanticStack<VisibilityNode>();

    m_semanticStack.push_front(new MemFuncDeclNode(visibility, id, returnType, params));
}

void Parser::ConstructConstructorDeclAction()
{
    FParamListNode* params = PopTargetNodeFromSemanticStack<FParamListNode>();
    VisibilityNode* visibility = PopTargetNodeFromSemanticStack<VisibilityNode>();
    m_semanticStack.push_front(new ConstructorDeclNode(visibility, params));
}

bool Parser::ConstructDotNodeAction()
{
    if (m_numDotsEncountered == 0)
    {
        return false;
    }

    ASTNode* right = m_semanticStack.front();
    m_semanticStack.pop_front();

    ASTNode* left = m_semanticStack.front();
    m_semanticStack.pop_front();

    m_semanticStack.push_front(new DotNode(left, right));
    return true;
}

void Parser::ConstructEncounteredDotsAction()
{
    while (m_numDotsEncountered > 0)
    {
        ConstructDotNodeAction();
        m_numDotsEncountered--;
    }
}

void Parser::UpdateNextNonTerminalIndex()
{
    for (size_t i = m_nextNonTerminalIndex; i < m_derivation.size(); i++)
    {
        if (m_derivation[i].GetType() == StackableType::NonTerminalItem)
        {
            m_nextNonTerminalIndex = i;
            break;
        }
    }
}

void Parser::WriteLexicalErrorToFile(const Token& t)
{
    m_errorFound = true;
    m_errorFile << "Lexical error: " << t.GetTokenType() << ": \""
	    << t.GetLexeme() << "\": line  " << t.GetLine() << ".\n";
}