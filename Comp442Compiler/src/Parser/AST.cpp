#include "AST.h"
#include "../Core/Core.h"

// Helpers /////////////////////////////////////////////////
void WriteIndentToStream(std::stringstream& ss, size_t indent)
{
    for (size_t i = 0; i < indent; i++)
    {
        ss << "|  ";
    }
}

// ASTNode //////////////////////////////////////////////////
ASTNode::~ASTNode() { }

// ASTNodeBase ///////////////////////////////////////////

ASTNodeBase::ASTNodeBase() : m_parent(nullptr) { }

ASTNodeBase::~ASTNodeBase()
{
    for (ASTNode* child : m_children)
    {
        delete child;
    }
}

ASTNode* ASTNodeBase::GetParent() { return m_parent; }
const ASTNode* ASTNodeBase::GetParent() const { return m_parent; }
void ASTNodeBase::SetParent(ASTNode* parent) { m_parent = parent; }

size_t ASTNodeBase::GetNumChild() const { return m_children.size(); }

void ASTNodeBase::AddChild(ASTNode* child)
{
    ASSERT(child != nullptr);
    m_children.push_back(child);
    child->SetParent(this);
}

void ASTNodeBase::AddChildFirst(ASTNode* child)
{
    ASSERT(child != nullptr);
    m_children.push_front(child);
    child->SetParent(this);
}

ASTNode* ASTNodeBase::GetChild(size_t index)
{
    auto it = m_children.begin();
    for (size_t i = 0; i < index; i++)
    {
        it++;
    }
    return *it;
}

std::list<ASTNode*>& ASTNodeBase::GetChildren() { return m_children; }

// LeafNode ////////////////////////////////////////////////////////////////
LeafNode::LeafNode() : m_parent(nullptr) { }

ASTNode* LeafNode::GetParent() { return m_parent; }
const ASTNode* LeafNode::GetParent() const { return m_parent; }
void LeafNode::SetParent(ASTNode* parent) { m_parent = parent; };
size_t LeafNode::GetNumChild() const { return 0; }

// EmptyNodeBase ////////////////////////////////////////////////////////////
ASTNode* EmptyNodeBase::GetParent() { return nullptr; }
const ASTNode* EmptyNodeBase::GetParent() const { return nullptr; }
void EmptyNodeBase::SetParent(ASTNode* parent) { }

size_t EmptyNodeBase::GetNumChild() const { return 0; }
std::string EmptyNodeBase::ToString(size_t indent) { return ""; };

// UnspecificedDimensionNode //////////////////////////////////////////
std::string UnspecificedDimensionNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "UnspecifiedArrDimension\n";

    return ss.str();
}

// IDNode ////////////////////////////////////////////////////////
IDNode::IDNode(const Token& id) : m_id(id) { }

const Token& IDNode::GetID() const { return m_id; }

std::string IDNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "id\n";
    WriteIndentToStream(ss, indent + 1);
    ss << GetID().GetLexeme() << "\n";

    return ss.str();
}

// TypeNode //////////////////////////////////////////////////////
TypeNode::TypeNode(const Token& type) : m_type(type) { }

const Token& TypeNode::GetType() const { return m_type; }

std::string TypeNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "TypeNode\n";
    WriteIndentToStream(ss, indent + 1);
    ss << GetType().GetLexeme() << "\n"; 

    return ss.str();
}

// OperatorNode ////////////////////////////////////////
OperatorNode::OperatorNode(const Token& op) : m_operator(op) { }
const Token& OperatorNode::GetOperator() const { return m_operator; }

std::string OperatorNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << m_operator.GetLexeme() <<"\n";
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

ASTNode* BaseBinaryOperator::GetLeft() { return GetChild(0); }
OperatorNode* BaseBinaryOperator::GetOperator() { return (OperatorNode*)GetChild(1); }
ASTNode* BaseBinaryOperator::GetRight() { return GetChild(2); }

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

IDNode* LiteralNode::GetLexemeNode() { return (IDNode*)GetChild(0); }

TypeNode* LiteralNode::GetType() { return (TypeNode*)GetChild(1); }

std::string LiteralNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "Literal\n";
    WriteIndentToStream(ss, indent + 1);
    ss << GetLexemeNode()->GetID().GetLexeme() << "\n";
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
    for (ASTNode* dimension : GetChildren())
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
    return (IDNode*)GetChild(0);
}

TypeNode* VarDeclNode::GetType()
{
    return (TypeNode*)GetChild(1);
}

DimensionNode* VarDeclNode::GetDimension()
{
    return dynamic_cast<DimensionNode*>(GetChild(2));
}

AParamListNode* VarDeclNode::GetParamList()
{
    return dynamic_cast<AParamListNode*>(GetChild(2));
}

std::string VarDeclNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "VarDecl\n";
    ss << GetID()->ToString(indent + 1);
    ss << GetType()->ToString(indent + 1);
    ss << GetChild(2)->ToString(indent + 1);

    return ss.str();
}

// DotNode ////////////////////////////////////////////////////////
DotNode::DotNode(ASTNode* leftSide, ASTNode* rightSide)
{
    AddChild(leftSide);
    AddChild(rightSide);
}

ASTNode* DotNode::GetLeft() { return GetChild(0); }

ASTNode* DotNode::GetRight() { return GetChild(1); }

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

ExprNode::ExprNode() { }

std::string ExprNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "Expr\n";
    ss << GetChild(0)->ToString(indent + 1);
    return ss.str();
}

// SignNode ////////////////////////////////////////////
SignNode::SignNode(const Token& sign) : m_sign(sign) { }

const Token& SignNode::GetSign() { return m_sign; }

std::string SignNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "Sign\n";
    WriteIndentToStream(ss, indent + 1);
    ss << m_sign.GetLexeme() << "\n";

    return ss.str();
}

// NotNode ///////////////////////////////////////////////////////
std::string NotNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "not\n";

    return ss.str();
}

// ModifiedExpr ///////////////////////////////////////////////////
ModifiedExpr::ModifiedExpr(ASTNode* modifier, ASTNode* expr)
{
    AddChild(modifier);
    AddChild(expr);
}

ASTNode* ModifiedExpr::GetModifier() { return GetChild(0); }
ASTNode* ModifiedExpr::GetExpr() { return GetChild(1); }

std::string ModifiedExpr::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "ModifiedExpr\n";
    ss << GetModifier()->ToString(indent + 1);
    ss << GetExpr()->ToString(indent + 1);

    return ss.str();
}

// BaseLangStatNode //////////////////////////////////
BaseLangStatNode::BaseLangStatNode(ExprNode* expr) { AddChild(expr); }

ExprNode* BaseLangStatNode::GetExpr() { return (ExprNode*)GetChild(0); }

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
VariableNode::VariableNode(IDNode* var, DimensionNode* dimension)
{ 
    AddChild(var); 
    AddChild(dimension);
}

ASTNode* VariableNode::GetVariable() { return GetChild(0); }
DimensionNode* VariableNode::GetDimension() { return (DimensionNode*)GetChild(1); }

std::string VariableNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "Variable\n";
    ss << GetVariable()->ToString(indent + 1);
    ss << GetDimension()->ToString(indent + 1);
    return ss.str();
}

// ReadStatNode //////////////////////////////////////////////////
ReadStatNode::ReadStatNode(ASTNode* var) { AddChild(var); }

ASTNode* ReadStatNode::GetVariable() { return GetChild(0); }

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

ASTNode* AssignStatNode::GetLeft() { return GetChild(0); }
ExprNode* AssignStatNode::GetRight() { return (ExprNode*)GetChild(1); }

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

ExprNode* IfStatNode::GetCondition() { return (ExprNode*)GetChild(0); }
StatBlockNode* IfStatNode::GetIfBranch() { return (StatBlockNode*)GetChild(1); }
StatBlockNode* IfStatNode::GetElseBranch() { return (StatBlockNode*)GetChild(2); }

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

ExprNode* WhileStatNode::GetCondition() { return (ExprNode*)GetChild(0); }
StatBlockNode* WhileStatNode::GetStatBlock() { return (StatBlockNode*)GetChild(1); }

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
    for (ASTNode* param : GetChildren())
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

IDNode* FuncCallNode::GetID() { return (IDNode*)GetChild(0); }
AParamListNode* FuncCallNode::GetParameters() { return (AParamListNode*)GetChild(1); }

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
    for (ASTNode* statement : GetChildren())
    {
        ss << statement->ToString(indent + 1);
    }
    return ss.str();
}

// FParamNode //////////////////////////////////////////////////
FParamNode::FParamNode(IDNode* id, TypeNode* type, DimensionNode* dimension)
    : VarDeclNode(id, type, dimension) { }

std::string FParamNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "FParam\n";
    ss << GetID()->ToString(indent + 1);
    ss << GetType()->ToString(indent + 1);
    ss << GetChild(2)->ToString(indent + 1);

    return ss.str();
}

// FParamListNode //////////////////////////////////////////
void FParamListNode::AddLoopingChild(ASTNode* param)
{
    AddChildFirst(param);
}

std::string FParamListNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "FParamList\n";
    for (ASTNode* param : GetChildren())
    {
        ss << param->ToString(indent + 1);
    }

    return ss.str();
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

IDNode* FunctionDefNode::GetID() { return (IDNode*)GetChild(0); }
TypeNode* FunctionDefNode::GetReturnType() { return (TypeNode*)GetChild(1); }
FParamListNode* FunctionDefNode::GetParameters() { return (FParamListNode*)GetChild(2); }
StatBlockNode* FunctionDefNode::GetBody() { return (StatBlockNode*)GetChild(3); }

std::string FunctionDefNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "FuncDef\n";
    ss << GetID()->ToString(indent + 1);
    ss << GetReturnType()->ToString(indent + 1);
    ss << GetParameters()->ToString(indent + 1);
    ss << GetBody()->ToString(indent + 1);

    return ss.str();
}

// VisibilityNode /////////////////////////////////////////////////////
VisibilityNode::VisibilityNode(const std::string& visibility) : m_visibility(visibility) { }

const std::string& VisibilityNode::GetVisibility() const { return m_visibility; }

std::string VisibilityNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "Visibility\n";
    WriteIndentToStream(ss, indent + 1);
    ss << m_visibility << "\n";

    return ss.str();
}

// DefaultVisibilityNode //////////////////////////////////////////////////////
DefaultVisibilityNode::DefaultVisibilityNode() 
    : VisibilityNode("default") { }

// MemVarNode /////////////////////////////////////////////////////////////
MemVarNode::MemVarNode(VisibilityNode* visibility, IDNode* id, 
    TypeNode* type, DimensionNode* dimension) : VarDeclNode(id, type, dimension) 
{
    AddChild(visibility);
}

VisibilityNode* MemVarNode::GetVisibility()
{
    return (VisibilityNode*)GetChild(3);
}

std::string MemVarNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "MemVar\n";
    ss << GetVisibility()->ToString(indent + 1);
    ss << GetID()->ToString(indent + 1);
    ss << GetType()->ToString(indent + 1);
    ss << GetDimension()->ToString(indent + 1);

    return ss.str();
}

// MemFuncDeclNode ///////////////////////////////////////////////////////////////////
MemFuncDeclNode::MemFuncDeclNode(VisibilityNode* visibility, IDNode* id, TypeNode* returnType, 
    FParamListNode* parameters) 
{
    AddChild(visibility);
    AddChild(id);
    AddChild(returnType);
    AddChild(parameters);
}

VisibilityNode* MemFuncDeclNode::GetVisibility()
{
    return (VisibilityNode*)GetChild(0);
}

IDNode* MemFuncDeclNode::GetID()
{
    return (IDNode*)GetChild(1);
}

TypeNode* MemFuncDeclNode::GetReturnType()
{
    return (TypeNode*)GetChild(2);
}

FParamListNode* MemFuncDeclNode::GetParameters()
{
    return (FParamListNode*)GetChild(3);
}

std::string MemFuncDeclNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "MemFuncDecl\n";
    ss << GetVisibility()->ToString(indent + 1);
    ss << GetID()->ToString(indent + 1);
    ss << GetParameters()->ToString(indent + 1);
    ss << GetReturnType()->ToString(indent + 1);

    return ss.str();
}

// MemFuncDefNode //////////////////////////////////////////////////////////////////
MemFuncDefNode::MemFuncDefNode(IDNode* classID, IDNode* id, TypeNode* returnType, 
    FParamListNode* parameters, StatBlockNode* statBlock) 
    : FunctionDefNode(id, returnType, parameters, statBlock)
{
    AddChild(classID);
}

IDNode* MemFuncDefNode::GetClassID() { return (IDNode*)GetChild(4); }

std::string MemFuncDefNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "MemFuncDef\n";
    ss << GetClassID()->ToString(indent + 1);
    ss << GetID()->ToString(indent + 1);
    ss << GetReturnType()->ToString(indent + 1);
    ss << GetParameters()->ToString(indent + 1);
    ss << GetBody()->ToString(indent + 1);

    return ss.str();
}

// ConstructorDeclNode ////////////////////////////////////////////////////
ConstructorDeclNode::ConstructorDeclNode(VisibilityNode* visibility, FParamListNode* params) 
{
    AddChild(visibility); 
    AddChild(params);
}

VisibilityNode* ConstructorDeclNode::GetVisibility() { return (VisibilityNode*)GetChild(0); }

FParamListNode* ConstructorDeclNode::GetParams() { return (FParamListNode*)GetChild(1); }

std::string ConstructorDeclNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "ConstructorDecl\n";
    ss << GetVisibility()->ToString(indent + 1);
    ss << GetParams()->ToString(indent + 1);

    return ss.str();
}

// ConstructorDefNode ///////////////////////////////////////////////////////////////
ConstructorDefNode::ConstructorDefNode(IDNode* classID, FParamListNode* params, 
    StatBlockNode* body) : FunctionDefNode(classID, new TypeNode(classID->GetID()), 
    params, body) { }

std::string ConstructorDefNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "ConstructorDef\n";
    ss << GetReturnType()->ToString(indent + 1);
    ss << GetParameters()->ToString(indent + 1);
    ss << GetBody()->ToString(indent + 1);

    return ss.str();
}

// InheritanceListNode ///////////////////////////////////////////////////////////
void InheritanceListNode::AddLoopingChild(ASTNode* id) { AddChildFirst(id); }

std::string InheritanceListNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "InheritanceList\n";
    for (ASTNode* id : GetChildren())
    {
        ss << id->ToString(indent + 1);
    }

    return ss.str();
}

// ClassDefNode //////////////////////////////////////////////////////////////
ClassDefNode::ClassDefNode(IDNode* id, InheritanceListNode* inheritanceList)
{
    AddChild(id);
    AddChild(inheritanceList);
}

ClassDefNode::~ClassDefNode()
{
    for (MemVarNode* var : m_varDeclarations)
    {
        delete var;
    }

    for (ConstructorDeclNode* constructor : m_constructors)
    {
        delete constructor;
    }

    for (MemFuncDeclNode* func : m_functionDeclarations)
    {
        delete func;
    }
}

IDNode* ClassDefNode::GetID() { return (IDNode*)GetChild(0); }

InheritanceListNode* ClassDefNode::GetInheritanceList() 
{ 
    return (InheritanceListNode*)GetChild(1); 
}

void ClassDefNode::AddVarDecl(MemVarNode* var) 
{
    m_varDeclarations.push_front(var);
    var->SetParent(this);
}

void ClassDefNode::AddConstructor(ConstructorDeclNode* constructor) 
{ 
    m_constructors.push_front(constructor); 
    constructor->SetParent(this);
}

void ClassDefNode::AddFuncDecl(MemFuncDeclNode* func) 
{ 
    m_functionDeclarations.push_front(func); 
    func->SetParent(this);
}

std::list<MemVarNode*>& ClassDefNode::GetVarDecls() { return m_varDeclarations; }
std::list<ConstructorDeclNode*>& ClassDefNode::GetConstructors() { return m_constructors; }
std::list<MemFuncDeclNode*>& ClassDefNode::GetFuncDecls() { return m_functionDeclarations; }

std::string ClassDefNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "ClassDef\n";
    ss << GetID()->ToString(indent + 1);
    ss << GetInheritanceList()->ToString(indent + 1);
    for (MemVarNode* memVar : m_varDeclarations)
    {
        ss << memVar->ToString(indent + 1);
    }

    for (ConstructorDeclNode* constructor : m_constructors)
    {
        ss << constructor->ToString(indent + 1);
    }

    for (MemFuncDeclNode* memFunc : m_functionDeclarations)
    {
        ss << memFunc->ToString(indent + 1);
    }

    return ss.str();
}

// ClassDefListNode ////////////////////////////////////////////////////////////
void ClassDefListNode::AddClass(ClassDefNode* classDef)
{
    AddChild(classDef);
}

std::string ClassDefListNode::ToString(size_t indent) 
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "ClassDefList\n";
    for (ASTNode* classDef : GetChildren())
    {
        ss << classDef->ToString(indent + 1);
    }

    return ss.str();
}

// FunctionDefListNode //////////////////////////////////////////////////////
void FunctionDefListNode::AddFunc(FunctionDefNode* funcDef)
{
    AddChild(funcDef);
}

std::string FunctionDefListNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "FuncDefList\n";
    for (ASTNode* funcDef : GetChildren())
    {
        ss << funcDef->ToString(indent + 1);
    }

    return ss.str();
}

// ProgramNode ////////////////////////////////////////////////////////////
ProgramNode::ProgramNode()
{
    AddChild(new ClassDefListNode());
    AddChild(new FunctionDefListNode());
}

ClassDefListNode* ProgramNode::GetClassList()
{
    return (ClassDefListNode*)GetChild(0);
}

FunctionDefListNode* ProgramNode::GetFunctionList()
{
    return (FunctionDefListNode*)GetChild(1);
}

std::string ProgramNode::ToString(size_t indent)
{
    std::stringstream ss;
    WriteIndentToStream(ss, indent);
    ss << "Program\n";
    ss << GetClassList()->ToString(indent + 1);
    ss << GetFunctionList()->ToString(indent + 1);

    return ss.str();
}