#include "Parser.h"
#include "../Core/Token.h"
#include "../Lexer/Lexer.h"
#include "../Core/Core.h"
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
        { NonTerminal::ClassDeclOrFuncDef, NonTerminal::ClassDeclOrFuncDefRepetition }));

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

// ErrorData //////////////////////////////////////////////////////////
ErrorData::ErrorData(const Token& token, StackableItem top) : m_token(token), m_top(top) { }

const StackableItem& ErrorData::GetTop() const { return m_top; }
const Token& ErrorData::GetToken() const { return m_token; }

std::ostream& operator<<(std::ostream& stream, const ErrorData& data)
{
    const Token& token = data.GetToken();
    stream << "Unexpected token \"" << token.GetLexeme() << "\". Expected " 
        << data.GetTop() << " at line " << token.GetLine() << ": \"" 
        << token.GetStrOfLine() << "\""; 
    return stream;
}

// Parser ////////////////////////////////////////////
bool Parser::Parse(const std::string& filepath)
{
    Reset(filepath);

    Parser& p = GetInstance();
    p.m_parsingStack.push_front(TokenType::EndOfFile);
    p.m_parsingStack.push_front(NonTerminal::Start);

    Token currToken = GetNextToken();
    
    while (!(p.m_parsingStack.front().GetType() == StackableType::TerminalItem 
        && p.m_parsingStack.front().GetTerminal() == TokenType::EndOfFile)
        && currToken.GetTokenType() != TokenType::EndOfFile)
    {
        const StackableItem& top = p.m_parsingStack.front();
        if (top.GetType() == StackableType::TerminalItem)
        {
            if (currToken.GetTokenType() == top.GetTerminal())
            {
                p.m_parsingStack.pop_front();
                currToken = GetNextToken();
                p.m_derivationFile << "\n"; // skip line since we read a terminal
            }
            else
            {
                p.m_errorFound = true;
                currToken = p.SkipError(currToken, top);
            }
        }
        else if (top.GetType() == StackableType::NonTerminalItem)
        {
            RuleID rule = p.m_parsingTable[top.GetNonTerminal()]->GetRule(currToken);
            if (rule != NullRule)
            {
                p.PopNonTerminal();
                
                p.PushToStack(RuleManager::GetRule(rule));
                p.WriteDerivationToFile();
            }
            else
            {
                p.m_errorFound = true;
                currToken = p.SkipError(currToken, top);
            }
        }

    }

    p.WriteErrorsToFile();

    if (currToken.GetTokenType() != TokenType::EndOfFile || p.m_errorFound)
    {
        return false;
    }
    return true;
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

void Parser::Reset(const std::string& filepath)
{
    Lexer::SetInputFile(filepath);

    Parser& p = GetInstance();
    p.m_parsingStack.clear();
    p.m_errorFound = false;

    std::string simpleName = SimplifyFilename(filepath);
    p.m_derivationFile = std::ofstream(simpleName + ".outderivation");
    p.m_errorFile = std::ofstream(simpleName + ".outsyntaxerrors");
    p.m_derivationFile << NonTerminal::Start << "\n";

    p.m_derivation.clear();
    p.m_derivation.emplace_back(NonTerminal::Start);
    p.m_derivation.emplace_back(TokenType::EndOfFile);
    p.m_nextNonTerminalIndex = 0;
}

Token Parser::GetNextToken()
{
    Token nextToken = Lexer::GetNextToken();
    while (TokenIsIgnored(nextToken.GetTokenType()) || nextToken.IsError())
    {
        if (nextToken.IsError())
        {
            GetInstance().m_errorFound = true;
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

    m_parsingTable[NonTerminal::Expr] 
        = new ParsingTableEntry({{TokenType::Minus, 52}, {TokenType::Plus, 52}, 
        {TokenType::ID, 52}, {TokenType::IntegerLiteral, 52}, {TokenType::OpenParanthese, 52}, 
        {TokenType::Not, 52}, {TokenType::FloatLiteral, 52}});

    m_parsingTable[NonTerminal::Expr2] 
        = new ParsingTableEntry({{TokenType::GreaterOrEqual, 53}, {TokenType::LessOrEqual, 53}, 
        {TokenType::GreaterThan, 53}, {TokenType::LessThan, 53}, {TokenType::NotEqual, 53}, 
        {TokenType::Equal, 53}, {TokenType::Comma, 54}, {TokenType::CloseParanthese, 54}, 
        {TokenType::SemiColon, 54}});

    m_parsingTable[NonTerminal::RelExpr] 
        = new ParsingTableEntry({{TokenType::Minus, 55}, {TokenType::Plus, 55}, 
        {TokenType::ID, 55}, {TokenType::IntegerLiteral, 55}, {TokenType::OpenParanthese, 55}, 
        {TokenType::Not, 55}, {TokenType::FloatLiteral, 55}});

    m_parsingTable[NonTerminal::ArithExpr] 
        = new ParsingTableEntry({{TokenType::Minus, 56}, {TokenType::Plus, 56}, 
        {TokenType::ID, 56}, {TokenType::IntegerLiteral, 56}, {TokenType::OpenParanthese, 56}, 
        {TokenType::Not, 56}, {TokenType::FloatLiteral, 56}});

    m_parsingTable[NonTerminal::ArithExpr2] 
        = new ParsingTableEntry({{TokenType::Or, 57}, {TokenType::Minus, 57}, 
        {TokenType::Plus, 57}, {TokenType::GreaterOrEqual, 58}, {TokenType::LessOrEqual, 58}, 
        {TokenType::GreaterThan, 58}, {TokenType::LessThan, 58}, {TokenType::NotEqual, 58}, 
        {TokenType::Equal, 58}, {TokenType::Comma, 58}, {TokenType::CloseSquareBracket, 58}, 
        {TokenType::CloseParanthese, 58}, {TokenType::SemiColon, 58}});   

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
        {TokenType::CloseParanthese, 63}, {TokenType::SemiColon, 63}});

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
        = new ParsingTableEntry({{TokenType::OpenSquareBracket, 82}});

    m_parsingTable[NonTerminal::ArraySize2] 
        = new ParsingTableEntry({{TokenType::CloseSquareBracket, 84}, 
        {TokenType::IntegerLiteral, 83}});

    m_parsingTable[NonTerminal::ArraySizeRepetition] 
        = new ParsingTableEntry({{TokenType::Comma, 86}, {TokenType::OpenSquareBracket, 85}, 
        {TokenType::CloseParanthese, 86}, {TokenType::SemiColon, 86}});

    m_parsingTable[NonTerminal::Type] 
        = new ParsingTableEntry({{TokenType::ID, 89}, {TokenType::FloatKeyword, 88}, 
        {TokenType::IntegerKeyword, 87}});

    m_parsingTable[NonTerminal::ReturnType] 
        = new ParsingTableEntry({{TokenType::ID, 90}, {TokenType::Void, 91}, 
        {TokenType::FloatKeyword, 90}, {TokenType::IntegerKeyword, 90}});

    m_parsingTable[NonTerminal::FParams] 
        = new ParsingTableEntry({{TokenType::ID, 92}, {TokenType::CloseParanthese, 93}});

    m_parsingTable[NonTerminal::AParams] 
        = new ParsingTableEntry({{TokenType::Minus, 94}, {TokenType::Plus, 94}, 
        {TokenType::ID, 94}, {TokenType::IntegerLiteral, 94}, {TokenType::CloseParanthese, 95}, 
        {TokenType::OpenParanthese, 94}, {TokenType::Not, 94}, {TokenType::FloatLiteral, 94}});

    m_parsingTable[NonTerminal::FParamsTail] 
        = new ParsingTableEntry({{TokenType::Comma, 96}, {TokenType::CloseParanthese, 97}});
    
    m_parsingTable[NonTerminal::AParamsTail] 
        = new ParsingTableEntry({{TokenType::Comma, 98}, {TokenType::CloseParanthese, 99}});

    m_parsingTable[NonTerminal::RelOp] 
        = new ParsingTableEntry({{TokenType::GreaterOrEqual, 105}, 
        {TokenType::LessOrEqual, 104}, {TokenType::GreaterThan, 103},
        {TokenType::LessThan, 102}, {TokenType::NotEqual, 101}, {TokenType::Equal, 100}});

    m_parsingTable[NonTerminal::AddOp] 
        = new ParsingTableEntry({{TokenType::Or, 108}, {TokenType::Minus, 107}, 
        {TokenType::Plus, 106}});

    m_parsingTable[NonTerminal::MultOp] 
        = new ParsingTableEntry({{TokenType::And, 111}, {TokenType::Divide, 110}, 
        {TokenType::Multiply, 109}});
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

void Parser::WriteErrorsToFile()
{
    for (auto it = m_errors.rbegin(); it != m_errors.rend(); it++)
    {
        m_errorFile << *it << "\n";
    }
}

Token Parser::SkipError(const Token& currToken, const StackableItem& top)
{
    m_errors.emplace_front(currToken, top);
    
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
            || (SetManager::IsInFirstSet(top, TokenType::None) 
            && !SetManager::IsInFollowSet(top, nextToken.GetTokenType()))) 
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