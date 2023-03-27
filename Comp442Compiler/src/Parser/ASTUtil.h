#pragma once
#include "AST.h"
#include "../SemanticChecking/SymbolTable.h"

DotNode* FindRootDotNodeParent(ASTNode* node);
DotNode* FindFirstDotNodeParent(ASTNode* node);
bool HasDotForParent(ASTNode* node);
DotNode* GetRootDotNode(ASTNode* node);
DotNode* GetRootDotNode(DotNode* dot);

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

bool IsArrayType(const std::string typeStr);

int GetOffset(SymbolTable* context, const std::string& name);
int GetOffset(ASTNode* node);
int GetOffset(VariableNode* var);
int GetOffset(AssignStatNode* assign);
int GetOffset(SymbolTable* context, ITempVarNode* tempVarNode);
int GetOffset(TempVarNodeBase* tempVarNode);

// returns InvalidSize if the type provided is could not be computed 
size_t ComputeSize(TypeNode* type, DimensionNode* dimensions);
size_t ComputeSize(const std::string& typeStr);

// returns the size of a class object
size_t FindSize(SymbolTable* globalTable, const std::string& typeStr);