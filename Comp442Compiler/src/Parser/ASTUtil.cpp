#include "ASTUtil.h"
#include "../Core/Core.h"
#include "../SemanticChecking/SemanticErrors.h"

// helpers //////////////////////////////////////////////////////////////

SymbolTable* FindNameInDot(SymbolTable* globalTable, SymbolTable* contextTable, 
    ASTNode* dotRemainder, const std::string& name)
{
    ASTNode* left;
    ASTNode* right;
    if (dynamic_cast<DotNode*>(dotRemainder) != nullptr)
    {
        DotNode* dot = (DotNode*)dotRemainder;
        left = dot->GetLeft();
        right = dot->GetRight();
    }
    else
    {
        left = dotRemainder;
        right = nullptr;
    }


    if (dynamic_cast<VariableNode*>(left) != nullptr)
    {
        VariableNode* var = (VariableNode*)left;

        if (var->GetVariable()->GetID().GetLexeme() == name)
        {
            return contextTable;
        }

        SymbolTableEntry* varEntry = contextTable->FindEntryInScope(var->
            GetVariable()->GetID().GetLexeme());

        if (varEntry == nullptr || (varEntry->GetKind() != SymbolTableEntryKind::MemVar 
            && varEntry->GetKind() != SymbolTableEntryKind::LocalVariable))
        {
            return nullptr;
        }

        if (right != nullptr)
        {
            SymbolTable* nextContext = GetContextTable(globalTable, contextTable, 
                left, false);
            if (nextContext != nullptr)
            {
                return FindNameInDot(globalTable, nextContext, right, name);
            }
        }
    }
    else if (dynamic_cast<FuncCallNode*>(left) != nullptr)
    {
        FuncCallNode* funcCall = (FuncCallNode*)left;
        
        if (funcCall->GetID()->GetID().GetLexeme() == name)
        {
            return contextTable;
        }

        if (contextTable->GetParentEntry()->GetKind() != SymbolTableEntryKind::Class)
        {
            SymbolTableEntry* funcEntry = contextTable->FindEntryInScope(funcCall->
                GetID()->GetID().GetLexeme());

            if (funcEntry == nullptr 
                || funcEntry->GetKind() != SymbolTableEntryKind::FreeFunction)
            {
                return nullptr;
            }
        }

        if (right != nullptr)
        {
            SymbolTable* nextContext = GetContextTable(globalTable, contextTable, 
                left, false);
            if (nextContext != nullptr)
            {
                return FindNameInDot(globalTable, nextContext, right, name);
            }
        }
    }
    else
    {
        DEBUG_BREAK(); // should never reach here
        return nullptr;
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////
DotNode* FindFirstDotNodeParent(ASTNode* node)
{
    ASTNode* parent = node->GetParent();
    while(parent != nullptr)
    {
        if (dynamic_cast<DotNode*>(parent) != nullptr)
        {
            return (DotNode*)parent;
        }
        parent = parent->GetParent();
    }
    return nullptr;
}

bool HasDotForParent(ASTNode* node) { return FindFirstDotNodeParent(node) != nullptr; }

DotNode* GetRootDotNode(ASTNode* node)
{
    DotNode* first = FindFirstDotNodeParent(node);
    if (first == nullptr)
    {
        return nullptr;
    }
    return GetRootDotNode(first);
}

DotNode* GetRootDotNode(DotNode* dot)
{
    ASTNode* curr = dot;
    while (dynamic_cast<DotNode*>(curr->GetParent()) != nullptr)
    {
        curr = curr->GetParent();
    }
    return (DotNode*)curr;
}

SymbolTable* GetContextTable(SymbolTable* globalTable, SymbolTable* prevContext, 
    ASTNode* node, bool logErrors)
{
    if (dynamic_cast<VariableNode*>(node) != nullptr)
    {
        VariableNode* var = (VariableNode*)node;
        SymbolTableEntry* entry = prevContext->FindEntryInScope(
            var->GetVariable()->GetID().GetLexeme());

        std::string type;

        if (entry == nullptr)
        {
            bool hasError = false;
            if (var->GetVariable()->GetID().GetLexeme() == "self")
            {
                if (IsValidSelf(prevContext, var))
                {
                    type = var->GetEvaluatedType();
                }
                else
                {
                    hasError = true;
                }
            }

            if (hasError)
            {
                if (logErrors)
                {
                    if (prevContext == globalTable 
                        || prevContext->GetParentEntry()->GetKind() 
                        != SymbolTableEntryKind::Class)
                    {
                        SemanticErrorManager::AddError(new UndeclaredSymbolError(
                            var->GetVariable()->GetID()));
                    }
                    else
                    {
                        SemanticErrorManager::AddError(new UnknownMemberError(
                            prevContext->GetName(), var->GetVariable()->GetID()));
                    }
                }
                return nullptr;
            }
        }
        else
        {
            type = entry->GetEvaluatedType();
        }

        SymbolTableEntry* classEntry = globalTable->FindEntryInTable(type);
        
        if (classEntry == nullptr || classEntry->GetKind() != SymbolTableEntryKind::Class)
        {
            if (logErrors)
            {
                SemanticErrorManager::AddError(new UnknownMemberError(prevContext->GetName(), 
                    var->GetVariable()->GetID()));
            }
            return nullptr;
        }

        return classEntry->GetSubTable();
    }
    else if (dynamic_cast<FuncCallNode*>(node) != nullptr)
    {
        FuncCallNode* funcCall = (FuncCallNode*)node;
        SymbolTableEntry* entry = prevContext->FindEntryInScope(
            funcCall->GetID()->GetID().GetLexeme());
        
        if (entry == nullptr)
        {
            if (logErrors)
            {
                if (prevContext == globalTable 
                    || prevContext->GetParentEntry()->GetKind() != SymbolTableEntryKind::Class)
                {
                    SemanticErrorManager::AddError(new UndeclaredSymbolError(
                        funcCall->GetID()->GetID()));
                }
                else
                {
                    SemanticErrorManager::AddError(new UnknownMemberError(
                        prevContext->GetName(), funcCall->GetID()->GetID()));
                }
            }
            return nullptr;
        }

        SymbolTableEntry* classEntry = globalTable->FindEntryInTable(
            entry->GetEvaluatedType());

        if (classEntry == nullptr || classEntry->GetKind() != SymbolTableEntryKind::Class)
        {
            if (logErrors)
            {
                SemanticErrorManager::AddError(new UnknownMemberError(prevContext->GetName(), 
                    funcCall->GetID()->GetID()));
            }
            return nullptr;
        }

        return classEntry->GetSubTable();
    }
    else
    {
        // should never reach here
        DEBUG_BREAK();
    }

    return nullptr;
}

SymbolTable* GetGlobalTable(SymbolTable* currTable)
{
    while (currTable->GetParentTable() != nullptr)
    {
        currTable = currTable->GetParentTable();
    }
    return currTable;
}

SymbolTable* GetContextTableFromName(SymbolTable* currContext, 
    DotNode* dot, const std::string& name)
{
    return FindNameInDot(GetGlobalTable(currContext), currContext, dot, name);
}

bool IsValidSelf(SymbolTable* contextTable, VariableNode* var)
{
    SymbolTableEntry* parentEntry = contextTable->GetParentEntry();
    return parentEntry != nullptr && (parentEntry->GetKind() 
        == SymbolTableEntryKind::ConstructorDecl 
        || parentEntry->GetKind() == SymbolTableEntryKind::MemFuncDecl);
}


bool IsArrayType(const std::string typeStr)
{
    for (char c : typeStr)
    {
        if (c == '[' || c == ']')
        {
            return true;
        }
    }
    return false;
}