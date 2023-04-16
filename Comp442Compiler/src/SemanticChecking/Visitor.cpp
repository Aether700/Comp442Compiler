#include "Visitor.h"
#include "SemanticErrors.h"
#include "../Parser/AST.h"
#include "../Parser/ASTUtil.h"
#include "../Core/Core.h"

#include <sstream>

// Helpers /////////////////////////////////////////////////////

template<typename NodeType>
void CheckForLocalVarOverShadowing(NodeType* element)
{
    const std::string& varName = element->GetID()->GetID().GetLexeme();
    SymbolTableEntry* elementEntry = element->GetSymbolTable()->FindEntryInTable(varName);
    ASSERT(elementEntry != nullptr);
    if (elementEntry->GetKind() == SymbolTableEntryKind::LocalVariable 
        || elementEntry->GetKind() == SymbolTableEntryKind::Parameter)
    {
        SymbolTableEntry* parentEntry = elementEntry->GetParentTable()->GetParentEntry();
        if (parentEntry->GetKind() == SymbolTableEntryKind::MemFuncDecl
            || parentEntry->GetKind() == SymbolTableEntryKind::ConstructorDecl)
        {
            SymbolTableEntry* overshadowedMem = parentEntry->GetParentTable()
                ->FindEntryInScope(varName);
            if (overshadowedMem != nullptr && overshadowedMem->GetKind() 
                == SymbolTableEntryKind::MemVar)
            {
                SemanticErrorManager::AddWarning(new LocalVarOverShadowingMem(
                    ((ClassDefNode*)overshadowedMem->GetNode()->GetParent())->GetID()->GetID(), 
                    element->GetID()->GetID()));
            }
        }
    }
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

// tries to find a class or memvar entry of the provided name given the provided 
// name and parameters. returns nullptr if there is no such inheritance in the 
// inheritance tree
SymbolTableEntry* FindEntryInInheritanceTree(SymbolTable* globalTable, 
    SymbolTable* firstClassTable, const std::string& name, 
    SymbolTableEntryKind kind, const std::string& params = "")
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
            if (entry->GetKind() == SymbolTableEntryKind::FreeFunction
                && originalEntry->GetKind() == SymbolTableEntryKind::FreeFunction)
            {
                FreeFuncTableEntry* funcEntry = (FreeFuncTableEntry*)entry;
                FreeFuncTableEntry* originalFuncEntry = (FreeFuncTableEntry*)originalEntry;
                if (funcEntry->GetParamTypes() == originalFuncEntry->GetParamTypes())
                {
                    IDNode* originalNode = GetIDFromEntry(originalEntry);
                    IDNode* errorNode = GetIDFromEntry(entry);
                    SemanticErrorManager::AddError(
                        new DuplicateSymbolError(originalNode->GetID(),
                            errorNode->GetID()));
                    toDelete.push_front(entry);
                }
                else
                {
                    IDNode* funcID = GetIDFromEntry(entry);
                    SemanticErrorManager::AddWarning(new OverloadedFreeFuncWarn(funcID->GetID()));
                }
            }
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
    if (HasDotForParent(element))
    {
        return;
    }

    SymbolTable* currTable = element->GetSymbolTable();
    ASSERT(currTable != nullptr);
    if (element->GetID().GetLexeme() == "self")
    {
        SymbolTableEntry* parentEntry = currTable->GetParentEntry();
        if (parentEntry == nullptr 
            || (parentEntry->GetKind() != SymbolTableEntryKind::MemFuncDecl 
            && parentEntry->GetKind() != SymbolTableEntryKind::ConstructorDecl))
        {
            SemanticErrorManager::AddError(new UndeclaredSymbolError(element->GetID()));    
        }
    }
    else if (!currTable->ScopeContainsName(element->GetID().GetLexeme()))
    {
        SemanticErrorManager::AddError(new UndeclaredSymbolError(element->GetID()));
    }
}

void SemanticChecker::Visit(VarDeclNode* element)
{
    // check for localvar overshadowing member
    CheckForLocalVarOverShadowing(element);

    // check for constructor parameters
    AParamListNode* params = element->GetParamList();
    if (params != nullptr)
    {
        const std::string& classID = element->GetType()->GetType().GetLexeme();
        SymbolTableEntry* classEntry = m_globalTable->FindEntryInTable(classID);
        if (classEntry != nullptr && classEntry->GetKind() == SymbolTableEntryKind::Class)
        {
            SymbolTable* classTable = classEntry->GetSubTable();
            bool found = false;
            for (SymbolTableEntry* entry : *classTable)
            {
                if (entry->GetKind() == SymbolTableEntryKind::ConstructorDecl)
                {
                    ConstructorTableEntry* constructor = (ConstructorTableEntry*)entry;
                    FParamListNode* fparam = constructor->GetDeclNode()->GetParameters();
                    if (HasMatchingParameters(fparam, params))
                    {
                        found = true;
                        break;
                    }
                }
            }

            if (!found)
            {
                SemanticErrorManager::AddError(new 
                    IncorrectParametersProvidedToFuncCallError(element->GetType(), params));
            }
        }
    }
}

void SemanticChecker::Visit(DimensionNode* element)
{
    for (ASTNode* node : element->GetChildren())
    {
        if (dynamic_cast<UnspecificedDimensionNode*>(node) == nullptr 
            && node->GetEvaluatedType() != "integer")
        {
            SemanticErrorManager::AddError(
                new ArrayIndexingTypeError(element->GetFirstToken()));
            break;
        }
    }
}

void SemanticChecker::Visit(DotNode* element)
{
    ASTNode* parent = element->GetParent();

    if (dynamic_cast<DotNode*>(parent) == nullptr)
    {
        TestDotRemainder(element->GetSymbolTable(), element);
    }
}

void SemanticChecker::Visit(BaseBinaryOperator* element)
{
    // check for array type
    if (IsArrayType(element->GetLeft()->GetEvaluatedType()) 
        || IsArrayType(element->GetRight()->GetEvaluatedType()))
    {
        SemanticErrorManager::AddError(new OperationOnArrayError(element->GetFirstToken()));
    }
    else if (element->GetEvaluatedType() == ASTNode::InvalidType) 
    {
        // check for invalid operands
        SemanticErrorManager::AddError(new InvalidOperandForOperatorError(element));
    }

}

void SemanticChecker::Visit(ReturnStatNode* element)
{
    SymbolTableEntry* entry = element->GetSymbolTable()->GetParentEntry();
    if (entry->GetEvaluatedType() 
        != element->GetExpr()->GetEvaluatedType())
    {
        SemanticErrorManager::AddError(
            new IncorrectReturnTypeError(element->GetSymbolTable()->GetName(), 
            entry->GetEvaluatedType(), element->GetExpr()->GetEvaluatedType(), 
            element->GetFirstToken()));
    }
    else
    {
        for (SymbolTableEntry* currEntry : m_functionWithCorrectReturnStat)
        {
            if (currEntry == entry)
            {
                return;
            }
        }
        m_functionWithCorrectReturnStat.push_front(entry);
    }
}

void SemanticChecker::Visit(AssignStatNode* element)
{
    if (IsArrayType(element->GetLeft()->GetEvaluatedType()) 
        || IsArrayType(element->GetRight()->GetEvaluatedType()))
    {
        SemanticErrorManager::AddError(new OperationOnArrayError(element->GetFirstToken()));
    }
    else if (element->GetLeft()->GetEvaluatedType() != element->GetRight()->GetEvaluatedType())
    {
        SemanticErrorManager::AddError(new InvalidTypeMatchupForAssignError(element));
    }
}

void SemanticChecker::Visit(FParamNode* element)
{
    CheckForLocalVarOverShadowing(element);
}

void SemanticChecker::Visit(FuncCallNode* element)
{
    if (HasDotForParent(element))
    {
        return;
    }

    const std::string& funcName = element->GetID()->GetID().GetLexeme();

    // for free functions (memfunc and constructors are handled by dot node)
    bool found = false;
    for (SymbolTableEntry* entry : *m_globalTable)
    {
        if (entry->GetKind() == SymbolTableEntryKind::FreeFunction 
            && entry->GetName() == funcName)
        {
            FParamListNode* fparams = ((FunctionDefNode*)((FreeFuncTableEntry*)entry)->
                GetNode())->GetParameters();
            if (HasMatchingParameters(fparams, element->GetParameters()))
            {
                found = true;
                break;
            }
        }
    }

    if (!found)
    {
        SemanticErrorManager::AddError(
            new IncorrectParametersProvidedToFuncCallError(element));
    }
}

void SemanticChecker::Visit(FunctionDefNode* element)
{
    // check if a return statement is provided
    CheckReturnStatement(element);

    // check if function is overloaded
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

void SemanticChecker::Visit(ProgramNode* element)
{
    // check number of main functions
    size_t numMainEncountered = 0;
    SymbolTableEntry* mainEntry = nullptr;
    for (SymbolTableEntry* entry : *m_globalTable)
    {
        if (entry->GetKind() == SymbolTableEntryKind::FreeFunction 
            && entry->GetNode()->GetSymbolTable()->GetName() == "main")
        {
            mainEntry = entry;
            numMainEncountered++;
            if (numMainEncountered > 1)
            {
                break;
            }
        }
    }

    if (numMainEncountered != 1)
    {
        SemanticErrorManager::AddError(new IncorrectNumberOfMainFuncError());
    }
    else
    {
        // check if main has parameters
        if (((FunctionDefNode*)mainEntry->GetNode())->GetParameters()->GetNumChild() > 0)
        {
            SemanticErrorManager::AddWarning(new MainHasParametersWarn());
        }
    }
}

void SemanticChecker::Visit(MemFuncDeclNode* element)
{
    // check for overriden functions
    ClassDefNode* classNode = (ClassDefNode*)element->GetParent();
    if (classNode->GetInheritanceList()->GetNumChild() > 0)
    {
        const std::string& funcName = element->GetID()->GetID().GetLexeme();
        MemFuncTableEntry* currElementEntry = (MemFuncTableEntry*)element->GetParent()
            ->GetSymbolTable()->FindEntryInTable(funcName);

        bool found = false;
        for (ASTNode* baseNode : classNode->GetInheritanceList()->GetChildren())
        {
            IDNode* id = (IDNode*)baseNode;
            SymbolTableEntry* parentClassEntry = m_globalTable->FindEntryInTable(
                id->GetID().GetLexeme());

            if (parentClassEntry != nullptr)
            {
                ASSERT(parentClassEntry->GetKind() == SymbolTableEntryKind::Class);
                for (SymbolTableEntry* parentEntry : *parentClassEntry->GetSubTable())
                {
                    if (parentEntry->GetKind() == SymbolTableEntryKind::MemFuncDecl)
                    {
                        MemFuncTableEntry* memFuncEntry = (MemFuncTableEntry*)parentEntry;
                        MemFuncDeclNode* memFuncNode = memFuncEntry->GetDeclNode();
                        if (memFuncNode->GetID()->GetID().GetLexeme() 
                            == funcName && memFuncEntry->GetParamTypes() 
                            == currElementEntry->GetParamTypes())
                        {
                            SemanticErrorManager::AddWarning(
                                new OverridenFuncWarn(currElementEntry->GetClassID(), 
                                memFuncEntry->GetClassID(), element->GetID()->GetID(), 
                                currElementEntry->GetParamTypes()));
                            found = true;
                            break;
                        }
                    }
                }

                if (found)
                {
                    break;
                }
            }
        }
    }
}

void SemanticChecker::Visit(MemFuncDefNode* element)
{
    // check if a return statement is provided
    CheckReturnStatement(element);

    //check for overloaded function
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
            SemanticErrorManager::AddWarning(new MemOverShadowingMemWarn(
                ((ClassDefNode*)element->GetParent())->GetID()->GetID(), 
                GetIDFromEntry(entry)->GetID()));
        }
    }
}

void SemanticChecker::Visit(ClassDefNode* element)
{
    std::list<ClassDefNode*> visited;
    std::list<ClassDefNode*> stack;

    const Token& className = element->GetID()->GetID();
    
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
            SemanticErrorManager::AddError(new CircularInheritanceDependencyError(className));
            break;
        }

        {
            SymbolTable* currClassTable = currClass->GetSymbolTable();
            bool found = false;
            Token t;
            for (SymbolTableEntry* entry : *currClassTable)
            {
                if (entry->GetKind() == SymbolTableEntryKind::MemVar 
                    && entry->GetEvaluatedType() == className.GetLexeme())
                {
                    found = true;
                    t = ((VarDeclNode*)entry->GetNode())->GetID()->GetID();
                    break;
                }
            }

            if (found)
            {
                SemanticErrorManager::AddError(
                    new CircularClassMemberDependencyError(className.GetLexeme(), 
                    currClass->GetID()->GetID().GetLexeme(), t));
            break;
            }
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

void SemanticChecker::TestDotRemainder(SymbolTable* contextTable, 
    ASTNode* dotRemainder)
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
        SymbolTableEntry* varEntry = contextTable->FindEntryInScope(var->
            GetVariable()->GetID().GetLexeme());

        if (varEntry == nullptr || (varEntry->GetKind() != SymbolTableEntryKind::MemVar 
            && varEntry->GetKind() != SymbolTableEntryKind::LocalVariable
            && varEntry->GetKind() != SymbolTableEntryKind::Parameter))
        {
            bool hasError = true;
            if (contextTable == m_globalTable 
                || contextTable->GetParentEntry()->GetKind() != SymbolTableEntryKind::Class)
            {
                if (var->GetVariable()->GetID().GetLexeme() == "self")
                {
                    if (IsValidSelf(contextTable, var))
                    {
                        hasError = false;
                    }
                    else
                    {
                        SemanticErrorManager::AddError(new UndeclaredSymbolError
                            (var->GetVariable()->GetID()));
                    }
                }
                else
                {
                    SemanticErrorManager::AddError(new UndeclaredSymbolError
                        (var->GetVariable()->GetID()));
                }

            }
            else
            {
                SemanticErrorManager::AddError(new UnknownMemberError
                    (contextTable->GetName(), var->GetVariable()->GetID()));
            }

            if (hasError)
            {
                return;
            }
        }
        else if (varEntry->GetKind() == SymbolTableEntryKind::MemVar)
        {
            MemVarTableEntry* memVarEntry = (MemVarTableEntry*)varEntry;
            SymbolTable* callingContext = dotRemainder->GetSymbolTable();
            if (!ClassMemberIsAccessible(memVarEntry, callingContext))
            {
                SemanticErrorManager::AddError(new ProhibitedAccessToPrivateMemberError(
                    var->GetVariable()->GetID()));
            }
        }


        if (right != nullptr)
        {
            SymbolTable* nextContext = GetContextTable(m_globalTable, contextTable, left);
            if (nextContext != nullptr)
            {
                TestDotRemainder(nextContext, right);
            }
        }
    }
    else if (dynamic_cast<FuncCallNode*>(left) != nullptr)
    {
        FuncCallNode* funcCall = (FuncCallNode*)left;
        
        if (contextTable->GetParentEntry()->GetKind() != SymbolTableEntryKind::Class)
        {
            SymbolTableEntry* funcEntry = contextTable->FindEntryInScope(funcCall->
                GetID()->GetID().GetLexeme());

            if (funcEntry == nullptr 
                || funcEntry->GetKind() != SymbolTableEntryKind::FreeFunction)
            {
                SemanticErrorManager::AddError(new UndeclaredSymbolError
                    (funcCall->GetID()->GetID()));
                return;
            }
        }
        else // mem func call or constructor
        {
            const std::string& funcName = funcCall->GetID()->GetID().GetLexeme();
            bool foundSameName = false;
            bool foundSameArgs = false;
            for (SymbolTableEntry* entry : *contextTable)
            {
                if ((entry->GetKind() == SymbolTableEntryKind::MemFuncDecl 
                    && entry->GetName() == funcName) 
                    || (entry->GetKind() == SymbolTableEntryKind::ConstructorDecl 
                    && contextTable->GetName() == funcName))
                {

                    if (entry->GetKind() == SymbolTableEntryKind::MemFuncDecl)
                    {
                        MemFuncTableEntry* memFuncEntry = (MemFuncTableEntry*)entry;

                        if (!ClassMemberIsAccessible(memFuncEntry, dotRemainder->GetSymbolTable()))
                        {
                            SemanticErrorManager::AddError(new ProhibitedAccessToPrivateMemberError(((MemFuncDefNode*)
                                ((MemFuncTableEntry*)entry)->GetNode())->GetID()->GetID()));
                        }
                    }

                    foundSameName = true;
                    FParamListNode* fparams;
                    if (entry->GetKind() == SymbolTableEntryKind::MemFuncDecl)
                    {
                        fparams = ((MemFuncDefNode*)
                            ((MemFuncTableEntry*)entry)->GetNode())->GetParameters();
                    }
                    else
                    {
                        fparams = ((ConstructorDefNode*)
                            ((ConstructorTableEntry*)entry)->GetNode())->GetParameters();
                    }

                    if (HasMatchingParameters(fparams, funcCall->GetParameters()))
                    {
                        foundSameArgs = true;
                        break;
                    }
                }
            }

            if (!foundSameName)
            {
                SemanticErrorManager::AddError(
                    new UnknownMemberError(contextTable->GetName(), 
                    funcCall->GetID()->GetID()));
            }
            else if (!foundSameArgs)
            {
                SemanticErrorManager::AddError(
                    new IncorrectParametersProvidedToFuncCallError(funcCall));
            }
        }

        if (right != nullptr)
        {
            SymbolTable* nextContext = GetContextTable(m_globalTable, contextTable, left);
            if (nextContext != nullptr)
            {
                TestDotRemainder(nextContext, right);
            }
        }
    }
    else
    {
        DEBUG_BREAK(); // error here
    }
}