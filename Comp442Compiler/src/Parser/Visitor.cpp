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

std::string FunctionParamTypeToStr(FunctionDefNode* func)
{
    std::stringstream typeStr;
    bool hasParam = false;
    auto params = func->GetParameters();
    for (ASTNode* param : *func->GetParameters())
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

// SymbolTableAssembler ////////////////////////////////////////
SymbolTableAssembler::SymbolTableAssembler() : m_globalScopeTable(nullptr) { }

SymbolTableAssembler::~SymbolTableAssembler()
{
    for (SymbolTableEntry* entry : m_workingList)
    {
        delete entry;
    }
    delete m_globalScopeTable;
}

void SymbolTableAssembler::Visit(VarDeclNode* element)
{
    SymbolTableEntry* entry = new VarSymbolTableEntry(element,
        SymbolTableEntryKind::LocalVariable);

    m_workingList.push_front(entry);
}

void SymbolTableAssembler::Visit(FParamNode* element)
{
    SymbolTableEntry* entry = new VarSymbolTableEntry(element, 
        SymbolTableEntryKind::Parameter);

    m_workingList.push_front(entry);
}

void SymbolTableAssembler::Visit(FunctionDefNode* element)
{
    SymbolTable* functionTable = new SymbolTable("::" 
        + element->GetID()->GetID().GetLexeme());
    std::list<SymbolTableEntry*> entriesToKeep;
    for (SymbolTableEntry* entry : m_workingList)
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

    m_workingList.clear();
    m_workingList = entriesToKeep;

    SymbolTableEntry* entry = new FreeFuncTableEntry(element, 
        FunctionParamTypeToStr(element), functionTable);

    m_workingList.push_front(entry);
}

void SymbolTableAssembler::Visit(MemVarNode* element)
{
    MemVarTableEntry* entry = new MemVarTableEntry(element);
    m_workingList.push_front(entry);
}

void SymbolTableAssembler::Visit(ClassDefNode* element)
{
    const std::string& className = element->GetID()->GetID().GetLexeme();
    SymbolTable* classTable = new SymbolTable(className);

    std::list<MemVarTableEntry*> memVarEntries;

    std::list<SymbolTableEntry*> entriesToKeep;
    for (SymbolTableEntry* entry : m_workingList)
    {
        if (entry->GetKind() == SymbolTableEntryKind::MemVar)
        {
            MemVarTableEntry* memVarEntry = (MemVarTableEntry*)entry;
            if (memVarEntry->GetClassID() == className)
            {
                memVarEntries.push_front(memVarEntry);
            }
            else
            {
                entriesToKeep.push_front(entry);
            }
        }
        else if (entry->GetKind() != SymbolTableEntryKind::MemFunc && entry->GetKind() 
            != SymbolTableEntryKind::Constructor)
        {
            entriesToKeep.push_front(entry);
        }
    }

    m_workingList.clear();
    m_workingList = entriesToKeep;


    for (MemVarTableEntry* memVarEntry : memVarEntries)
    {
        classTable->AddEntry(memVarEntry);
    }
    SymbolTableEntry* classEntry = new ClassTableEntry(element, classTable);

    m_workingList.push_front(classEntry);
}

void SymbolTableAssembler::Visit(ProgramNode* element)
{
    m_globalScopeTable = new SymbolTable("Global");

    for (SymbolTableEntry* entry : m_workingList)
    {
        m_globalScopeTable->AddEntry(entry);
    }
    
    m_workingList.clear();
}

SymbolTable* SymbolTableAssembler::GetGlobalSymbolTable() { return m_globalScopeTable; }