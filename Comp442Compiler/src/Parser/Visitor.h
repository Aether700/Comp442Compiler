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
class ProgramNode;

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
    virtual void Visit(ProgramNode* element) { }
};


class IVisitableElement
{
public:
    virtual void AcceptVisit(Visitor* visitor) = 0;
};

class SymbolTableAssembler : public Visitor
{ 
public:
    SymbolTableAssembler();
    ~SymbolTableAssembler();

    virtual void Visit(VarDeclNode* element) override;
    virtual void Visit(FParamNode* element) override;
    virtual void Visit(FunctionDefNode* element) override;
    virtual void Visit(ProgramNode* element) override;
    
    SymbolTable* GetGlobalSymbolTable();

private:
    SymbolTable* m_globalScopeTable;
    std::list<SymbolTableEntry*> m_stack;
};