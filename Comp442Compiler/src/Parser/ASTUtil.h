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

bool IsValidSelf(SymbolTable* contextTable, VariableNode* var);

bool IsArrayType(const std::string typeStr);

int GetOffset(SymbolTable* context, const std::string& name);
int GetOffset(VariableNode* var);
int GetOffset(AssignStatNode* assign);
int GetOffset(SymbolTable* context, ITempVarNode* tempVarNode);
int GetOffset(TempVarNodeBase* tempVarNode);