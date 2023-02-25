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

size_t ASTNode::GetNumChild() const { return m_children.size(); }

void ASTNode::AddChild(ASTNode* child)
{
    ASSERT(child != nullptr);
    m_children.push_back(child);
    child->m_parent = this;
}

void ASTNode::AddChildFirst(ASTNode* child)
{
    ASSERT(child != nullptr);
    m_children.push_front(child);
    child->m_parent = this;
}

void ASTNode::WriteIndentToStream(std::stringstream& ss, size_t indent)
{
    for (size_t i = 0; i < indent; i++)
    {
        ss << "|  ";
    }
}

ASTNode::Iterator ASTNode::begin() { return m_children.begin(); }
ASTNode::Iterator ASTNode::end() { return m_children.end(); }

// UnspecificedDimensionNode //////////////////////////////////////////
std::string UnspecificedDimensionNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "UnspecifiedArrDimension\n";

    return ss.str();
}

// IDNode ////////////////////////////////////////////////////////
IDNode::IDNode(const std::string& id) : m_id(id) { }

const std::string& IDNode::GetID() const { return m_id; }

std::string IDNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "id\n";
    WriteIndentToStream(ss, indent + 1);
    ss << GetID() << "\n";

    return ss.str();
}

// TypeNode //////////////////////////////////////////////////////
TypeNode::TypeNode(const std::string& type) : m_type(type) { }

const std::string& TypeNode::GetType() const { return m_type; }

std::string TypeNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "TypeNode\n";
    WriteIndentToStream(ss, indent + 1);
    ss << GetType() << "\n"; 

    return ss.str();
}

// OperatorNode ////////////////////////////////////////
OperatorNode::OperatorNode(const std::string& op) : m_operator(op) { }
const std::string& OperatorNode::GetOperator() const { return m_operator; }

std::string OperatorNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << m_operator <<"\n";
    return ss.str();
}

// BaseBinaryOperator //////////////////////////////////////////////
BaseBinaryOperator::BaseBinaryOperator(const std::string& name, ASTNode* left, 
    OperatorNode* op, ASTNode* right) : m_name(name)
{
    AddChild(left);
    AddChild(op);
    AddChild(right);
}

ASTNode* BaseBinaryOperator::GetLeft() { return *begin(); }
OperatorNode* BaseBinaryOperator::GetOperator() { return (OperatorNode*)*(++begin()); }
ASTNode* BaseBinaryOperator::GetRight() { return *(++(++begin())); }

std::string BaseBinaryOperator::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << m_name << "\n";
    ss << GetLeft()->ToString(indent + 1);
    ss << GetOperator()->ToString(indent + 1);
    ss << GetRight()->ToString(indent + 1);
    return ss.str();
}

// AddOpNode //////////////////////////////////////////////
AddOpNode::AddOpNode(ASTNode* left, OperatorNode* op, ASTNode* right) 
    : BaseBinaryOperator("AddOp", left, op, right) { }

// MultOpNode ////////////////////////////////////////////////
MultOpNode::MultOpNode(ASTNode* left, OperatorNode* op, ASTNode* right) 
    : BaseBinaryOperator("MultOp", left, op, right) { }

// RelOpNode ///////////////////////////////////////////////////
RelOpNode::RelOpNode(ASTNode* left, OperatorNode* op, ASTNode* right) 
    : BaseBinaryOperator("RelOp", left, op, right) { }

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
    return (TypeNode*)*(++begin());
}

std::string LiteralNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "Literal\n";
    WriteIndentToStream(ss, indent + 1);
    ss << GetLexemeNode()->GetID() << "\n";
    ss << GetType()->ToString(indent + 1);
    return ss.str();
}

// DimensionNode ////////////////////////////////////
void DimensionNode::AddLoopingChild(ASTNode* dimension) { AddChildFirst(dimension); }

std::string DimensionNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "Dimensions\n";
    for (ASTNode* dimension : *this)
    {
        ss << dimension->ToString(indent + 1);
    }
    return ss.str();
}

// VarDeclNode ////////////////////////////////////////////
VarDeclNode::VarDeclNode(IDNode* id, TypeNode* type, DimensionNode* dimension)
{
    AddChild(id);
    AddChild(type);
    AddChild(dimension);
}

VarDeclNode::VarDeclNode(IDNode* id, TypeNode* type, AParamListNode* param)
{
    AddChild(id);
    AddChild(type);
    AddChild(param);
}

IDNode* VarDeclNode::GetID()
{
    return (IDNode*)*begin();
}

TypeNode* VarDeclNode::GetType()
{
    return (TypeNode*)*(++begin());
}

DimensionNode* VarDeclNode::GetDimension()
{
    return dynamic_cast<DimensionNode*>(GetThirdNode());
}

AParamListNode* VarDeclNode::GetParamList()
{
    return dynamic_cast<AParamListNode*>(GetThirdNode());
}

std::string VarDeclNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "VarDecl\n";
    ss << GetID()->ToString(indent + 1);
    ss << GetType()->ToString(indent + 1);
    ss << GetThirdNode()->ToString(indent + 1);

    return ss.str();
}

ASTNode* VarDeclNode::GetThirdNode()
{
    return *(++(++(begin())));
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
    return *(++begin());
}

std::string DotNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "Dot\n";
    
    ss << GetLeft()->ToString(indent + 1);
    ss << GetRight()->ToString(indent + 1);

    return ss.str();
}

// ExprNode ////////////////////////////////////////
ExprNode::ExprNode(ASTNode* exprRoot) { AddChild(exprRoot); }

std::string ExprNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "Expr\n";
    ss << (*begin())->ToString(indent + 1);
    return ss.str();
}

// BaseLangStatNode //////////////////////////////////
BaseLangStatNode::BaseLangStatNode(ExprNode* expr) { AddChild(expr); }

ExprNode* BaseLangStatNode::GetExpr() { return (ExprNode*)*begin(); }

// ReturnStatNode ////////////////////////////////////////////////
ReturnStatNode::ReturnStatNode(ExprNode* expr) : BaseLangStatNode(expr) { }

std::string ReturnStatNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "ReturnStat\n";
    ss << GetExpr()->ToString(indent + 1);

    return ss.str();
}

// VariableNode ///////////////////////////////////////////////////
VariableNode::VariableNode(IDNode* var, DimensionNode* dimension) : m_dimension(dimension)
{ 
    AddChild(var); 
    AddChild(dimension);
}

VariableNode::VariableNode(DotNode* var) : m_dimension(nullptr) { AddChild(var); }

ASTNode* VariableNode::GetVariable() { return *begin(); }
DimensionNode* VariableNode::GetDimension() { return m_dimension; }

std::string VariableNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "Variable\n";
    ss << GetVariable()->ToString(indent + 1);
    if (GetDimension() != nullptr)
    {
        ss << GetDimension()->ToString(indent + 1);
    }
    return ss.str();
}

// ReadStatNode //////////////////////////////////////////////////
ReadStatNode::ReadStatNode(VariableNode* var) { AddChild(var); }

VariableNode* ReadStatNode::GetVariable() { return (VariableNode*)*begin(); }

std::string ReadStatNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "ReadStat\n";
    ss << GetVariable()->ToString(indent + 1);

    return ss.str();
}

// WriteStatNode //////////////////////////////////////////////////////
WriteStatNode::WriteStatNode(ExprNode* expr) : BaseLangStatNode(expr) { }

std::string WriteStatNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "WriteStat\n";
    ss << GetExpr()->ToString(indent + 1);

    return ss.str();
}

// AssignStatNode ///////////////////////////////////////////
AssignStatNode::AssignStatNode(ASTNode* left, ExprNode* expr)
{
    AddChild(left);
    AddChild(expr);
}

ASTNode* AssignStatNode::GetLeft() { return *begin(); }
ExprNode* AssignStatNode::GetRight() { return (ExprNode*)*(++begin()); }

std::string AssignStatNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "AssigStat\n";
    ss << GetLeft()->ToString(indent + 1);
    ss << GetRight()->ToString(indent + 1);
    return ss.str();
}

// IfStatNode ////////////////////////////////////////////////////////////////////////
IfStatNode::IfStatNode(ExprNode* condition, StatBlockNode* ifBranch, StatBlockNode* elseBranch)
{
    AddChild(condition);
    AddChild(ifBranch);
    AddChild(elseBranch);
}

ExprNode* IfStatNode::GetCondition() { return (ExprNode*)*begin(); }
StatBlockNode* IfStatNode::GetIfBranch() { return (StatBlockNode*)*(++begin()); }
StatBlockNode* IfStatNode::GetElseBranch() { return (StatBlockNode*)*(++(++begin())); }

std::string IfStatNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "IfStat\n";
    ss << GetCondition()->ToString(indent + 1);
    ss << GetIfBranch()->ToString(indent + 1);
    ss << GetElseBranch()->ToString(indent + 1);

    return ss.str();
}

// WhileStatNode //////////////////////////////////////////////////////////////////////
WhileStatNode::WhileStatNode(ExprNode* condition, StatBlockNode* statBlock)
{
    AddChild(condition);
    AddChild(statBlock);
}

ExprNode* WhileStatNode::GetCondition() { return (ExprNode*)*begin(); }
StatBlockNode* WhileStatNode::GetStatBlock() { return (StatBlockNode*)*(++begin()); }

std::string WhileStatNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "WhileStat\n";
    ss << GetCondition()->ToString(indent + 1);
    ss << GetStatBlock()->ToString(indent + 1);

    return ss.str();
}

// AParamListNode //////////////////////////////////////////////
void AParamListNode::AddLoopingChild(ASTNode* param)
{
    ASSERT(dynamic_cast<ExprNode*>(param) != nullptr);
    AddChildFirst(param);
}

std::string AParamListNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "AParams\n";
    for (ASTNode* param : *this)
    {
        ss << param->ToString(indent + 1);
    }
    return ss.str();
}

// FuncCallNode ////////////////////////////////////////////// 
FuncCallNode::FuncCallNode(IDNode* id, AParamListNode* parameters)
{
    AddChild(id);
    AddChild(parameters);
}

IDNode* FuncCallNode::GetID() { return (IDNode*)*begin(); }
AParamListNode* FuncCallNode::GetParameters() { return (AParamListNode*)*(++begin()); }

std::string FuncCallNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "FuncCall\n";
    ss << GetID()->ToString(indent + 1);
    ss << GetParameters()->ToString(indent + 1);

    return ss.str();
}

// StatBlockNode ///////////////////////////////////////////////////
void StatBlockNode::AddLoopingChild(ASTNode* statement) { AddChildFirst(statement); }

std::string StatBlockNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "StatBlock\n";
    for (ASTNode* statement : *this)
    {
        ss << statement->ToString(indent + 1);
    }
    return ss.str();
}

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
TypeNode* FunctionDefNode::GetReturnType() { return (TypeNode*)*(++begin()); }
FParamListNode* FunctionDefNode::GetParameters() { return (FParamListNode*)*(++(++begin())); }
StatBlockNode* FunctionDefNode::GetStatBlock() { return (StatBlockNode*)*(++(++(++begin()))); }

// MemberData //////////////////////////////////////////////////////////////////
MemberData::MemberData(const std::string& visibility) : m_visibility(visibility) { }
const std::string& MemberData::GetVisibility() const { return m_visibility; }

// MemVarNode /////////////////////////////////////////////////////////////
MemVarNode::MemVarNode(const std::string& visibility, IDNode* id, 
    TypeNode* type, DimensionNode* dimension) : VarDeclNode(id, type, dimension), 
    MemberData(visibility) { }

// MemFuncNode ///////////////////////////////////////////////////////////////////
MemFuncNode::MemFuncNode(const std::string& visibility, IDNode* id, TypeNode* returnType, 
    FParamListNode* parameters, StatBlockNode* statBlock) 
    : FunctionDefNode(id, returnType, parameters, statBlock), MemberData(visibility) { }

// ClassDefNode //////////////////////////////////////////////////////////////
ClassDefNode::ClassDefNode(IDNode* id)
{
    AddChild(id);
}

ClassDefNode::~ClassDefNode()
{
    for (MemVarNode* var : m_varDeclarations)
    {
        delete var;
    }

    for (MemFuncNode* func : m_functionDefinitions)
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
    return (FunctionDefListNode*)*(++begin());
}