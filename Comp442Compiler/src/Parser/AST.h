#pragma once
#include <list>
#include <string>
#include <sstream>

class StatBlockNode;
class ExprNode;
class AParamListNode;

class ASTNode
{
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
    void AddVarDecl(VarDeclNode* var);
};

class FunctionDefNode : public ASTNode
{
public:
    FunctionDefNode(IDNode* id, TypeNode* returnType, FParamListNode* parameters,  
        StatBlockNode* statBlock);

    IDNode* GetID();
    TypeNode* GetReturnType();
    FParamListNode* GetParameters();
    StatBlockNode* GetStatBlock();
};

class MemberData
{
public:
    MemberData(const std::string& visibility);

    const std::string& GetVisibility() const;
private:
    std::string m_visibility;
};

class MemVarNode : public VarDeclNode, public MemberData
{
public:
    MemVarNode(const std::string& visibility, IDNode* id, 
        TypeNode* type, DimensionNode* dimension);
};

class MemFuncNode : public FunctionDefNode, public MemberData
{
public:
    MemFuncNode(const std::string& visibility, IDNode* id, TypeNode* returnType, 
        FParamListNode* parameters, StatBlockNode* statBlock);
};

class ClassDefNode : public ASTNode
{
public:
    ClassDefNode(IDNode* id);
    ~ClassDefNode();

    void AddVarDecl(MemVarNode* var);
    void AddFuncDecl(MemFuncNode* func);

    IDNode* GetID();
    std::list<MemVarNode*>& GetVarDecl();
    std::list<MemFuncNode*>& GetFuncDefNode();

private:
    std::list<MemVarNode*> m_varDeclarations;
    std::list<MemFuncNode*> m_functionDefinitions;
};

class ClassDefListNode : public ASTNode
{
public:
    void AddClass(ClassDefNode* classDef);
};

class FunctionDefListNode : public ASTNode
{
public:
    void AddFunc(FunctionDefNode* funcDef);
};

class ProgramNode : public ASTNode
{
public:
    ProgramNode();

    ClassDefListNode* GetClassList();
    FunctionDefListNode* GetFunctionList();
};