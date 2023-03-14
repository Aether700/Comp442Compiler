#pragma once
#include <list>

#include "SymbolTable.h"


class IVisitableElement;
class IDNode;
class DotNode;
class BaseBinaryOperator;
class AssignStatNode;
class FParamNode;
class FParamListNode;
class AParamListNode;
class FuncCallNode;
class MemVarNode;
class MemFuncDeclNode;
class MemFuncDefNode;
class ConstructorDeclNode;
class ProgramNode;


class Visitor
{
public:
    virtual void Visit(IVisitableElement* element) { }
    virtual void Visit(IDNode* element) { }
    virtual void Visit(DotNode* element) { }
    virtual void Visit(BaseBinaryOperator* element) { }
    virtual void Visit(AssignStatNode* element) { }
    virtual void Visit(VarDeclNode* element) { }
    virtual void Visit(FParamNode* element) { }
    virtual void Visit(FuncCallNode* element) { }
    virtual void Visit(FunctionDefNode* element) { }
    virtual void Visit(MemVarNode* element) { }
    virtual void Visit(MemFuncDeclNode* element) { }
    virtual void Visit(MemFuncDefNode* element) { }
    virtual void Visit(ConstructorDeclNode* element) { }
    virtual void Visit(ConstructorDefNode* element) { }
    virtual void Visit(InheritanceListNode* element) { }
    virtual void Visit(ClassDefNode* element) { }
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
    virtual void Visit(MemVarNode* element) override;
    virtual void Visit(MemFuncDeclNode* element) override;
    virtual void Visit(MemFuncDefNode* element) override;
    virtual void Visit(ConstructorDeclNode* element) override;
    virtual void Visit(ConstructorDefNode* element) override;
    virtual void Visit(InheritanceListNode* element) override;
    virtual void Visit(ClassDefNode* element) override;
    virtual void Visit(ProgramNode* element) override;
    
    SymbolTable* GetGlobalSymbolTable();

private:
    // return true if a match was found, false otherwise
    bool TryMatchMemFuncDeclAndDef(MemFuncDefEntry* def);
    bool TryMatchMemFuncDeclAndDef(ConstructorDefEntry* def);
    void CheckForDeclWithoutDef(ClassTableEntry* classEntry);

    SymbolTable* m_globalScopeTable;
    std::list<SymbolTableEntry*> m_workingList;
};

class SemanticChecker : public Visitor
{
public:
    SemanticChecker(SymbolTable* globalTable);

    virtual void Visit(IDNode* element) override;
    virtual void Visit(DotNode* element) override;
    virtual void Visit(BaseBinaryOperator* element) override;
    virtual void Visit(AssignStatNode* element) override;
    virtual void Visit(FuncCallNode* element) override;
    virtual void Visit(FunctionDefNode* element) override;
    virtual void Visit(MemFuncDefNode* element) override;
    virtual void Visit(ConstructorDefNode* element) override;
    virtual void Visit(InheritanceListNode* element) override;
    virtual void Visit(ClassDefNode* element) override;
    

private:
    bool HasFoundOverLoadedFunc(const std::list<std::string>& funcList, 
        const std::string& name);

    bool HasMatchingParameters(FParamListNode* fparam, AParamListNode* aparam);

    // recursively go down the dot chain to try and find the type of the dot expression
    void TestDotRemainder(SymbolTable* contextTable, ASTNode* dotRemainder);

    SymbolTable* m_globalTable;
    std::list<std::string> m_overloadedFreeFuncFound;
    std::list<std::string> m_overloadedMemFuncFound;
    std::list<std::string> m_overloadedConstructorFound;
};