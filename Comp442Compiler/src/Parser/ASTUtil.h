#pragma once
#include "AST.h"
#include "../SemanticChecking/SymbolTable.h"

std::string BaseTypeOfArr(const std::string& arrTypeStr);
std::string VarDeclToTypeStr(VarDeclNode* var);
std::string FunctionParamTypeToStr(FParamListNode* params);

bool PrivateMemberIsAccessible(MemVarTableEntry* member, SymbolTable* callingContext);
bool PrivateMemberIsAccessible(MemFuncTableEntry* member, SymbolTable* callingContext);
bool HasMatchingParameters(FParamListNode* fparam, AParamListNode* aparam);

DotNode* FindRootDotNodeParent(ASTNode* node);
DotNode* FindFirstDotNodeParent(ASTNode* node);
bool HasDotForParent(ASTNode* node);
DotNode* GetRootDotNode(ASTNode* node);
DotNode* GetRootDotNode(DotNode* dot);
bool IsRootDot(DotNode* dot);
bool DotIsVar(DotNode* dot);
FuncCallNode* FindFirstFuncCallInDot(DotNode* dot);
bool DotHasFuncCall(DotNode* dot);

/* returns the context SymbolTable from the provided left side of a dot 
   expression or nullptr if the left side of the dot expression is invalid
*/
SymbolTable* GetContextTable(SymbolTable* global, SymbolTable* prevContext, 
    ASTNode* node, bool logErrors = true);

// tries to retrieve the context for the provided node which is 
// part of a dot expression. Returns nullptr if the dot expression is invalid
SymbolTable* RetrieveContextTableFromNodeInDotExpr(ASTNode* node, const std::string& name);

SymbolTable* GetGlobalTable(SymbolTable* currTable);

SymbolTable* GetContextTableFromName(SymbolTable* currContext, 
    DotNode* dot, const std::string& name);

TagTableEntry* FindEntryForFuncCall(FuncCallNode* funcCall, SymbolTable* context = nullptr);
TagTableEntry* FindConstructorEntry(SymbolTable* classTable, AParamListNode* params);

template<typename ParentNode>
bool HasNodeInParentTree(ASTNode* n)
{
    while (n->GetParent() != nullptr)
    {
        n = n->GetParent();
        if (dynamic_cast<ParentNode*>(n) != nullptr)
        {
            return true;
        }
    }
    return false;
}

StatBlockNode* GetParentStatBlock(ASTNode* node);

bool IsValidSelf(SymbolTable* contextTable, VariableNode* var);

bool IsArrayType(ASTNode* n);
bool IsArrayType(const std::string typeStr);

int GetOffset(SymbolTable* context, const std::string& name);
int GetOffset(ASTNode* node);
int GetOffset(LiteralNode* var);
int GetOffset(VariableNode* var);
int GetOffset(AssignStatNode* assign);
int GetOffset(SymbolTable* context, ITempVarNode* tempVarNode);
int GetOffset(RefVarNode* refNode);
int GetOffset(TempVarNodeBase* tempVarNode);

int GetOffsetOfExpr(DotNode* dotExpr);
int GetInternalOffsetOfExpr(DotNode* dotExpr);

int GetOffsetOfRight(ASTNode* rightOfDotExpr, SymbolTable* context);

// returns InvalidSize if the type provided is could not be computed 
size_t ComputeSize(TypeNode* type, DimensionNode* dimensions);
size_t ComputeSize(const std::string& typeStr);

// returns the size of a class object
size_t FindSize(SymbolTable* globalTable, const std::string& typeStr);

bool IsRefWhenParameter(const std::string& typeStr);
bool IsParam(SymbolTable* context, const std::string& name);

bool IsRef(ASTNode* node);