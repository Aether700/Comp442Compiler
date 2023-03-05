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
        SymbolTableEntryKind::Variable, VarDeclToTypeStr(element));

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
            need to change entry kind somewhere around here (currently always variable)
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
    for (SymbolTableEntry* entry : m_stack)
    {
        functionTable->AddEntry(entry);
    }

    m_stack.clear();

    SymbolTableEntry* entry = new SymbolTableEntry(element->GetID()->GetID().GetLexeme(), 
        SymbolTableEntryKind::Function, typeStr.str(), functionTable);

    m_stack.push_front(entry);
}