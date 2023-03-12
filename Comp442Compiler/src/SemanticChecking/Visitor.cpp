#include "Visitor.h"
#include "SemanticErrors.h"
#include "../Parser/AST.h"
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
    SymbolTableEntry* entry = new VarSymbolTableEntry(element, VarDeclToTypeStr(element),
        SymbolTableEntryKind::LocalVariable);

    m_workingList.push_front(entry);
}

void SymbolTableAssembler::Visit(FParamNode* element)
{
    SymbolTableEntry* entry = new VarSymbolTableEntry(element, VarDeclToTypeStr(element),
        SymbolTableEntryKind::Parameter);

    m_workingList.push_front(entry);
}

void SymbolTableAssembler::Visit(FunctionDefNode* element)
{
    SymbolTable* functionTable = new SymbolTable(element->GetID()->GetID().GetLexeme());
    std::list<SymbolTableEntry*> entriesToKeep;
    std::list<SymbolTableEntry*> toDelete;
    for (SymbolTableEntry* entry : m_workingList)
    {
        if (entry->GetKind() == SymbolTableEntryKind::LocalVariable 
            || entry->GetKind() == SymbolTableEntryKind::Parameter)
        {
            SymbolTableEntry* originalEntry = functionTable->AddEntryFirst(entry);
            if (originalEntry != nullptr)
            {
                VarDeclNode* originalNode = (VarDeclNode*)originalEntry->GetNode();
                VarDeclNode* errorNode = (VarDeclNode*)entry->GetNode();
                SemanticErrorManager::AddError(
                    new DuplicateSymbolError(originalNode->GetID()->GetID(), 
                    errorNode->GetID()->GetID()));
                toDelete.push_front(entry);
            }
        }
        else
        {
            entriesToKeep.push_back(entry);
        }
    }

    m_workingList.clear();
    m_workingList = entriesToKeep;

    for (SymbolTableEntry* entry : toDelete)
    {
        delete entry;
    }

    FreeFuncTableEntry* entry = new FreeFuncTableEntry(element, 
        FunctionParamTypeToStr(element->GetParameters()), functionTable);

    element->SetSymbolTable(functionTable);

    m_workingList.push_front(entry);
}

void SymbolTableAssembler::Visit(MemVarNode* element)
{
    MemVarTableEntry* entry = new MemVarTableEntry(element, VarDeclToTypeStr(element));
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
    std::list<SymbolTableEntry*> toDelete;
    for (SymbolTableEntry* entry : m_workingList)
    {
        if (entry->GetKind() == SymbolTableEntryKind::LocalVariable 
            || entry->GetKind() == SymbolTableEntryKind::Parameter)
        {
            SymbolTableEntry* originalEntry = functionTable->AddEntryFirst(entry);
            if (originalEntry != nullptr)
            {
                VarDeclNode* originalNode = (VarDeclNode*)originalEntry->GetNode();
                VarDeclNode* errorNode = (VarDeclNode*)entry->GetNode();
                SemanticErrorManager::AddError(
                    new DuplicateSymbolError(originalNode->GetID()->GetID(), 
                    errorNode->GetID()->GetID()));
                toDelete.push_front(entry);
            }
        }
        else
        {
            entriesToKeep.push_back(entry);
        }
    }

    m_workingList.clear();
    m_workingList = entriesToKeep;

    for (SymbolTableEntry* entry : toDelete)
    {
        delete entry;
    }

    MemFuncDefEntry* entry = new MemFuncDefEntry(element, 
        FunctionParamTypeToStr(element->GetParameters()), functionTable);
    
    element->SetSymbolTable(functionTable);

    if (!TryMatchMemFuncDeclAndDef(entry))
    {
        // no function declaration here
        DEBUG_BREAK();
    }
}

void SymbolTableAssembler::Visit(ConstructorDeclNode* element)
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

    ConstructorTableEntry* entry = new ConstructorTableEntry(element, 
        FunctionParamTypeToStr(element->GetParameters()));
    m_workingList.push_front(entry);
}

void SymbolTableAssembler::Visit(ConstructorDefNode* element)
{
    SymbolTable* functionTable;
    
    {
        const std::string& className = element->GetID()->GetID().GetLexeme();
        functionTable = new SymbolTable(className + "::constructor");
    }

    std::list<SymbolTableEntry*> entriesToKeep;
    std::list<SymbolTableEntry*> toDelete;
    for (SymbolTableEntry* entry : m_workingList)
    {
        if (entry->GetKind() == SymbolTableEntryKind::LocalVariable 
            || entry->GetKind() == SymbolTableEntryKind::Parameter)
        {
            SymbolTableEntry* originalEntry = functionTable->AddEntryFirst(entry);
            if (originalEntry != nullptr)
            {
                VarDeclNode* originalNode = (VarDeclNode*)originalEntry->GetNode();
                VarDeclNode* errorNode = (VarDeclNode*)entry->GetNode();
                SemanticErrorManager::AddError(
                    new DuplicateSymbolError(originalNode->GetID()->GetID(), 
                    errorNode->GetID()->GetID()));
                toDelete.push_front(entry);
            }
        }
        else
        {
            entriesToKeep.push_back(entry);
        }
    }

    m_workingList.clear();
    m_workingList = entriesToKeep;

    for (SymbolTableEntry* entry : toDelete)
    {
        delete entry;
    }

    ConstructorDefEntry* entry = new ConstructorDefEntry(element, 
        FunctionParamTypeToStr(element->GetParameters()), functionTable);
    
    element->SetSymbolTable(functionTable);

    if (!TryMatchMemFuncDeclAndDef(entry))
    {
        // no function declaration here
        DEBUG_BREAK();
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
    std::list<ConstructorTableEntry*> constructorEntries;

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
        else if (entry->GetKind() == SymbolTableEntryKind::ConstructorDecl)
        {
            ConstructorTableEntry* constructor = (ConstructorTableEntry*)entry;
            if (constructor->GetClassID() == className)
            {
                constructorEntries.push_front(constructor);
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

    std::list<SymbolTableEntry*> toDelete;

    for (MemVarTableEntry* memVarEntry : memVarEntries)
    {
        SymbolTableEntry* originalEntry = classTable->AddEntry(memVarEntry);
        if (originalEntry != nullptr)
        {
            VarDeclNode* originalNode = (VarDeclNode*)originalEntry->GetNode();
            VarDeclNode* errorNode = (VarDeclNode*)memVarEntry->GetNode();
            SemanticErrorManager::AddError(
                new DuplicateSymbolError(originalNode->GetID()->GetID(), 
                errorNode->GetID()->GetID()));
            toDelete.push_front(memVarEntry);
        }
    }

    for (ConstructorTableEntry* constructor : constructorEntries)
    {        
        SymbolTableEntry* originalEntry = classTable->AddEntry(constructor);
        if (originalEntry != nullptr)
        {
            IDNode* originalNode = GetIDFromEntry(originalEntry);
            IDNode* errorNode = GetIDFromEntry(constructor);
            SemanticErrorManager::AddError(
                new DuplicateSymbolError(originalNode->GetID(), 
                errorNode->GetID()));
            toDelete.push_front(constructor);
        }
    }

    for (MemFuncTableEntry* memFuncEntry : memFuncEntries)
    {
        SymbolTableEntry* originalEntry = classTable->AddEntry(memFuncEntry);
        if (originalEntry != nullptr)
        {
            IDNode* originalNode = GetIDFromEntry(originalEntry);
            IDNode* errorNode = GetIDFromEntry(memFuncEntry);
            SemanticErrorManager::AddError(
                new DuplicateSymbolError(originalNode->GetID(), 
                errorNode->GetID()));
            toDelete.push_front(memFuncEntry);
        }
    }

    for (SymbolTableEntry* entry : toDelete)
    {
        delete entry;
    }

    SymbolTableEntry* classEntry = new ClassTableEntry(element, classTable);

    element->SetSymbolTable(classTable);

    m_workingList.push_front(classEntry);
}

void SymbolTableAssembler::Visit(ProgramNode* element)
{
    m_globalScopeTable = new SymbolTable("Global");

    std::list<SymbolTableEntry*> toDelete;
    for (SymbolTableEntry* entry : m_workingList)
    {
        SymbolTableEntry* originalEntry = m_globalScopeTable->AddEntry(entry);
        if (originalEntry != nullptr)
        {
            IDNode* originalNode = GetIDFromEntry(originalEntry);
            IDNode* errorNode = GetIDFromEntry(entry);
            SemanticErrorManager::AddError(
                new DuplicateSymbolError(originalNode->GetID(), 
                errorNode->GetID()));
            toDelete.push_front(entry);
        }
    }
    
    for (SymbolTableEntry* entry : toDelete)
    {
        delete entry;
    }

    element->SetSymbolTable(m_globalScopeTable);
    m_workingList.clear();
}

SymbolTable* SymbolTableAssembler::GetGlobalSymbolTable() { return m_globalScopeTable; }

bool SymbolTableAssembler::TryMatchMemFuncDeclAndDef(MemFuncDefEntry* def)
{
    for (SymbolTableEntry* entry : m_workingList)
    {
        ClassTableEntry* classDefEntry = dynamic_cast<ClassTableEntry*>(entry);
        if (classDefEntry != nullptr && classDefEntry->GetName() == def->GetClassID())
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

bool SymbolTableAssembler::TryMatchMemFuncDeclAndDef(ConstructorDefEntry* def)
{
    for (SymbolTableEntry* entry : m_workingList)
    {
        ClassTableEntry* classDefEntry = dynamic_cast<ClassTableEntry*>(entry);
        if (classDefEntry != nullptr && classDefEntry->GetName() == def->GetClassID())
        {
            for (SymbolTableEntry* classEntry : *classDefEntry->GetSubTable())
            {
                ConstructorTableEntry* decl = dynamic_cast<ConstructorTableEntry*>(classEntry);
                if (decl != nullptr && decl->GetParamTypes() == def->GetParamTypes())
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

IDNode* SymbolTableAssembler::GetIDFromEntry(SymbolTableEntry* entry)
{
    if (entry->GetKind() == SymbolTableEntryKind::LocalVariable 
        || entry->GetKind() == SymbolTableEntryKind::MemVar)
    {
        return ((VarDeclNode*)entry->GetNode())->GetID();
    }
    else if (entry->GetKind() == SymbolTableEntryKind::FreeFunction 
        || entry->GetKind() == SymbolTableEntryKind::MemFuncDecl
        || entry->GetKind() == SymbolTableEntryKind::ConstructorDecl)
    {
        return ((FunctionDefNode*)entry->GetNode())->GetID();
    }
    return nullptr;
}

// SemanticChecker //////////////////////////////////////////
SemanticChecker::SemanticChecker(SymbolTable* globalTable) 
    : m_globalTable(globalTable), m_currTable(globalTable) { }
    
void SemanticChecker::Visit(IDNode* element)
{
    SymbolTable* currTable = element->GetSymbolTable();
    ASSERT(currTable != nullptr);
    if (!currTable->ScopeContainsName(element->GetID().GetLexeme()))
    {
        SemanticErrorManager::AddError(new UnknownSymbolError(element->GetID()));
    }
}