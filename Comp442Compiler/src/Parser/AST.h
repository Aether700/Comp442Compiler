#pragma once
#include <list>
#include <string>

class ASTNode
{
public:
    ASTNode();
    virtual ~ASTNode();

    ASTNode* GetParent();
    const ASTNode* GetParent() const;

    void AddChild(ASTNode* child);

    auto begin();
    auto end();

private:
    ASTNode* m_parent;
    std::list<ASTNode*> m_children;
};

class VarNode : public ASTNode
{
public:
    VarNode(const std::string& id);
    VarNode(const std::string& id, const std::list<std::string>& indices);

    const std::string& GetID() const;
    const std::list<std::string>& GetIndices() const;

private:
    std::string m_id;
    std::list<std::string> m_indices;
};

class VarDeclNode : public VarNode
{
public:
    VarDeclNode(const std::string& id, const std::string& type);
    VarDeclNode(const std::string& id, const std::string& type, 
        const std::list<std::string>& arraySize);

    const std::string& GetType() const;
    const std::list<std::string>& GetArraySize() const;

private:
    std::string m_type;
};

class DotNode : public ASTNode
{
public:
    DotNode(ASTNode* leftSide, ASTNode* rightSide);

    ASTNode* GetLeft();
    ASTNode* GetRight();
};

class ClassDefNode : public ASTNode
{
    // Not implemented yet
};

class FreeFunctionDefNode : public ASTNode
{
    // Not implemented yet
};

class ClassListNode : public ASTNode
{
    // Not implemented yet
};

class FreeFunctionListNode : public ASTNode
{
    // Not implemented yet
};

class ProgramNode : public ASTNode
{
public:
    ProgramNode();

    ClassListNode* GetClassList();
    FreeFunctionListNode* GetFunctionList();
};