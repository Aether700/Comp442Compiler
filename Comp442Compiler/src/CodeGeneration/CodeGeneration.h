#pragma once
#include "../SemanticChecking/Visitor.h"
#include <list>

typedef size_t RegisterID;

class PlatformSpecifications
{
public:
    static size_t GetIntStrSize();
    static int GetIntStrArg1Offset();
    static int GetIntStrArg2Offset();

    static int GetPutStrArg1Offset();

    static size_t GetAddressSize();
    static size_t GetIntSize();
    static size_t GetFloatSize();
    static size_t GetBoolSize();
    

private:
    // sizes in bytes
    static constexpr size_t s_addressSize = 4;
    static constexpr size_t s_intSize = 4;
    static constexpr size_t s_floatSize = 8;
    static constexpr size_t s_boolSize = 1;

    static constexpr size_t s_intstrStackFrameSize = 24;
    static constexpr int s_intstrFirstArgOffset = -8;
    static constexpr int s_intstrSecondArgOffset = -12;

    static constexpr int s_putstrFirstArgOffset = -8;
};


// generates the size entry of the SymbolTable
class SizeGenerator : public Visitor
{
public:
    SizeGenerator(SymbolTable* globalTable);

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
    void ComputeOffsets(SymbolTable* table, int startOffset = 0);
    void ComputeClassOffsets(ClassTableEntry* classEntry);

    bool ClassHasOffsets(ClassTableEntry* classEntry);

    SymbolTable* m_globalTable;

    std::list<ASTNode*> m_toRevisit;
    std::list<ClassTableEntry*> m_classesWithOffsets;
};

class CodeGenerator : public Visitor
{
public:
    CodeGenerator(const std::string& filepath);
    virtual void Visit(AssignStatNode* element) override;
    virtual void Visit(WriteStatNode* element) override;
    virtual void Visit(ProgramNode* element) override;


    void OutputCode() const;

private:
    static constexpr size_t s_startStackAddr = 100;

    std::string m_filepath;
    std::string m_dataCode;
    std::string m_executionCode;

    std::string IncrementStackFrame(size_t frameSize);
    std::string DecrementStackFrame(size_t frameSize);

    RegisterID m_topOfStackRegister;
    RegisterID m_zeroRegister; // register with value 0 not necessarily r0
    RegisterID m_jumpReturnRegister;
    RegisterID m_returnValRegister;
    std::list<RegisterID> m_registerStack;
};