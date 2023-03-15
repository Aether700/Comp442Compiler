#pragma once
#include "AST.h"
#include "../SemanticChecking/SymbolTable.h"


DotNode* FindFirstDotNodeParent(ASTNode* node);
bool HasDotForParent(ASTNode* node);
DotNode* GetRootDotNode(ASTNode* node);
DotNode* GetRootDotNode(DotNode* dot);

// returns nullptr if the node is invalid as a leftside of a dot expression
SymbolTable* GetContextTable(SymbolTable* global, SymbolTable* prevContext, 
    ASTNode* node, bool logErrors = true);

SymbolTable* GetGlobalTable(SymbolTable* currTable);

SymbolTable* GetContextTableFromName(SymbolTable* currContext, 
    DotNode* dot, const std::string& name);

bool IsArrayType(const std::string typeStr);