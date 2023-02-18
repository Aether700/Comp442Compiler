#pragma once
#include <list>

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