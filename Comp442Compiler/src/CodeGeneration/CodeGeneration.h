#pragma once
#include "../SemanticChecking/Visitor.h"
#include "../Core/Util.h"

#include <list>
#include <unordered_map>

typedef size_t RegisterID;
constexpr RegisterID NullRegister = SIZE_MAX;

class LiteralNode;
class ExprNode;
class VariableNode;
class TempVarNodeBase;

class PlatformSpecifications
{
public:
    static constexpr size_t GetIntStrSize() { return s_intstrStackFrameSize; }
    static constexpr int GetIntStrArg1Offset() { return s_intstrFirstArgOffset; }
    static constexpr int GetIntStrArg2Offset() { return s_intstrSecondArgOffset; }

    static constexpr int GetPutStrArg1Offset() { return s_putstrFirstArgOffset; }

    static constexpr size_t GetAddressSize() { return s_addressSize; }
    static constexpr size_t GetIntSize() { return s_intSize; }
    static constexpr size_t GetFloatSize() { return s_floatSize; }
    

private:
    // sizes in bytes
    static constexpr size_t s_addressSize = 4;
    static constexpr size_t s_intSize = 4;
    static constexpr size_t s_floatSize = 8;

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

    virtual void Visit(ExprNode* element) override;
    virtual void Visit(ModifiedExpr* element) override;
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
    void TryAddTempVar(VarDeclNode* element);
    
    // returns InvalidType if could not compute
    size_t ComputeSizeOfFunc(SymbolTable* funcTable);

    void ProcessFunc(ASTNode* func, SymbolTableEntryKind expectedKind);
    void ComputeOffsets(SymbolTable* table, int startOffset = 0);
    void ComputeClassOffsets(ClassTableEntry* classEntry);

    bool ClassHasOffsets(ClassTableEntry* classEntry);

    void CreateTempVar(TempVarNodeBase* node);

    SymbolTable* m_globalTable;

    std::list<ASTNode*> m_toRevisit;
    std::list<ClassTableEntry*> m_classesWithOffsets;
};

class CodeGenerator : public Visitor
{
public:
    CodeGenerator(SymbolTable* globalTable, const std::string& filepath);
    virtual void Visit(ExprNode* element) override;
    virtual void Visit(ModifiedExpr* element) override;
    virtual void Visit(BaseBinaryOperator* element) override;
    virtual void Visit(FunctionDefNode* element) override;
    virtual void Visit(IfStatNode* element) override;
    virtual void Visit(WhileStatNode* element) override;
    virtual void Visit(AssignStatNode* element) override;
    virtual void Visit(WriteStatNode* element) override;
    virtual void Visit(ProgramNode* element) override;


    void OutputCode() const;

private:
    // adds the provided framesize to the scope size stack and moves the stack pointer by the previous top of the stack 
    std::string IncrementStackFrame(size_t frameSize);
    std::string DecrementStackFrame();

    std::string GenerateModifiedExpr(ModifiedExpr* modExpr);
    std::string GenerateMinusExpr(ModifiedExpr* modExpr);

    std::string GenerateBinaryOp(BaseBinaryOperator* opNode);    

    // code generation for different binary operators
    std::string GenerateAddOp(BaseBinaryOperator* opNode);
    std::string GenerateSubOp(BaseBinaryOperator* opNode);
    std::string GenerateMultOp(BaseBinaryOperator* opNode);
    std::string GenerateDivOp(BaseBinaryOperator* opNode);

    std::string GenerateOr(BaseBinaryOperator* opNode);
    std::string GenerateAnd(BaseBinaryOperator* opNode);
    std::string GenerateNot(ModifiedExpr* expr);
    std::string GenerateEqual(BaseBinaryOperator* opNode);
    std::string GenerateNotEqual(BaseBinaryOperator* opNode);
    std::string GenerateLessThan(BaseBinaryOperator* opNode);
    std::string GenerateGreaterThan(BaseBinaryOperator* opNode);
    std::string GenerateLessOrEqual(BaseBinaryOperator* opNode);
    std::string GenerateGreaterOrEqual(BaseBinaryOperator* opNode);

    std::string GenerateArithmeticOp(BaseBinaryOperator* opNode, const char* commandName);
    std::string GenerateAndOr(BaseBinaryOperator* opNode, const char* commandName);
    std::string GenerateRelOp(BaseBinaryOperator* opNode, const char* commandName);

    // returns the string of code to stores the value of the node and stores the register in which 
    //the data was stored in the provided outRegister field or NullRegister if the operation 
    //is not supported
    
    std::string ComputeVal(ASTNode* node, RegisterID& outRegister);
    std::string ComputeVal(BaseBinaryOperator* node, RegisterID& outRegister);
    std::string ComputeVal(ExprNode* node, RegisterID& outRegister);
    std::string ComputeVal(ModifiedExpr* node, RegisterID& outRegister);

    std::string LoadVarInRegister(ASTNode* n, RegisterID& outRegister);
    std::string LoadVarInRegister(LiteralNode* node, RegisterID& outRegister);
    std::string LoadVarInRegister(VariableNode* node, RegisterID& outRegister);
    std::string LoadTempVarInRegister(TempVarNodeBase* tempVar, RegisterID& outRegister);

    std::string LoadFloatToAddr(int offset, LiteralNode* floatLiteral);

    std::string CopyData(SymbolTableEntry* data, SymbolTableEntry* dest);
    std::string& GetCurrStatBlock(ASTNode* node);

    std::string m_filepath;
    std::string m_dataCode;
    std::string m_executableCode;
    //std::string m_currStatBlockCode;

    RegisterID m_topOfStackRegister;
    RegisterID m_zeroRegister; // register with value 0 not necessarily r0
    RegisterID m_jumpReturnRegister;
    RegisterID m_returnValRegister;
    std::list<RegisterID> m_registerStack;
    std::list<size_t> m_scopeSizeStack;
    std::unordered_map<ASTNode*, std::string> m_statBlocks;

    TagGenerator m_tagGen;
};