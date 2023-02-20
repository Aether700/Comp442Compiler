#include "AST.h"
#include "../Core/Core.h"

// ASTNode ////////////////////////////////////////
ASTNode::ASTNode() : m_parent(nullptr) { }

ASTNode::~ASTNode()
{
    for (ASTNode* child : m_children)
    {
        delete child;
    }
}
    
ASTNode* ASTNode::GetParent() { return m_parent; }
const ASTNode* ASTNode::GetParent() const { return m_parent; }

void ASTNode::AddChild(ASTNode* child)
{
    ASSERT(child != nullptr);
    m_children.push_back(child);
    child->m_parent = this;
}

ASTNode::Iterator ASTNode::begin() { return m_children.begin(); }
ASTNode::Iterator ASTNode::end() { return m_children.end(); }

// IDNode ////////////////////////////////////////////////////////
IDNode::IDNode(const std::string& id) : m_id(id) { }

const std::string& IDNode::GetID() const { return m_id; }

// TypeNode //////////////////////////////////////////////////////
TypeNode::TypeNode(const std::string& type) : m_type(type) { }

const std::string& TypeNode::GetType() const { return m_type; }

// BaseOperatorNode ////////////////////////////////////////
BaseOperatorNode::BaseOperatorNode(const std::string& op) : m_operator(op) { }
const std::string& BaseOperatorNode::GetOperator() const { return m_operator; }

// AddOpNode //////////////////////////////////////////////
AddOpNode::AddOpNode(const std::string& op) : BaseOperatorNode(op) { }

// MultOpNode ////////////////////////////////////////////////
MultOpNode::MultOpNode(const std::string& op) : BaseOperatorNode(op) { }

// RelOpNode ///////////////////////////////////////////////////
RelOpNode::RelOpNode(const std::string& op) : BaseOperatorNode(op) { }

// LiteralNode ////////////////////////////////////////////
LiteralNode::LiteralNode(IDNode* lexeme, TypeNode* type)
{
    AddChild(lexeme);
    AddChild(type);
}

IDNode* LiteralNode::GetLexemeNode()
{
    return (IDNode*)*begin();
}

TypeNode* LiteralNode::GetType()
{
    return (TypeNode*)*(begin()++);
}

// Dimension ////////////////////////////////////
void DimensionNode::AddDimension(LiteralNode* dimension) { AddChild(dimension); }

// VarDeclNode ////////////////////////////////////////////
VarDeclNode::VarDeclNode(IDNode* id, TypeNode* type, DimensionNode* dimension)
{
    AddChild(id);
    AddChild(type);
    AddChild(dimension);
}

IDNode* VarDeclNode::GetID()
{
    return (IDNode*)*begin();
}

TypeNode* VarDeclNode::GetType()
{
    return (TypeNode*)*(begin()++);
}

DimensionNode* VarDeclNode::GetDimension()
{
    return (DimensionNode*)*((begin()++)++);
}

// DotNode ////////////////////////////////////////////////////////
DotNode::DotNode(ASTNode* leftSide, ASTNode* rightSide)
{
    AddChild(leftSide);
    AddChild(rightSide);
}

ASTNode* DotNode::GetLeft()
{
    return *begin();
}

ASTNode* DotNode::GetRight()
{
    return *(begin()++);
}

// ExprNode ////////////////////////////////////////
ExprNode::ExprNode(LiteralNode* literal) { AddChild(literal); }
ExprNode::ExprNode(VarDeclNode* var) { AddChild(var); }
ExprNode::ExprNode(DotNode* dot) { AddChild(dot); }
ExprNode::ExprNode(AddOpNode* op) { AddChild(op); }
ExprNode::ExprNode(MultOpNode* op) { AddChild(op); }
ExprNode::ExprNode(RelOpNode* op) { AddChild(op); }

// BaseLangStatNode //////////////////////////////////
BaseLangStatNode::BaseLangStatNode(ExprNode* expr) { AddChild(expr); }

ExprNode* BaseLangStatNode::GetExpr() { return (ExprNode*)*begin(); }

// ReturnStatNode ////////////////////////////////////////////////
ReturnStatNode::ReturnStatNode(ExprNode* expr) : BaseLangStatNode(expr) { }

// VariableNode ///////////////////////////////////////////////////
VariableNode::VariableNode(IDNode* var, DimensionNode* dimension) : m_dimension(dimension)
{ 
    AddChild(var); 
    AddChild(dimension);
}

VariableNode::VariableNode(DotNode* var) : m_dimension(nullptr) { AddChild(var); }

ASTNode* VariableNode::GetVariable() { return *begin(); }
DimensionNode* VariableNode::GetDimension() { return m_dimension; }

// ReadStatNode //////////////////////////////////////////////////
ReadStatNode::ReadStatNode(VariableNode* var) { AddChild(var); }

VariableNode* ReadStatNode::GetVariable() { return (VariableNode*)*begin(); }

// WriteStatNode //////////////////////////////////////////////////////
WriteStatNode::WriteStatNode(ExprNode* expr) : BaseLangStatNode(expr) { }

// AssignStatNode ///////////////////////////////////////////
AssignStatNode::AssignStatNode(IDNode* left, ExprNode* expr)
{
    AddChild(left);
    AddChild(expr);
}

AssignStatNode::AssignStatNode(DotNode* left, ExprNode* expr)
{
    AddChild(left);
    AddChild(expr);
}

ASTNode* AssignStatNode::GetLeft() { return *begin(); }
ExprNode* AssignStatNode::GetRight() { return (ExprNode*)*(begin()++); }

// IfStatNode ////////////////////////////////////////////////////////////////////////
IfStatNode::IfStatNode(RelOpNode* condition, StatBlockNode* ifBranch, StatBlockNode* elseBranch)
{
    AddChild(condition);
    AddChild(ifBranch);
    AddChild(elseBranch);
}

RelOpNode* IfStatNode::GetCondition() { return (RelOpNode*)*begin(); }
StatBlockNode* IfStatNode::GetIfBranch() { return (StatBlockNode*)*(begin()++); }
StatBlockNode* IfStatNode::GetElseBranch() { return (StatBlockNode*)*((begin()++)++); }

// WhileStatNode //////////////////////////////////////////////////////////////////////
WhileStatNode::WhileStatNode(RelOpNode* condition, StatBlockNode* statBlock)
{
    AddChild(condition);
    AddChild(statBlock);
}

RelOpNode* WhileStatNode::GetCondition() { return (RelOpNode*)*begin(); }
StatBlockNode* WhileStatNode::GetStatBlock() { return (StatBlockNode*)*(begin()++); }

// AParamList //////////////////////////////////////////////
void AParamListNode::AddParam(ExprNode* param)
{
    AddChild(param);
}

// FuncCallNode ////////////////////////////////////////////// 
FuncCallNode::FuncCallNode(IDNode* id, AParamListNode* parameters)
{
    AddChild(id);
    AddChild(parameters);
}

IDNode* FuncCallNode::GetID() { return (IDNode*)*begin(); }
AParamListNode* FuncCallNode::GetParameters() { return (AParamListNode*)*(begin()++); }

// StatBlockNode ///////////////////////////////////////////////////
void StatBlockNode::AddStatement(ReturnStatNode* statement) { AddChild(statement); }
void StatBlockNode::AddStatement(ReadStatNode* statement) { AddChild(statement); }
void StatBlockNode::AddStatement(WriteStatNode* statement) { AddChild(statement); }
void StatBlockNode::AddStatement(AssignStatNode* statement) { AddChild(statement); }
void StatBlockNode::AddStatement(IfStatNode* statement) { AddChild(statement); }
void StatBlockNode::AddStatement(WhileStatNode* statement) { AddChild(statement); }
void StatBlockNode::AddStatement(FuncCallNode* statement) { AddChild(statement); }

// FParamListNode //////////////////////////////////////////
void FParamListNode::AddVarDecl(VarDeclNode* var)
{
    AddChild(var);
}

// FunctionDefNode ////////////////////////////////////////////////////////////////
FunctionDefNode::FunctionDefNode(IDNode* id, TypeNode* returnType, FParamListNode* parameters,  
    StatBlockNode* statBlock)
{
    AddChild(id);
    AddChild(returnType);
    AddChild(parameters);
    AddChild(statBlock);
}

IDNode* FunctionDefNode::GetID() { return (IDNode*)*begin(); }
TypeNode* FunctionDefNode::GetReturnType() { return (TypeNode*)*(begin()++); }
FParamListNode* FunctionDefNode::GetParameters() { return (FParamListNode*)*((begin()++)++); }
StatBlockNode* FunctionDefNode::GetStatBlock() { return (StatBlockNode*)*(((begin()++)++)++); }

// MemberData ////////////////////////////////////////////////////////
MemberData::MemberData(const std::string& visibility) : m_visibility(visibility) { }
const std::string& MemberData::GetVisibility() const { return m_visibility; }

// MemVarNode /////////////////////////////////////////////////////////////
MemVarNode::MemVarNode(const std::string& visibility, IDNode* id, 
    TypeNode* type, DimensionNode* dimension) : VarDeclNode(id, type, dimension), 
    MemberData(visibility)
{
}

// ClassDefNode //////////////////////////////////////////////////////////////
ClassDefNode::ClassDefNode(IDNode* id)
{
    AddChild(id);
}

ClassDefNode::~ClassDefNode()
{
    for (VarDeclNode* var : m_varDeclarations)
    {
        delete var;
    }

    for (FunctionDefNode* func : m_functionDefinitions)
    {
        delete func;
    }
}

IDNode* ClassDefNode::GetID() { return (IDNode*)*begin(); }
void ClassDefNode::AddVarDecl(MemVarNode* var) { m_varDeclarations.push_back(var); }

void ClassDefNode::AddFuncDecl(MemFuncNode* func) 
{ 
    m_functionDefinitions.push_back(func); 
}

std::list<MemVarNode*>& ClassDefNode::GetVarDecl() { return m_varDeclarations; }
std::list<MemFuncNode*>& ClassDefNode::GetFuncDefNode() { return m_functionDefinitions; }

// ClassDefListNode ////////////////////////////////////////////////////////////
void ClassDefListNode::AddClass(ClassDefNode* classDef)
{
    AddChild(classDef);
}

// FunctionDefListNode //////////////////////////////////////////////////////
void FunctionDefListNode::AddFunc(FunctionDefNode* funcDef)
{
    AddChild(funcDef);
}

// ProgramNode ////////////////////////////////////////////////////////////
ProgramNode::ProgramNode()
{
    AddChild(new ClassDefListNode());
    AddChild(new FunctionDefListNode());
}

ClassDefListNode* ProgramNode::GetClassList()
{
    return (ClassDefListNode*)*begin();
}

FunctionDefListNode* ProgramNode::GetFunctionList()
{
    return (FunctionDefListNode*)*(begin()++);
}