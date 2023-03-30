#include "ASTUtil.h"
#include "../Core/Core.h"
#include "../SemanticChecking/SemanticErrors.h"
#include "../CodeGeneration/CodeGeneration.h"

#include <vector>

// helpers //////////////////////////////////////////////////////////////

std::string VarDeclToTypeStr(VarDeclNode* var)
{
    ASSERT(var != nullptr);
    std::stringstream ss;
    ss << var->GetType()->GetType().GetLexeme();

    if (var->GetDimension() != nullptr)
    {
        for (ASTNode* dimension : *var->GetDimension())
        {
            LiteralNode* literal = dynamic_cast<LiteralNode*>(dimension);
            if (literal != nullptr)
            {
                ss << "[" << literal->GetLexemeNode()->GetID().GetLexeme() << "]";
            }
            else
            {
                ASSERT(dynamic_cast<UnspecificedDimensionNode*>(dimension) != nullptr);
                ss << "[]";
            }
        }
    }
    return ss.str();
}


std::string FunctionParamTypeToStr(FParamListNode* params)
{
    std::stringstream typeStr;
    bool hasParam = false;
    for (ASTNode* param : *params)
    {
        VarDeclNode* var = dynamic_cast<VarDeclNode*>(param);
        ASSERT(var != nullptr);
        typeStr << VarDeclToTypeStr(var) << ", ";
        hasParam = true;
    }

    if (hasParam)
    {
        // remove last ", "
        std::string tempCopy = typeStr.str();
        std::string trimmedStr = tempCopy.substr(0, tempCopy.length() - 2);
        typeStr.str("");
        typeStr << trimmedStr;
    }

    return typeStr.str();
}

bool HasMatchingParameters(FParamListNode* fparam, AParamListNode* aparam)
{
    if (fparam->GetNumChild() != aparam->GetNumChild())
    {
        return false;
    }

    auto it = aparam->GetChildren().begin();
    for (ASTNode* f : fparam->GetChildren())
    {
        FParamNode* currParam = (FParamNode*)f;
        if (currParam->GetEvaluatedType() != (*it)->GetEvaluatedType())
        {
            return false;
        }
        it++;
    }

    return true;
}


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

bool IsRootDot(DotNode* dot)
{
    return dynamic_cast<DotNode*>(dot->GetParent()) == nullptr;
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

TagTableEntry* FindEntryForFuncCall(FuncCallNode* funcCall)
{
    const std::string& funcName = funcCall->GetID()->GetID().GetLexeme();
    AParamListNode* aparams = funcCall->GetParameters();
    SymbolTable* table = funcCall->GetSymbolTable();
    SymbolTableEntry* tempEntry = table->FindEntryInScope(funcName);
    ASSERT(tempEntry != nullptr);
    table = tempEntry->GetParentTable();

    for (SymbolTableEntry* currEntry : *table)
    {
        if (currEntry->GetKind() == SymbolTableEntryKind::ConstructorDecl 
            || currEntry->GetKind() == SymbolTableEntryKind::FreeFunction 
            || currEntry->GetKind() == SymbolTableEntryKind::MemFuncDecl)
        {
            FunctionDefNode* funcNode = (FunctionDefNode*)currEntry->GetNode();
            if (currEntry->GetName() == funcName && HasMatchingParameters(funcNode->GetParameters(), aparams))
            {
                return (TagTableEntry*)currEntry;
            }
        }
    }
    return nullptr;
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
    SymbolTableEntry* entry = context->FindEntryInScope(name);
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
    else if (dynamic_cast<RefVarNode*>(node) != nullptr)
    {
        return GetOffset((RefVarNode*)node);
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
    if (dynamic_cast<VariableNode*>(assign->GetLeft()) != nullptr)
    {
        return GetOffset((VariableNode*)assign->GetLeft());
    }
    else if (dynamic_cast<DotNode*>(assign->GetLeft()) != nullptr)
    {
        return GetOffsetOfExpr((DotNode*)assign->GetLeft());
    }
    else
    {
        DEBUG_BREAK();
    }
    return INTMAX_MAX;
}

int GetOffset(SymbolTable* context, ITempVarNode* tempVarNode)
{
    return GetOffset(context, tempVarNode->GetTempVarName());
}

int GetOffset(RefVarNode* refNode)
{
    return GetOffset(refNode->GetSymbolTable(), refNode->GetRefVarName());
}

int GetOffset(TempVarNodeBase* tempVar)
{
    return GetOffset(tempVar->GetSymbolTable(), tempVar->GetTempVarName());
}

int GetOffsetOfExpr(DotNode* dotExpr)
{
    return GetOffset(dotExpr->GetLeft()) + GetInternalOffsetOfExpr(dotExpr);
}

int GetInternalOffsetOfExpr(DotNode* dotExpr)
{
    SymbolTable* global = GetGlobalTable(dotExpr->GetSymbolTable());
    SymbolTableEntry* classEntry = global->FindEntryInTable(dotExpr->GetLeft()->GetEvaluatedType());
    return GetOffsetOfRight(dotExpr->GetRight(), classEntry->GetSubTable());
}

int GetOffsetOfRight(ASTNode* rightOfDotExpr, SymbolTable* context)
{
    if (dynamic_cast<VariableNode*>(rightOfDotExpr) != nullptr)
    {
        VariableNode* var = (VariableNode*)rightOfDotExpr;
        return GetOffset(context, var->GetVariable()->GetID().GetLexeme());
    }
    else if (dynamic_cast<DotNode*>(rightOfDotExpr) != nullptr)
    {
        DotNode* dot = (DotNode*)rightOfDotExpr;
        ASTNode* leftOfDot = dot->GetLeft();
        SymbolTable* newContext = nullptr;
        if (dynamic_cast<VariableNode*>(leftOfDot) != nullptr)
        {
            VariableNode* var = (VariableNode*)leftOfDot;
            auto temp = var->GetVariable()->GetID().GetLexeme();
            SymbolTable* global = GetGlobalTable(context);
            newContext = global->FindEntryInTable(var->GetEvaluatedType())->GetSubTable();
        }
        else if (dynamic_cast<FuncCallNode*>(leftOfDot) != nullptr)
        {
            FuncCallNode* var = (FuncCallNode*)leftOfDot;
            newContext = context->FindEntryInScope(var->GetID()->GetID().GetLexeme())->GetParentTable();
        }
        return GetOffsetOfRight(dot->GetRight(), newContext);
    }
    else
    {
        DEBUG_BREAK();
    }
    return INTMAX_MAX;
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

bool IsRefWhenParameter(const std::string& typeStr)
{
    if (IsArrayType(typeStr))
    {
        return true;
    }
    return typeStr != "integer" && typeStr != "float";
}

bool IsParam(SymbolTable* context, const std::string& name)
{
    return context->FindEntryInScope(name)->GetKind() == SymbolTableEntryKind::Parameter;
}