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

auto ASTNode::begin() { return m_children.begin(); }
auto ASTNode::end() { return m_children.end(); }

// VarNode //////////////////////////////////////////////////
VarNode::VarNode(const std::string& id) : m_id(id) { }
VarNode::VarNode(const std::string& id, const std::list<std::string>& indices) 
    : m_id(id), m_indices(indices) { }

const std::string& VarNode::GetID() const { return m_id; }
const std::list<std::string>& VarNode::GetIndices() const { return m_indices; }

// VarDeclNode ////////////////////////////////////////////
VarDeclNode::VarDeclNode(const std::string& id, const std::string& type) 
    : VarNode(id), m_type(type) { }

VarDeclNode::VarDeclNode(const std::string& id, const std::string& type, 
    const std::list<std::string>& arraySize) : VarNode(id, arraySize), m_type(type) { }

const std::string& VarDeclNode::GetType() const { return m_type; }
const std::list<std::string>& VarDeclNode::GetArraySize() const { return GetIndices(); }

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


// ProgramNode /////////////////////////////////////
ProgramNode::ProgramNode()
{
    AddChild(new ClassListNode());
    AddChild(new FreeFunctionListNode());
}

ClassListNode* ProgramNode::GetClassList()
{
    return (ClassListNode*)*begin();
}

FreeFunctionListNode* ProgramNode::GetFunctionList()
{
    return (FreeFunctionListNode*)*(begin()++);
}