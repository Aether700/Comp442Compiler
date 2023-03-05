#pragma once
#include <list>

#include "SymbolTable.h"


class IVisitableElement;
class IDNode;
class VarDeclNode;
class FParamNode;
class FunctionDefNode;
class MemVarNode;
class MemFuncDeclNode;
class ConstructorDeclNode;

class Visitor
{
public:
    virtual void Visit(IVisitableElement* element) { }
    virtual void Visit(IDNode* element) { }
    virtual void Visit(VarDeclNode* element) { }
    virtual void Visit(FParamNode* element) { }
    virtual void Visit(FunctionDefNode* element) { }
    virtual void Visit(MemVarNode* element) { }
    virtual void Visit(MemFuncDeclNode* element) { }
    virtual void Visit(ConstructorDeclNode* element) { }
};


class IVisitableElement
{
public:
    virtual void AcceptVisit(Visitor* visitor) = 0;
};

class SymbolTableAssembler : public Visitor
{ 
public:
    ~SymbolTableAssembler();

    virtual void Visit(VarDeclNode* element) override;
    virtual void Visit(FParamNode* element) override;
    virtual void Visit(FunctionDefNode* element) override;

    // temp
    SymbolTableEntry* GetLast() { return m_stack.front(); }
    //

private:
    std::list<SymbolTableEntry*> m_stack;
};