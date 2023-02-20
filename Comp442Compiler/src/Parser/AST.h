#pragma once
#include <list>
#include <string>

class StatBlockNode;

class ASTNode
{
public:
    using Iterator = std::list<ASTNode*>::iterator;

    ASTNode();
    virtual ~ASTNode();

    ASTNode* GetParent();
    const ASTNode* GetParent() const;


    ASTNode::Iterator begin();
    ASTNode::Iterator end();

protected:
    void AddChild(ASTNode* child);

private:
    ASTNode* m_parent;
    std::list<ASTNode*> m_children;
};

class IDNode : public ASTNode
{
public:
    IDNode(const std::string& id);

    const std::string& GetID() const;
private:
    std::string m_id;
};

class TypeNode : public ASTNode
{
public:
    TypeNode(const std::string& type);

    const std::string& GetType() const;
private:
    std::string m_type;
};

class BaseOperatorNode : public ASTNode
{
public:
    BaseOperatorNode(const std::string& op);
    const std::string& GetOperator() const;

private:
    std::string m_operator;
};

class AddOpNode : public BaseOperatorNode
{
public:
    AddOpNode(const std::string& op);
};

class MultOpNode : public BaseOperatorNode
{
public:
    MultOpNode(const std::string& op);
};

class RelOpNode : public BaseOperatorNode
{
public:
    RelOpNode(const std::string& op);
};

class LiteralNode : public ASTNode
{
public:
    LiteralNode(IDNode* lexeme, TypeNode* type);

    IDNode* GetLexemeNode();
    TypeNode* GetType();
};

class DimensionNode : public ASTNode
{
public: 
    void AddDimension(LiteralNode* dimension);
};

class VarDeclNode : public ASTNode
{
public:
    VarDeclNode(IDNode* id, TypeNode* type, DimensionNode* dimension);

    IDNode* GetID();
    TypeNode* GetType();
    DimensionNode* GetDimension();
};

class DotNode : public ASTNode
{
public:
    DotNode(ASTNode* leftSide, ASTNode* rightSide);

    ASTNode* GetLeft();
    ASTNode* GetRight();
};

class ExprNode : public ASTNode
{
public:
    ExprNode(LiteralNode* literal);
    ExprNode(VarDeclNode* var);
    ExprNode(DotNode* dot);
    ExprNode(AddOpNode* op);
    ExprNode(MultOpNode* op);
    ExprNode(RelOpNode* op);
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
};

class VariableNode : public ASTNode
{
public:
    VariableNode(IDNode* var, DimensionNode* dimension);
    VariableNode(DotNode* var);

    ASTNode* GetVariable();
    
    // can be nullptr
    DimensionNode* GetDimension();

private:
    DimensionNode* m_dimension;
};

class ReadStatNode : public ASTNode
{
public:
    ReadStatNode(VariableNode* var);

    VariableNode* GetVariable();
};

class WriteStatNode : public BaseLangStatNode
{
public:
    WriteStatNode(ExprNode* expr);
};

class AssignStatNode : public ASTNode
{
public:
    AssignStatNode(IDNode* left, ExprNode* expr);
    AssignStatNode(DotNode* left, ExprNode* expr);

    ASTNode* GetLeft();
    ExprNode* GetRight();
};

class IfStatNode : public ASTNode
{
public:
    IfStatNode(RelOpNode* condition, StatBlockNode* ifBranch, StatBlockNode* elseBranch);

    RelOpNode* GetCondition();
    StatBlockNode* GetIfBranch();
    StatBlockNode* GetElseBranch();
};

class WhileStatNode : public ASTNode
{
public:
    WhileStatNode(RelOpNode* condition, StatBlockNode* statBlock);

    RelOpNode* GetCondition();
    StatBlockNode* GetStatBlock();
};

class AParamListNode : public ASTNode
{
public:
    void AddParam(ExprNode* param);
};

class FuncCallNode : public ASTNode
{
public:
    FuncCallNode(IDNode* id, AParamListNode* parameters);

    IDNode* GetID();
    AParamListNode* GetParameters();
};

class StatBlockNode : public ASTNode
{
public:
    void AddStatement(ReturnStatNode* statement);
    void AddStatement(ReadStatNode* statement);
    void AddStatement(WriteStatNode* statement);
    void AddStatement(AssignStatNode* statement);
    void AddStatement(IfStatNode* statement);
    void AddStatement(WhileStatNode* statement);
    void AddStatement(FuncCallNode* statement);
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