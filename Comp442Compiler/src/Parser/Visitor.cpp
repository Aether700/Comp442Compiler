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
    SymbolTable* functionTable = new SymbolTable(element->GetID()->GetID().GetLexeme());
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
        FunctionParamTypeToStr(element->GetParameters()), functionTable);

    m_workingList.push_front(entry);
}

void SymbolTableAssembler::Visit(MemVarNode* element)
{
    MemVarTableEntry* entry = new MemVarTableEntry(element);
    m_workingList.push_front(entry);
}

void SymbolTableAssembler::Visit(MemFuncDeclNode* element)
{
    // delete all parameters for this node since we will use 
    // the entries from the definition node
    std::list<SymbolTableEntry*> entriesToDelete;
    std::list<SymbolTableEntry*> entriesToKeep;
    
    for (SymbolTableEntry* entry : m_workingList)
    {
        if (entry->GetKind() == SymbolTableEntryKind::Parameter)
        {
            entriesToDelete.push_front(entry);
        }
        else
        {
            entriesToKeep.push_back(entry);
        }
    }

    m_workingList.clear();
    m_workingList = entriesToKeep;

    for (SymbolTableEntry* entry : entriesToDelete)
    {
        delete entry;
    }

    MemFuncTableEntry* entry = new MemFuncTableEntry(element, 
        FunctionParamTypeToStr(element->GetParameters()));
    TryMatchMemFuncDeclAndDef(entry);
    m_workingList.push_front(entry);
}

void SymbolTableAssembler::Visit(MemFuncDefNode* element)
{
    SymbolTable* functionTable;
    
    {
        const std::string& className = element->GetClassID()->GetID().GetLexeme();
        functionTable = new SymbolTable(className + "::" 
            + element->GetID()->GetID().GetLexeme());
    }

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

    MemFuncDefEntry* entry = new MemFuncDefEntry(element, 
        FunctionParamTypeToStr(element->GetParameters()), functionTable);
    
    if (!TryMatchMemFuncDeclAndDef(entry))
    {
        m_workingList.push_front(entry);
    }
}

void SymbolTableAssembler::Visit(InheritanceListNode* element)
{
    InheritanceListEntry* entry = new InheritanceListEntry(element);
    m_workingList.push_front(entry);
}

void SymbolTableAssembler::Visit(ClassDefNode* element)
{
    const std::string& className = element->GetID()->GetID().GetLexeme();
    SymbolTable* classTable = new SymbolTable(className);


    InheritanceListEntry* inheritanceList = nullptr;
    std::list<MemVarTableEntry*> memVarEntries;
    std::list<MemFuncTableEntry*> memFuncEntries;

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
        else if (entry->GetKind() == SymbolTableEntryKind::InheritanceList)
        {
            InheritanceListEntry* currInheritanceList = (InheritanceListEntry*)entry;
            if (currInheritanceList->GetClassID() == className)
            {
                ASSERT(inheritanceList == nullptr);
                inheritanceList = currInheritanceList;
            }
            else
            {
                entriesToKeep.push_front(entry);
            }
        }
        else if (entry->GetKind() == SymbolTableEntryKind::MemFuncDecl)
        {
            MemFuncTableEntry* memFunc = (MemFuncTableEntry*)entry;
            if (memFunc->GetClassID() == className)
            {
                memFuncEntries.push_front(memFunc);
            }
            else
            {
                entriesToKeep.push_front(entry);
            }
        }
        else 
        {
            entriesToKeep.push_front(entry);
        }
    }

    m_workingList.clear();
    m_workingList = entriesToKeep;

    classTable->AddEntry(inheritanceList);

    for (MemVarTableEntry* memVarEntry : memVarEntries)
    {
        classTable->AddEntry(memVarEntry);
    }

    for (MemFuncTableEntry* memFuncEntry : memFuncEntries)
    {
        classTable->AddEntry(memFuncEntry);
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

bool SymbolTableAssembler::TryMatchMemFuncDeclAndDef(MemFuncTableEntry* decl)
{
    for (auto it = m_workingList.begin(); it != m_workingList.end(); it++)
    {
        MemFuncDefEntry* def = dynamic_cast<MemFuncDefEntry*>(*it);
        if (def != nullptr)
        {
            if (decl->GetName() == def->GetName() 
                && decl->GetClassID() == def->GetClassID() 
                && decl->GetParamTypes() == def->GetParamTypes() 
                && decl->GetReturnType() == def->GetReturnType())
            {
                decl->SetDefinition(def);
                m_workingList.erase(it);
                delete def;
                return true;
            }
        }
    }
    return false;
}

bool SymbolTableAssembler::TryMatchMemFuncDeclAndDef(MemFuncDefEntry* def)
{
    for (SymbolTableEntry* entry : m_workingList)
    {
        ClassTableEntry* classDefEntry = dynamic_cast<ClassTableEntry*>(entry);
        if (classDefEntry != nullptr)
        {
            for (SymbolTableEntry* classEntry : *classDefEntry->GetSubTable())
            {
                MemFuncTableEntry* decl = dynamic_cast<MemFuncTableEntry*>(classEntry);
                if (decl != nullptr && decl->GetName() == def->GetName() 
                    && decl->GetParamTypes() == def->GetParamTypes() 
                    && decl->GetReturnType() == def->GetReturnType())
                {
                    decl->SetDefinition(def);
                    return true;
                }
            }
            break;
        }
    }
    return false;
}