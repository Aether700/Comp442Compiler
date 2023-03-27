#include "ASTUtil.h"
#include "../Core/Core.h"
#include "../SemanticChecking/SemanticErrors.h"
#include "../CodeGeneration/CodeGeneration.h"

#include <vector>

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
DotNode* FindRootDotNodeParent(ASTNode* node)
{
    ASSERT(HasDotForParent(node));
    ASTNode* currNode = node;
    while (HasDotForParent(currNode))
    {
        currNode = currNode->GetParent();
    }
    ASSERT(dynamic_cast<DotNode*>(currNode) != nullptr);
    return (DotNode*)currNode;
}

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
                SemanticErrorManager::AddError(
                    new InvalidDotOperatorUsageError(var->GetVariable()->GetID()));
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
        auto s = node->ToString();
        DEBUG_BREAK();
    }

    return nullptr;
}

SymbolTable* RetrieveContextTableFromNodeInDotExpr(ASTNode* node, const std::string& name)
{
    if (!HasDotForParent(node))
    {
        return nullptr;
    }

    DotNode* currDot = FindRootDotNodeParent(node);
    SymbolTable* currContext = currDot->GetSymbolTable();
    return FindNameInDot(GetGlobalTable(currContext), currContext, currDot, name);
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

StatBlockNode* GetParentStatBlock(ASTNode* node)
{
    ASTNode* curr = node->GetParent();
    while (dynamic_cast<StatBlockNode*>(curr) == nullptr)
    {
        if (curr == nullptr || curr->GetParent() == nullptr)
        {
            return nullptr;
        }
        curr = curr->GetParent();
    }
    return dynamic_cast<StatBlockNode*>(curr);
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

int GetOffset(SymbolTable* context, const std::string& name)
{
    SymbolTableEntry* entry = context->FindEntryInTable(name);
    ASSERT(entry != nullptr);
    return entry->GetOffset();
}

int GetOffset(ASTNode* node)
{
    if (dynamic_cast<VariableNode*>(node) != nullptr)
    {
        return GetOffset((VariableNode*)node);
    }
    else if (dynamic_cast<AssignStatNode*>(node) != nullptr)
    {
        return GetOffset((AssignStatNode*)node);
    }
    else if (dynamic_cast<TempVarNodeBase*>(node) != nullptr)
    {
        return GetOffset((TempVarNodeBase*)node);
    }
    else if (dynamic_cast<LiteralNode*>(node) != nullptr)
    {
        return GetOffset((LiteralNode*)node);
    }
    else
    {
        DEBUG_BREAK();
    }
    return INTMAX_MAX;
}

int GetOffset(LiteralNode* node)
{
    ASSERT(node->GetEvaluatedType() == "float");
    return GetOffset(node->GetSymbolTable(), node->GetTempVarName());
}

int GetOffset(VariableNode* var)
{
    VarDeclNode* declNode = (VarDeclNode*)var->GetSymbolTable()->FindEntryInScope(var->
        GetVariable()->GetID().GetLexeme())->GetNode();
    if (!IsArrayType(declNode->GetEvaluatedType()))
    {
        return GetOffset(var->GetSymbolTable(), var->GetVariable()->GetID().GetLexeme());
    }
    int varOffset = GetOffset(var->GetSymbolTable(), var->GetVariable()->GetID().GetLexeme());
    size_t baseTypeSize = ComputeSize(declNode->GetType(), nullptr);
    size_t numDimensions = declNode->GetDimension()->GetNumChild();

    std::vector<size_t> dimensions;
    dimensions.reserve(numDimensions);

    for (ASTNode* baseNode : declNode->GetDimension()->GetChildren())
    {
        if (dynamic_cast<LiteralNode*>(baseNode) != nullptr)
        {
            LiteralNode* lit = (LiteralNode*)baseNode;
            int dimension = std::stoi(lit->GetLexemeNode()->GetID().GetLexeme());
            dimensions.push_back((size_t)dimension);
        }
    }

    /*
    in[2][3][6]
    in[3][0][1] -> 3 * (in * 3 * 6) + 0 * (in * 6) + 1 * in
    */

    int internalOffset = 0;
    size_t dimensionIndex = 1; // skip the first dimension 
    for (ASTNode* baseNode : var->GetDimension()->GetChildren())
    {
        if (dynamic_cast<LiteralNode*>(baseNode) != nullptr)
        {
            LiteralNode* lit = (LiteralNode*)baseNode;
            int index = std::stoi(lit->GetLexemeNode()->GetID().GetLexeme());
            int currOffset = (int)baseTypeSize * -1;
            for (size_t i = dimensionIndex; i < dimensions.size(); i++)
            {
                currOffset *= (int)dimensions[i];
            }
            dimensionIndex++;
            internalOffset += index * currOffset;
        }
    }

    return varOffset + internalOffset;

    /*
    // temp
    //for now assume one dimension arrays only and only literal nodes
    ASSERT(numDimensions == 1);

    LiteralNode* l = (LiteralNode*)var->GetDimension()->GetChildren().front();
    int index = std::stoi(l->GetLexemeNode()->GetID().GetLexeme());
    return varOffset - (index * ComputeSize(declNode->GetType(), nullptr));
    //////////////////////////////////////////
    */
}

int GetOffset(AssignStatNode* assign)
{
    // currently assumes that the assignment is made to a simple variable
    return GetOffset((VariableNode*)assign->GetLeft());
}

int GetOffset(SymbolTable* context, ITempVarNode* tempVarNode)
{
    return GetOffset(context, tempVarNode->GetTempVarName());
}

int GetOffset(TempVarNodeBase* tempVar)
{
    auto temp = tempVar->ToString();
    return GetOffset(tempVar->GetSymbolTable(), tempVar->GetTempVarName());
}

size_t ComputeSize(TypeNode* type, DimensionNode* dimensions)
{
    const std::string& typeStr = type->GetType().GetLexeme();
    size_t baseSize;
    if (typeStr == "integer")
    {
        baseSize = PlatformSpecifications::GetIntSize();
    }
    else if (typeStr == "float")
    {
        baseSize = PlatformSpecifications::GetFloatSize();
    }
    else if (typeStr == "bool")
    {
        baseSize = PlatformSpecifications::GetFloatSize();
    }
    else
    {
        baseSize = FindSize(GetGlobalTable(type->GetSymbolTable()), typeStr);
    }

    if (baseSize == InvalidSize)
    {
        return InvalidSize;
    }

    size_t totalSize = baseSize;
    if (dimensions != nullptr)
    {
        for (ASTNode* baseNode : dimensions->GetChildren())
        {
            if (dynamic_cast<UnspecificedDimensionNode*>(baseNode) != nullptr)
            {
                // check what to do with unspecified nodes for now debug break
                DEBUG_BREAK();
            }
            else
            {
                LiteralNode* literal = (LiteralNode*)baseNode;
                ASSERT(literal->GetEvaluatedType() == "integer");
                int lexemInt = std::stoi(literal->GetLexemeNode()->GetID().GetLexeme());
                ASSERT(lexemInt > 0);
                totalSize *= ((size_t)lexemInt);
            }
        }
    }
    return totalSize;
}

size_t ComputeSize(const std::string& typeStr)
{
    if (typeStr == "integer")
    {
        return PlatformSpecifications::GetIntSize();
    }
    else if (typeStr == "float")
    {
        return PlatformSpecifications::GetFloatSize();
    }
    else
    {
        return InvalidSize;
    }
}

size_t FindSize(SymbolTable* globalTable, const std::string& typeStr)
{
    SymbolTableEntry* classEntry = globalTable->FindEntryInTable(typeStr);
    ASSERT(classEntry != nullptr);
    return classEntry->GetSize();
}