#include "Visitor.h"
#include "AST.h"
#include "../Core/Core.h"

#include <sstream>

// Helpers /////////////////////////////////////////////////////

std::string VarDeclToTypeStr(VarDeclNode* var)
{
    ASSERT(var != nullptr);
    std::stringstream ss;
    ss << var->GetType()->GetType().GetLexeme();
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
    return ss.str();
}

// SymbolTableAssembler ////////////////////////////////////////
SymbolTableAssembler::~SymbolTableAssembler()
{
    for (SymbolTableEntry* entry : m_stack)
    {
        delete entry;
    }
}

void SymbolTableAssembler::Visit(VarDeclNode* element)
{
    SymbolTableEntry* entry = new SymbolTableEntry(element->GetID()->GetID().GetLexeme(),
        SymbolTableEntryKind::LocalVariable, VarDeclToTypeStr(element));

    m_stack.push_front(entry);
}

void SymbolTableAssembler::Visit(FParamNode* element)
{
    SymbolTableEntry* entry = new SymbolTableEntry(element->GetID()->GetID().GetLexeme(),
        SymbolTableEntryKind::Parameter, VarDeclToTypeStr(element));

    m_stack.push_front(entry);
}

void SymbolTableAssembler::Visit(FunctionDefNode* element)
{
    std::stringstream typeStr;
    typeStr << element->GetReturnType()->GetType().GetLexeme() << ": ";

    {
        bool hasParam = false;
        auto params = element->GetParameters();
        for (ASTNode* param : *element->GetParameters())
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
            typeStr.str(trimmedStr);
        }
    }

    SymbolTable* functionTable = new SymbolTable();
    std::list<SymbolTableEntry*> entriesToKeep;
    for (SymbolTableEntry* entry : m_stack)
    {
        if (entry->GetKind() == SymbolTableEntryKind::LocalVariable 
            || entry->GetKind() == SymbolTableEntryKind::Parameter)
        {
            functionTable->AddEntry(entry);
        }
        else
        {
            entriesToKeep.push_back(entry);
        }
    }

    m_stack.clear();
    m_stack = entriesToKeep;

    SymbolTableEntry* entry = new SymbolTableEntry(element->GetID()->GetID().GetLexeme(), 
        SymbolTableEntryKind::FreeFunction, typeStr.str(), functionTable);

    m_stack.push_front(entry);
}