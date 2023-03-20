#pragma once
#include "../SemanticChecking/Visitor.h"


// generates the size entry of the SymbolTable
class SizeGenerator : public Visitor
{
public:
    SizeGenerator(SymbolTable* globalTable, size_t intSize = 4, size_t floatSize = 8, size_t boolSize = 1);

    virtual void Visit(BaseBinaryOperator* element) override;
    virtual void Visit(VarDeclNode* element) override;
    virtual void Visit(FParamNode* element) override;
    virtual void Visit(FunctionDefNode* element) override;
    virtual void Visit(MemVarNode* element) override;
    virtual void Visit(MemFuncDeclNode* element) override;
    virtual void Visit(ConstructorDeclNode* element) override;
    virtual void Visit(ClassDefNode* element) override;
    virtual void Visit(ProgramNode* element) override;

private:
    // returns InvalidSize if the type provided is could not be computed 
    size_t ComputeSize(TypeNode* type, DimensionNode* dimensions);
    size_t ComputeSize(const std::string& typeStr);

    // returns the size of a class object
    size_t FindSize(const std::string& typeStr);

    void TryAddTempVar(VarDeclNode* element);
    
    // returns InvalidType if could not compute
    size_t ComputeSizeOfFunc(SymbolTable* funcTable);

    void ProcessFunc(ASTNode* func, SymbolTableEntryKind expectedKind);

    SymbolTable* m_globalTable;

	// size in bytes
	size_t m_intSize;
	size_t m_floatSize;
	size_t m_boolSize;

    std::list<ASTNode*> m_toRevisit;
};

class CodeGenerator : public Visitor
{
public:
};