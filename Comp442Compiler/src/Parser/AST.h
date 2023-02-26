#pragma once
#include <list>
#include <string>
#include <sstream>

class StatBlockNode;
class ExprNode;
class AParamListNode;

class ASTNode
{
    friend class ClassDefNode;
public:
    using Iterator = std::list<ASTNode*>::iterator;

    ASTNode();
    virtual ~ASTNode();

    ASTNode* GetParent();
    const ASTNode* GetParent() const;

    size_t GetNumChild() const;

    // eventually will be abstract
    virtual std::string ToString(size_t indent = 0) { return ""; }

    ASTNode::Iterator begin();
    ASTNode::Iterator end();

protected:
    void AddChild(ASTNode* child);
    void AddChildFirst(ASTNode* child);

    void WriteIndentToStream(std::stringstream& ss, size_t indent);

private:
    ASTNode* m_parent;
    std::list<ASTNode*> m_children;
};

// serves as stop point when looping
class StopNode : public ASTNode { };

// used to differentiate between free functions, member functions and constructors
class FreeFuncMarkerNode : public ASTNode { };
class MemFuncMarkerNode : public ASTNode { };
class ConstructorMarkerNode : public ASTNode { };

// serves as a marker when an array size is not specified
class UnspecificedDimensionNode : public ASTNode 
{ 
public:
    virtual std::string ToString(size_t indent = 0) override;
};

class IDNode : public ASTNode
{
public:
    IDNode(const std::string& id);

    const std::string& GetID() const;

    virtual std::string ToString(size_t indent = 0) override;

private:
    std::string m_id;
};

class TypeNode : public ASTNode
{
public:
    TypeNode(const std::string& type);

    const std::string& GetType() const;

    virtual std::string ToString(size_t indent = 0) override;
private:
    std::string m_type;
};

class OperatorNode : public ASTNode
{
public:
    OperatorNode(const std::string& op);
    const std::string& GetOperator() const;

    virtual std::string ToString(size_t indent = 0) override;

private:
    std::string m_operator;
};

class BaseBinaryOperator : public ASTNode
{
public:
    BaseBinaryOperator(const std::string& name, ASTNode* left, OperatorNode* op, ASTNode* right);

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

class LiteralNode : public ASTNode
{
public:
    LiteralNode(IDNode* lexeme, TypeNode* type);

    IDNode* GetLexemeNode();
    TypeNode* GetType();

    virtual std::string ToString(size_t indent = 0) override;
};

class DimensionNode : public ASTNode
{
public: 
    void AddLoopingChild(ASTNode* dimension);

    virtual std::string ToString(size_t indent = 0) override;
};

class VarDeclNode : public ASTNode
{
public:
    VarDeclNode(IDNode* id, TypeNode* type, DimensionNode* dimension);
    VarDeclNode(IDNode* id, TypeNode* type, AParamListNode* params);

    IDNode* GetID();
    TypeNode* GetType();
    DimensionNode* GetDimension();
    AParamListNode* GetParamList();

    virtual std::string ToString(size_t indent = 0) override;

private:
    ASTNode* GetThirdNode();
};

class DotNode : public ASTNode
{
public:
    DotNode(ASTNode* leftSide, ASTNode* rightSide);

    ASTNode* GetLeft();
    ASTNode* GetRight();

    virtual std::string ToString(size_t indent = 0) override;
};

class ExprNode : public ASTNode
{
public:
    ExprNode(ASTNode* exprRoot);

    virtual std::string ToString(size_t indent = 0) override;

protected:
    ExprNode();

private:
    std::string m_sign;
};

class SignNode : public ASTNode
{
public:
    SignNode(const std::string& sign);

    const std::string& GetSign();

    virtual std::string ToString(size_t indent = 0) override;

private:
    std::string m_sign;
};

class NotNode : public ASTNode 
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
class BaseLangStatNode : public ASTNode
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

class VariableNode : public ASTNode
{
public:
    VariableNode(IDNode* var, DimensionNode* dimension);
    VariableNode(DotNode* var);

    ASTNode* GetVariable();
    
    // can be nullptr
    DimensionNode* GetDimension();

    virtual std::string ToString(size_t indent = 0) override;

private:
    DimensionNode* m_dimension;
};

class ReadStatNode : public ASTNode
{
public:
    ReadStatNode(VariableNode* var);

    VariableNode* GetVariable();

    virtual std::string ToString(size_t indent = 0) override;
};

class WriteStatNode : public BaseLangStatNode
{
public:
    WriteStatNode(ExprNode* expr);
    virtual std::string ToString(size_t indent = 0) override;
};

class AssignStatNode : public ASTNode
{
public:
    AssignStatNode(ASTNode* left, ExprNode* expr);

    ASTNode* GetLeft();
    ExprNode* GetRight();

    virtual std::string ToString(size_t indent = 0) override;
};

class IfStatNode : public ASTNode
{
public:
    IfStatNode(ExprNode* condition, StatBlockNode* ifBranch, StatBlockNode* elseBranch);

    ExprNode* GetCondition();
    StatBlockNode* GetIfBranch();
    StatBlockNode* GetElseBranch();

    virtual std::string ToString(size_t indent = 0) override;
};

class WhileStatNode : public ASTNode
{
public:
    WhileStatNode(ExprNode* condition, StatBlockNode* statBlock);

    ExprNode* GetCondition();
    StatBlockNode* GetStatBlock();

    virtual std::string ToString(size_t indent = 0) override;
};

class AParamListNode : public ASTNode
{
public:
    void AddLoopingChild(ASTNode* param);

    virtual std::string ToString(size_t indent = 0) override;
};

class FuncCallNode : public ASTNode
{
public:
    FuncCallNode(IDNode* id, AParamListNode* parameters);

    IDNode* GetID();
    AParamListNode* GetParameters();

    virtual std::string ToString(size_t indent = 0) override;
};

class StatBlockNode : public ASTNode
{
public:
    void AddLoopingChild(ASTNode* statement);

    virtual std::string ToString(size_t indent = 0) override;
};

class FParamListNode : public ASTNode
{
public:
    void AddLoopingChild(ASTNode* param);

    virtual std::string ToString(size_t indent = 0) override;
};

class FunctionDefNode : public ASTNode
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

class VisibilityNode : public ASTNode
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

class MemFuncDeclNode : public ASTNode
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

class ConstructorDeclNode : public ASTNode
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

class InheritanceListNode : public ASTNode
{
public:
    void AddLoopingChild(ASTNode* id);

    virtual std::string ToString(size_t indent = 0) override;
};

class ClassDefNode : public ASTNode
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

class ClassDefListNode : public ASTNode
{
public:
    void AddClass(ClassDefNode* classDef);

    virtual std::string ToString(size_t indent = 0) override;
};

class FunctionDefListNode : public ASTNode
{
public:
    void AddFunc(FunctionDefNode* funcDef);

    virtual std::string ToString(size_t indent = 0) override;
};

class ProgramNode : public ASTNode
{
public:
    ProgramNode();

    ClassDefListNode* GetClassList();
    FunctionDefListNode* GetFunctionList();

    virtual std::string ToString(size_t indent = 0) override;
};