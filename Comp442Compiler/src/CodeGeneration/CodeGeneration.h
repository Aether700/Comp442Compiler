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
    static size_t GetIntStrSize();
    static int GetIntStrArg1Offset();
    static int GetIntStrArg2Offset();

    static int GetPutStrArg1Offset();

    static size_t GetAddressSize();
    static size_t GetIntSize();
    static size_t GetFloatSize();
    

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

    void CreateTempVar(TempVarNodeBase* node);

    SymbolTable* m_globalTable;

    std::list<ASTNode*> m_toRevisit;
    std::list<ClassTableEntry*> m_classesWithOffsets;
};

class CodeGenerator : public Visitor
{
public:
    CodeGenerator(const std::string& filepath);
    virtual void Visit(ModifiedExpr* element) override;
    virtual void Visit(BaseBinaryOperator* element) override;
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

    void GenerateMinusExpr(ModifiedExpr* modExpr);

    // code generation for different binary operators
    void GenerateAddOp(BaseBinaryOperator* opNode);
    void GenerateSubOp(BaseBinaryOperator* opNode);
    void GenerateMultOp(BaseBinaryOperator* opNode);
    void GenerateDivOp(BaseBinaryOperator* opNode);

   
    void GenerateOr(BaseBinaryOperator* opNode);
    void GenerateAnd(BaseBinaryOperator* opNode);
    void GenerateNot(ModifiedExpr* expr);
    void GenerateEqual(BaseBinaryOperator* opNode);

    /*
    void GenerateNotEqual(BaseBinaryOperator* opNode);
    void GenerateLessThan(BaseBinaryOperator* opNode);
    void GenerateGreaterThan(BaseBinaryOperator* opNode);
    void GenerateLessOrEqual(BaseBinaryOperator* opNode);
    void GenerateGreaterOrEqual(BaseBinaryOperator* opNode);
    */
    
    


    void GenerateArithmeticOp(BaseBinaryOperator* opNode, const char* commandName);
    void GenerateAndOr(BaseBinaryOperator* opNode, const char* commandName);

    // returns the string of code to stores the value of the node and stores the register in which 
    //the data was stored in the provided outRegister field or NullRegister if the operation 
    //is not supported
    std::string StoreValInRegister(ASTNode* node, RegisterID& outRegister);
    std::string StoreValInRegister(LiteralNode* node, RegisterID& outRegister);
    std::string StoreValInRegister(VariableNode* node, RegisterID& outRegister);
    std::string StoreValInRegister(BaseBinaryOperator* node, RegisterID& outRegister);
    std::string StoreValInRegister(ExprNode* node, RegisterID& outRegister);
    std::string StoreValInRegister(ModifiedExpr* node, RegisterID& outRegister);

    std::string LoadTempVarInRegister(TempVarNodeBase* tempVar, RegisterID& outRegister);

    template<typename NodeType>
    bool TempVarAlreadyOnStack(NodeType* node)
    {
        for (ASTNode* n : m_generatedTempVarNode)
        {
            if (n == node)
            {
                return true;
            }
        }
        return false;
    }

    RegisterID m_topOfStackRegister;
    RegisterID m_zeroRegister; // register with value 0 not necessarily r0
    RegisterID m_jumpReturnRegister;
    RegisterID m_returnValRegister;
    std::list<RegisterID> m_registerStack;
    std::unordered_map<SymbolTable*, size_t> m_tempVarUsed;
    std::list<ASTNode*> m_generatedTempVarNode;
    TagGenerator m_tagGen;
};