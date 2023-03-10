#pragma once
#include <list>
#include <string>
#include <sstream>

#include "../Core/Token.h"

class StatBlockNode;
class ExprNode;
class AParamListNode;

class ASTNode
{
public:
    virtual ~ASTNode();

    virtual ASTNode* GetParent() = 0;
    virtual const ASTNode* GetParent() const = 0;
    virtual void SetParent(ASTNode* parent) = 0;

    virtual size_t GetNumChild() const = 0;

    virtual std::string ToString(size_t indent = 0) = 0;
};

class ASTNodeBase : public ASTNode
{
public:
    ASTNodeBase();
    virtual ~ASTNodeBase();

    virtual ASTNode* GetParent() override;
    virtual const ASTNode* GetParent() const override;
    virtual void SetParent(ASTNode* parent) override;

    virtual size_t GetNumChild() const override;

protected:
    void AddChild(ASTNode* n);
    void AddChildFirst(ASTNode* n);

    ASTNode* GetChild(size_t index);
    std::list<ASTNode*>& GetChildren();

private:
    ASTNode* m_parent;
    std::list<ASTNode*> m_children;
};

class LeafNode : public ASTNode
{
public:
    LeafNode();

    virtual ASTNode* GetParent() override;
    virtual const ASTNode* GetParent() const override;
    virtual void SetParent(ASTNode* parent) override;

    virtual size_t GetNumChild() const override;

private:
    ASTNode* m_parent;
};

// represent an empty node used only during the construction of the AST
class EmptyNodeBase : public ASTNode 
{
public:
    virtual ASTNode* GetParent() override;
    virtual const ASTNode* GetParent() const override;
    virtual void SetParent(ASTNode* parent) override;

    virtual size_t GetNumChild() const override;

    virtual std::string ToString(size_t indent = 0) override;
};

// serves as stop point when looping
class StopNode : public EmptyNodeBase { };

// used to differentiate between free functions, member functions and constructors
class FreeFuncMarkerNode : public EmptyNodeBase { };
class MemFuncMarkerNode : public EmptyNodeBase { };
class ConstructorMarkerNode : public EmptyNodeBase { };

// serves as a marker when an array size is not specified
class UnspecificedDimensionNode : public LeafNode 
{ 
public:
    virtual std::string ToString(size_t indent = 0) override;
};

class IDNode : public LeafNode
{
public:
    IDNode(const Token& id);

    const Token& GetID() const;

    virtual std::string ToString(size_t indent = 0) override;

private:
    Token m_id;
};

class TypeNode : public LeafNode
{
public:
    TypeNode(const Token& type);

    const Token& GetType() const;

    virtual std::string ToString(size_t indent = 0) override;
private:
    Token m_type;
};

class OperatorNode : public LeafNode
{
public:
    OperatorNode(const Token& op);
    const Token& GetOperator() const;

    virtual std::string ToString(size_t indent = 0) override;

private:
    Token m_operator;
};

class BaseBinaryOperator : public ASTNodeBase
{
public:
    BaseBinaryOperator(const std::string& name, ASTNode* left, 
        OperatorNode* op, ASTNode* right);

    ASTNode* GetLeft();
    OperatorNode* GetOperator();
    ASTNode* GetRight();

    virtual std::string ToString(size_t indent = 0) override;

private:
    std::string m_name;
};

class AddOpNode : public BaseBinaryOperator
{
public:
    AddOpNode(ASTNode* left, OperatorNode* op, ASTNode* right);
};

class MultOpNode : public BaseBinaryOperator
{
public:
    MultOpNode(ASTNode* left, OperatorNode* op, ASTNode* right);
};

class RelOpNode : public BaseBinaryOperator
{
public:
    RelOpNode(ASTNode* left, OperatorNode* op, ASTNode* right);
};

class LiteralNode : public ASTNodeBase
{
public:
    LiteralNode(IDNode* lexeme, TypeNode* type);

    IDNode* GetLexemeNode();
    TypeNode* GetType();

    virtual std::string ToString(size_t indent = 0) override;
};

class DimensionNode : public ASTNodeBase
{
public: 
    void AddLoopingChild(ASTNode* dimension);

    virtual std::string ToString(size_t indent = 0) override;
};

class VarDeclNode : public ASTNodeBase
{
public:
    VarDeclNode(IDNode* id, TypeNode* type, DimensionNode* dimension);
    VarDeclNode(IDNode* id, TypeNode* type, AParamListNode* params);

    IDNode* GetID();
    TypeNode* GetType();
    DimensionNode* GetDimension();
    AParamListNode* GetParamList();

    virtual std::string ToString(size_t indent = 0) override;
};

class DotNode : public ASTNodeBase
{
public:
    DotNode(ASTNode* leftSide, ASTNode* rightSide);

    ASTNode* GetLeft();
    ASTNode* GetRight();

    virtual std::string ToString(size_t indent = 0) override;
};

class ExprNode : public ASTNodeBase
{
public:
    ExprNode(ASTNode* exprRoot);

    virtual std::string ToString(size_t indent = 0) override;

protected:
    ExprNode();

private:
    std::string m_sign;
};

class SignNode : public LeafNode
{
public:
    SignNode(const Token& sign);

    const Token& GetSign();

    virtual std::string ToString(size_t indent = 0) override;

private:
    Token m_sign;
};

class NotNode : public LeafNode 
{
public:
    virtual std::string ToString(size_t indent = 0) override;
};

// represents an expression being modified by a modifier such as a sign or the "not" keyword
class ModifiedExpr : public ExprNode
{
public:
    ModifiedExpr(ASTNode* modifier, ASTNode* expr);

    ASTNode* GetModifier();
    ASTNode* GetExpr();

    virtual std::string ToString(size_t indent = 0) override;
};

// base node of simple statements provided by the language such as return or write
class BaseLangStatNode : public ASTNodeBase
{
public:
    BaseLangStatNode(ExprNode* expr);

    ExprNode* GetExpr();
};

class ReturnStatNode : public BaseLangStatNode
{
public:
    ReturnStatNode(ExprNode* expr);

    virtual std::string ToString(size_t indent = 0) override;
};

class VariableNode : public ASTNodeBase
{
public:
    VariableNode(IDNode* var, DimensionNode* dimension);

    ASTNode* GetVariable();    
    DimensionNode* GetDimension();

    virtual std::string ToString(size_t indent = 0) override;
};

class ReadStatNode : public ASTNodeBase
{
public:
    ReadStatNode(ASTNode* var);

    ASTNode* GetVariable();

    virtual std::string ToString(size_t indent = 0) override;
};

class WriteStatNode : public BaseLangStatNode
{
public:
    WriteStatNode(ExprNode* expr);
    virtual std::string ToString(size_t indent = 0) override;
};

class AssignStatNode : public ASTNodeBase
{
public:
    AssignStatNode(ASTNode* left, ExprNode* expr);

    ASTNode* GetLeft();
    ExprNode* GetRight();

    virtual std::string ToString(size_t indent = 0) override;
};

class IfStatNode : public ASTNodeBase
{
public:
    IfStatNode(ExprNode* condition, StatBlockNode* ifBranch, StatBlockNode* elseBranch);

    ExprNode* GetCondition();
    StatBlockNode* GetIfBranch();
    StatBlockNode* GetElseBranch();

    virtual std::string ToString(size_t indent = 0) override;
};

class WhileStatNode : public ASTNodeBase
{
public:
    WhileStatNode(ExprNode* condition, StatBlockNode* statBlock);

    ExprNode* GetCondition();
    StatBlockNode* GetStatBlock();

    virtual std::string ToString(size_t indent = 0) override;
};

class AParamListNode : public ASTNodeBase
{
public:
    void AddLoopingChild(ASTNode* param);

    virtual std::string ToString(size_t indent = 0) override;
};

class FuncCallNode : public ASTNodeBase
{
public:
    FuncCallNode(IDNode* id, AParamListNode* parameters);

    IDNode* GetID();
    AParamListNode* GetParameters();

    virtual std::string ToString(size_t indent = 0) override;
};

class StatBlockNode : public ASTNodeBase
{
public:
    void AddLoopingChild(ASTNode* statement);

    virtual std::string ToString(size_t indent = 0) override;
};

class FParamNode : public VarDeclNode
{
public:
    FParamNode(IDNode* id, TypeNode* type, DimensionNode* dimension);

    virtual std::string ToString(size_t indent = 0) override;
};

class FParamListNode : public ASTNodeBase
{
public:
    void AddLoopingChild(ASTNode* param);

    virtual std::string ToString(size_t indent = 0) override;
};

class FunctionDefNode : public ASTNodeBase
{
public:
    FunctionDefNode(IDNode* id, TypeNode* returnType, FParamListNode* parameters,  
        StatBlockNode* statBlock);

    IDNode* GetID();
    TypeNode* GetReturnType();
    FParamListNode* GetParameters();
    StatBlockNode* GetBody();

    virtual std::string ToString(size_t indent = 0) override;
};

class VisibilityNode : public LeafNode
{
public:
    VisibilityNode(const std::string& visibility);

    const std::string& GetVisibility() const;

    virtual std::string ToString(size_t indent = 0) override;
    
private:
    std::string m_visibility;
};

class DefaultVisibilityNode : public VisibilityNode 
{
public:
    DefaultVisibilityNode();
};

class MemVarNode : public VarDeclNode
{
public:
    MemVarNode(VisibilityNode* visibility, IDNode* id, 
        TypeNode* type, DimensionNode* dimension);

    VisibilityNode* GetVisibility();

    virtual std::string ToString(size_t indent = 0) override;
};

class MemFuncDeclNode : public ASTNodeBase
{
public:
    MemFuncDeclNode(VisibilityNode* visibility, IDNode* id, TypeNode* returnType, 
        FParamListNode* parameters);

    VisibilityNode* GetVisibility();
    IDNode* GetID();
    TypeNode* GetReturnType();
    FParamListNode* GetParameters();

    virtual std::string ToString(size_t indent = 0) override;
};

class MemFuncDefNode : public FunctionDefNode
{
public:
    MemFuncDefNode(IDNode* classID, IDNode* id, TypeNode* returnType, 
        FParamListNode* parameters, StatBlockNode* statBlock);

    IDNode* GetClassID();

    virtual std::string ToString(size_t indent = 0) override;
};

class ConstructorDeclNode : public ASTNodeBase
{
public:
    ConstructorDeclNode(VisibilityNode* visibility, FParamListNode* params);

    VisibilityNode* GetVisibility();
    FParamListNode* GetParams();

    virtual std::string ToString(size_t indent = 0) override;
};

class ConstructorDefNode : public FunctionDefNode
{
public:
    ConstructorDefNode(IDNode* classID, FParamListNode* params, StatBlockNode* body);
    virtual std::string ToString(size_t indent = 0) override;
};

class InheritanceListNode : public ASTNodeBase
{
public:
    void AddLoopingChild(ASTNode* id);

    virtual std::string ToString(size_t indent = 0) override;
};

class ClassDefNode : public ASTNodeBase
{
public:
    ClassDefNode(IDNode* id, InheritanceListNode* inheritanceList);
    ~ClassDefNode();

    void AddVarDecl(MemVarNode* var);
    void AddConstructor(ConstructorDeclNode* constructor);
    void AddFuncDecl(MemFuncDeclNode* func);

    IDNode* GetID();
    InheritanceListNode* GetInheritanceList();
    std::list<MemVarNode*>& GetVarDecls();
    std::list<ConstructorDeclNode*>& GetConstructors();
    std::list<MemFuncDeclNode*>& GetFuncDecls();

    virtual std::string ToString(size_t indent = 0) override;

private:

    std::list<MemVarNode*> m_varDeclarations;
    std::list<ConstructorDeclNode*> m_constructors;
    std::list<MemFuncDeclNode*> m_functionDeclarations;
};

class ClassDefListNode : public ASTNodeBase
{
public:
    void AddClass(ClassDefNode* classDef);

    virtual std::string ToString(size_t indent = 0) override;
};

class FunctionDefListNode : public ASTNodeBase
{
public:
    void AddFunc(FunctionDefNode* funcDef);

    virtual std::string ToString(size_t indent = 0) override;
};

class ProgramNode : public ASTNodeBase
{
public:
    ProgramNode();

    ClassDefListNode* GetClassList();
    FunctionDefListNode* GetFunctionList();

    virtual std::string ToString(size_t indent = 0) override;
};