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

// returns the idnode of the node provided or the nullptr if there is no such node
IDNode* GetIDFromEntry(SymbolTableEntry* entry)
{
    switch(entry->GetKind())
    {
    case SymbolTableEntryKind::LocalVariable:
    case SymbolTableEntryKind::MemVar:
        return ((VarDeclNode*)entry->GetNode())->GetID();
    
    case SymbolTableEntryKind::FreeFunction:
        return ((FunctionDefNode*)entry->GetNode())->GetID();

    case SymbolTableEntryKind::MemFuncDecl:
        return ((MemFuncTableEntry*)entry)->GetDeclNode()->GetID();
    
    case SymbolTableEntryKind::ConstructorDecl:
        {
            ASTNode* parent = ((ConstructorTableEntry*)entry)->GetDeclNode()->GetParent();
            return ((ClassDefNode*)parent)->GetID();
        }

    case SymbolTableEntryKind::Class:
        return ((ClassDefNode*)entry->GetNode())->GetID();
    
    default:
        return nullptr;
    }
}

// returns nullptr if there is no such inheritance in the inheritance tree
SymbolTableEntry* FindEntryInInheritanceTree(SymbolTable* globalTable, 
    SymbolTable* firstClassTable, const std::string& name, SymbolTableEntryKind kind, 
    const std::string& params = "")
{
    if (kind != SymbolTableEntryKind::MemVar 
        && kind != SymbolTableEntryKind::MemFuncDecl
        && kind != SymbolTableEntryKind::ConstructorDecl)
    {
        return nullptr;
    }

    std::list<std::string> classesVerified;
    std::list<IDNode*> queue;
    {
        InheritanceListEntry* inheritanceList = (InheritanceListEntry*)*firstClassTable->begin();
        InheritanceListNode* node = (InheritanceListNode*)inheritanceList->GetNode();
        for (ASTNode* baseNode : node->GetChildren())
        {
            IDNode* id = (IDNode*) baseNode;
            queue.push_front(id);
            classesVerified.push_back(id->GetID().GetLexeme());
        }
    }

    while (queue.size() > 0)
    {
        // pop currClassID
        IDNode* currClassID = queue.front();
        queue.pop_front();
        SymbolTable* currClassTable = globalTable->FindEntryInTable(
            currClassID->GetID().GetLexeme())->GetSubTable();
        
        // check in current class if we find a member of the given kind and name
        {
            SymbolTableEntry* entry = currClassTable->FindEntryInTable(name);
            if (entry != nullptr && entry->GetKind() == kind)
            {
                switch (kind)
                {
                case SymbolTableEntryKind::MemVar:
                    return entry;
                
                case SymbolTableEntryKind::MemFuncDecl:
                    if (params == ((MemFuncTableEntry*)entry)->GetParamTypes())
                    {
                        return entry;
                    }
                    break;

                case SymbolTableEntryKind::ConstructorDecl:
                    if (params == ((ConstructorTableEntry*)entry)->GetParamTypes())
                    {
                        return entry;
                    }
                    break;
                }
            }
        }

        // enqueue child nodes that were not visited yet
        InheritanceListNode* inheritanceList = (InheritanceListNode*)((InheritanceListEntry*)
            *currClassTable->begin())->GetNode();
        
        for (ASTNode* baseNode : inheritanceList->GetChildren())
        {
            IDNode* id = (IDNode*) baseNode;

            bool found = false;
            for (std::string& currID : classesVerified)
            {
                if (currID == id->GetID().GetLexeme())
                {
                    found  = true;
                    break;
                }
            }

            if (!found)
            {
                queue.push_front(id);
                classesVerified.push_back(id->GetID().GetLexeme());
            }
        }
    }

    return nullptr;
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
        SemanticErrorManager::AddError(new MemberFunctionDeclNotFound(
            element->GetClassID()->GetID(), 
            element->GetID()->GetID()));
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
        SemanticErrorManager::AddError(new ConstructorDeclNotFound(element->GetID()->GetID()));
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

        if (entry->GetKind() == SymbolTableEntryKind::Class)
        {
            CheckForDeclWithoutDef((ClassTableEntry*)entry);
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

void SymbolTableAssembler::CheckForDeclWithoutDef(ClassTableEntry* classEntry)
{
    for (SymbolTableEntry* entry : *classEntry->GetSubTable())
    {
        if (entry->GetKind() == SymbolTableEntryKind::ConstructorDecl)
        {
            if (!((ConstructorTableEntry*)entry)->HasDefinition())
            {
                IDNode* classID = GetIDFromEntry(entry);
                ConstructorDeclNode* constructor 
                    = ((ConstructorTableEntry*)entry)->GetDeclNode();
                SemanticErrorManager::AddError(new ConstructorDefNotFoundError(classID->GetID(), 
                    constructor->GetToken()));
            }
        }
        else if (entry->GetKind() == SymbolTableEntryKind::MemFuncDecl)
        {
            if (!((MemFuncTableEntry*)entry)->HasDefinition())
            {
                MemFuncDeclNode* memFunc = ((MemFuncTableEntry*)entry)->GetDeclNode();
                SemanticErrorManager::AddError(new MemFuncDefNotFoundError(
                    ((ClassDefNode*)memFunc->GetParent())->GetID()->GetID(), 
                    memFunc->GetID()->GetID()));
            }
        }
    }
}

// SemanticChecker //////////////////////////////////////////
SemanticChecker::SemanticChecker(SymbolTable* globalTable) 
    : m_globalTable(globalTable) { }
    
void SemanticChecker::Visit(IDNode* element)
{
    SymbolTable* currTable = element->GetSymbolTable();
    ASSERT(currTable != nullptr);
    if (!currTable->ScopeContainsName(element->GetID().GetLexeme()))
    {
        SemanticErrorManager::AddError(new UndeclaredSymbolError(element->GetID()));
    }
}

void SemanticChecker::Visit(FunctionDefNode* element)
{
    const std::string& funcName = element->GetID()->GetID().GetLexeme();
    if (HasFoundOverLoadedFunc(m_overloadedFreeFuncFound, funcName))
    {
        return;
    }


    bool foundEntry = false;
    for (SymbolTableEntry* entry : *m_globalTable)
    {
        if (entry->GetKind() == SymbolTableEntryKind::FreeFunction)
        {
            if (entry->GetName() == funcName)
            {
                if (foundEntry)
                {
                    m_overloadedFreeFuncFound.push_front(funcName);
                    SemanticErrorManager::AddWarning(
                        new OverloadedFreeFuncWarn(GetIDFromEntry(entry)->GetID()));
                    break;
                }
                foundEntry = true;
            }
        }
    }
}

void SemanticChecker::Visit(MemFuncDefNode* element)
{
    const std::string& funcName = element->GetID()->GetID().GetLexeme();
    const std::string& classID = element->GetClassID()->GetID().GetLexeme();
    std::string idStr = classID + "::" + funcName;
    if (HasFoundOverLoadedFunc(m_overloadedMemFuncFound, idStr))
    {
        return;
    }


    SymbolTable* table = m_globalTable->FindEntryInTable(classID)->GetSubTable();

    bool foundEntry = false;
    for (SymbolTableEntry* entry : *table)
    {
        if (entry->GetKind() == SymbolTableEntryKind::MemFuncDecl)
        {
            MemFuncTableEntry* memFuncEntry = (MemFuncTableEntry*)entry;
            std::string entryID = memFuncEntry->GetClassID() + "::" + memFuncEntry->GetName();
            if (entryID == idStr)
            {
                if (foundEntry)
                {
                    m_overloadedMemFuncFound.push_front(idStr);
                    SemanticErrorManager::AddWarning(
                        new OverloadedMemFuncWarn(
                            ((ClassDefNode*)memFuncEntry->GetDeclNode()->
                            GetParent())->GetID()->GetID(), 
                            GetIDFromEntry(entry)->GetID()));
                    break;
                }
                foundEntry = true;
            }
        }
    }
}

void SemanticChecker::Visit(ConstructorDefNode* element)
{
    const std::string& classID = element->GetID()->GetID().GetLexeme();
    std::string idStr = classID + "::" + "constructor";
    if (HasFoundOverLoadedFunc(m_overloadedConstructorFound, idStr))
    {
        return;
    }


    SymbolTable* table = m_globalTable->FindEntryInTable(classID)->GetSubTable();

    bool foundEntry = false;
    for (SymbolTableEntry* entry : *table)
    {
        if (entry->GetKind() == SymbolTableEntryKind::ConstructorDecl)
        {
            ConstructorTableEntry* constructor = (ConstructorTableEntry*)entry;
            if (constructor->GetName() == idStr)
            {
                if (foundEntry)
                {
                    m_overloadedConstructorFound.push_front(idStr);
                    SemanticErrorManager::AddWarning(
                        new OverloadedConstructorWarn(
                            ((ClassDefNode*)constructor->GetDeclNode()->
                            GetParent())->GetID()->GetID()));
                    break;
                }
                foundEntry = true;
            }
        }
    }
}

void SemanticChecker::Visit(InheritanceListNode* element)
{
    SymbolTable* table = element->GetSymbolTable();
    for (SymbolTableEntry* entry : *table)
    {
        if (entry->GetKind() == SymbolTableEntryKind::ConstructorDecl)
        {
            continue;
        }

        std::string param = "";
        if (entry->GetKind() == SymbolTableEntryKind::MemFuncDecl)
        {
            param = ((MemFuncTableEntry*)entry)->GetParamTypes();
        }

        SymbolTableEntry* overshadowedEntry 
            = FindEntryInInheritanceTree(m_globalTable, table, entry->GetName(), 
                entry->GetKind(), param);
        
        if (overshadowedEntry != nullptr)
        {
            SemanticErrorManager::AddWarning(new OverShadowedMemWarn(
                ((ClassDefNode*)element->GetParent())->GetID()->GetID(), 
                GetIDFromEntry(entry)->GetID()));
        }
    }
}

void SemanticChecker::Visit(ClassDefNode* element)
{
    std::list<ClassDefNode*> visited;
    std::list<ClassDefNode*> stack;
    
    {
        InheritanceListNode* inheritanceList = element->GetInheritanceList();
        for (ASTNode* baseNode : inheritanceList->GetChildren())
        {
            IDNode* id = (IDNode*) baseNode;
            SymbolTableEntry* entry = m_globalTable->FindEntryInTable(id->GetID().GetLexeme());
            ASSERT(entry != nullptr);
            
            visited.push_front((ClassDefNode*)entry->GetNode());
            stack.push_front((ClassDefNode*)entry->GetNode());
        }
    }

    while (stack.size() > 0)
    {
        ClassDefNode* currClass = stack.front();
        stack.pop_front();

        if (currClass == element)
        {
            SemanticErrorManager::AddError(new CircularClassDependencyError(element->
                GetID()->GetID()));
            break;
        }

        // push on stack inherited classes
        InheritanceListNode* inheritanceList = currClass->GetInheritanceList();
        for (ASTNode* baseNode : inheritanceList->GetChildren())
        {
            IDNode* id = (IDNode*) baseNode;
            SymbolTableEntry* entry = m_globalTable->FindEntryInTable(id->GetID().GetLexeme());
            ASSERT(entry != nullptr);
            
            bool found = false;

            ClassDefNode* classDef = (ClassDefNode*)entry->GetNode();
            for (ClassDefNode* def : visited)
            {
                if (def == classDef)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                visited.push_front(classDef);
                stack.push_front(classDef);
            }
        }
    }
}

bool SemanticChecker::HasFoundOverLoadedFunc(const std::list<std::string>& funcList, 
    const std::string& name)
{
    for (const std::string& funcName : funcList)
    {
        if (funcName == name)
        {
            return true;
        }
    }
    return false;
}
