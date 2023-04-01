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
class RefVarNode;

class PlatformSpecifications
{
public:
    static constexpr size_t GetIntStrSize() { return s_intstrStackFrameSize; }
    static constexpr int GetIntStrArg1Offset() { return s_intstrFirstArgOffset; }
    static constexpr int GetIntStrArg2Offset() { return s_intstrSecondArgOffset; }

    static constexpr int GetPutStrArg1Offset() { return s_putstrFirstArgOffset; }

    static constexpr int GetGetStrArg1Offset() { return s_getstrFirstArgOffset; }

    static constexpr int GetStrIntArg1Offset() { return s_strintFirstArgOffset; }

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

    static constexpr int s_getstrFirstArgOffset = -8;

    static constexpr int s_strintFirstArgOffset = -8;
};


// generates the size entry of the SymbolTable
class SizeGenerator : public Visitor
{
public:
    SizeGenerator(SymbolTable* globalTable);

    virtual void Visit(LiteralNode* element) override;
    virtual void Visit(DotNode* element) override;
    virtual void Visit(ExprNode* element) override;
    virtual void Visit(ModifiedExpr* element) override;
    virtual void Visit(BaseBinaryOperator* element) override;
    virtual void Visit(VarDeclNode* element) override;
    virtual void Visit(FParamNode* element) override;
    virtual void Visit(FuncCallNode* element) override;
    virtual void Visit(FunctionDefNode* element) override;
    virtual void Visit(MemVarNode* element) override;
    virtual void Visit(MemFuncDeclNode* element) override;
    virtual void Visit(ConstructorDeclNode* element) override;
    virtual void Visit(ClassDefNode* element) override;
    virtual void Visit(ProgramNode* element) override;

private:
    void TryAddTempVar(FParamNode* element);
    void TryAddTempVar(VarDeclNode* element);
    void TryAddTempVar(TempVarNodeBase* element);
    void TryAddTempVar(ConstructorDeclNode* element);
    
    // returns InvalidType if could not compute
    size_t ComputeSizeOfFunc(SymbolTable* funcTable);

    void ProcessFunc(ASTNode* func, SymbolTableEntryKind expectedKind);
    void ComputeOffsets(SymbolTable* table, int startOffset = 0);
    void ComputeClassOffsets(ClassTableEntry* classEntry);

    bool ClassHasOffsets(ClassTableEntry* classEntry);

    void CreateTempVar(TempVarNodeBase* node);
    void CreateReference(RefVarNode* node);

    SymbolTable* m_globalTable;

    std::list<ASTNode*> m_toRevisit;
    std::list<ClassTableEntry*> m_classesWithOffsets;
    TagGenerator m_funcTagGen;
};

class CodeGenerator : public Visitor
{
public:
    CodeGenerator(SymbolTable* globalTable, const std::string& filepath);
    virtual void Visit(DotNode* element) override;
    virtual void Visit(ExprNode* element) override;
    virtual void Visit(BaseBinaryOperator* element) override;
    virtual void Visit(VarDeclNode* element) override;
    virtual void Visit(FuncCallNode* element) override;
    virtual void Visit(FunctionDefNode* element) override;
    virtual void Visit(MemFuncDefNode* element) override;
    virtual void Visit(ConstructorDefNode* element) override;
    virtual void Visit(IfStatNode* element) override;
    virtual void Visit(WhileStatNode* element) override;
    virtual void Visit(AssignStatNode* element) override;
    virtual void Visit(WriteStatNode* element) override;
    virtual void Visit(ReadStatNode* element) override;
    virtual void Visit(ReturnStatNode* element) override;
    virtual void Visit(ProgramNode* element) override;


    void OutputCode() const;

private:
    // moves the stack pointer by the provided frameSize 
    std::string IncrementStackFrame(size_t frameSize);
    std::string DecrementStackFrame(size_t frameSize);

    std::string GenerateModifiedExpr(ModifiedExpr* modExpr);
    std::string GenerateMinusExpr(ModifiedExpr* modExpr);

    std::string GenerateBinaryOp(BaseBinaryOperator* opNode);    
    std::string GenerateBinaryOpInt(BaseBinaryOperator* opNode);    
    std::string GenerateBinaryOpFloat(BaseBinaryOperator* opNode);  

    // code generation for different binary operators
    std::string GenerateAddOpInt(BaseBinaryOperator* opNode);
    std::string GenerateSubOpInt(BaseBinaryOperator* opNode);
    std::string GenerateMultOpInt(BaseBinaryOperator* opNode);
    std::string GenerateDivOpInt(BaseBinaryOperator* opNode);

    std::string GenerateOrInt(BaseBinaryOperator* opNode);
    std::string GenerateAndInt(BaseBinaryOperator* opNode);
    std::string GenerateNotInt(ModifiedExpr* expr);
    std::string GenerateEqualInt(BaseBinaryOperator* opNode);
    std::string GenerateNotEqualInt(BaseBinaryOperator* opNode);
    std::string GenerateLessThanInt(BaseBinaryOperator* opNode);
    std::string GenerateGreaterThanInt(BaseBinaryOperator* opNode);
    std::string GenerateLessOrEqualInt(BaseBinaryOperator* opNode);
    std::string GenerateGreaterOrEqualInt(BaseBinaryOperator* opNode);

    std::string GenerateArithmeticOpInt(BaseBinaryOperator* opNode, const char* commandName);
    std::string GenerateAndOrInt(BaseBinaryOperator* opNode, const char* commandName);
    std::string GenerateRelOpInt(BaseBinaryOperator* opNode, const char* commandName);

    std::string GenerateAddOpFloat(BaseBinaryOperator* opNode);
    std::string GenerateSubOpFloat(BaseBinaryOperator* opNode);
    std::string GenerateMultOpFloat(BaseBinaryOperator* opNode);
    std::string GenerateDivOpFloat(BaseBinaryOperator* opNode);

    std::string GenerateAddSubFloat(BaseBinaryOperator* opNode, const char* commandName);

    // returns the string of code to stores the value of the node and stores the register in which 
    //the data was stored in the provided outRegister field or NullRegister if the operation 
    //is not supported
    std::string ComputeVal(ASTNode* node, RegisterID& outRegister);
    std::string ComputeVal(BaseBinaryOperator* node, RegisterID& outRegister);
    std::string ComputeVal(FuncCallNode* node, RegisterID& outRegister);
    std::string ComputeVal(ExprNode* node, RegisterID& outRegister);
    std::string ComputeVal(ModifiedExpr* node, RegisterID& outRegister);

    std::string LoadVarInRegister(ASTNode* n, RegisterID& outRegister);
    std::string LoadVarInRegister(LiteralNode* node, RegisterID& outRegister);
    std::string LoadVarInRegister(VariableNode* node, RegisterID& outRegister);
    std::string LoadVarInRegister(DotNode* node, RegisterID& outRegister);
    std::string LoadVarInRegister(FuncCallNode* node, RegisterID& outRegister);
    std::string LoadVarInRegister(ExprNode* tempVar, RegisterID& outRegister);
    std::string LoadVarInRegister(TempVarNodeBase* tempVar, RegisterID& outRegister);

    std::string LoadFloatToAddr(int offset, ASTNode* expr);
    std::string LoadFloatToAddr(int offset, ExprNode* expr);
    std::string LoadFloatToAddr(int offset, VariableNode* var);
    std::string LoadFloatToAddr(int offset, LiteralNode* floatLiteral);

    std::string GenerateFunc(TagTableEntry* funcEntry, const std::string& statBlock, const std::string& fparamStr);

    std::string WriteNum(RegisterID numRegister, size_t frameSize);

    std::string CopyData(int dataOffset, size_t dataSize, int destOffset);
    std::string CopyData(SymbolTableEntry* data, SymbolTableEntry* dest);
    std::string CopyDataAtRef(RegisterID dataAddress, size_t dataSize, int destOffset);
    std::string CopyDataIntoRef(RegisterID value, RegisterID destAddr);
    std::string CopyDataIntoRef(SymbolTableEntry* data, RegisterID destAddr);

    std::string GetNumDigitsInNum(RegisterID num, RegisterID& outNumDigits);

    std::string ComputeOffsetAtRuntime(VariableNode* var, RegisterID& outRegister, SymbolTable* context = nullptr);
    std::string ComputeOffsetAtRuntime(DotNode* node, RegisterID& outRegister, SymbolTable* context = nullptr);

    std::string& GetCurrStatBlock(ASTNode* node);
    size_t GetCurrFrameSize(ASTNode* node);

    std::string PrepareToCallFunc(FuncCallNode* funcCall, TagTableEntry* funcEntry, RegisterID& prevStackTop);
    std::string HandleParametersForFuncCall(AParamListNode* aparams, TagTableEntry* funcEntry, RegisterID prevStackTop);
    std::string CallFunc(FuncCallNode* funcCall);
    std::string CallMemFunc(FuncCallNode* funcCall, SymbolTable* context, int selfOffset);
    std::string CallMemFunc(DotNode* dotExpr);
    std::string CallConstructor(VarDeclNode* decl);
    std::string HandleFuncReturnVal(FuncCallNode* funcCall, SymbolTableEntry* funcEntry = nullptr);


    std::string m_filepath;
    std::string m_dataCode;
    std::string m_executableCode;
    //std::string m_currStatBlockCode;

    RegisterID m_topOfStackRegister;
    RegisterID m_zeroRegister; // register with value 0 not necessarily r0
    RegisterID m_jumpReturnRegister;
    RegisterID m_returnValRegister;
    std::list<RegisterID> m_registerStack;
    std::unordered_map<ASTNode*, std::string> m_statBlocks;

    TagGenerator m_tagGen;
};