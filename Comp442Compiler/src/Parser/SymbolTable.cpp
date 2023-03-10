#include "SymbolTable.h"
#include "AST.h"
#include "../Core/Core.h"
#include <sstream>

// SymbolTableEntryKind ////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& stream, SymbolTableEntryKind kind)
{
    switch(kind)
    {
    case SymbolTableEntryKind::Class:
        stream << "class";
        break;

    case SymbolTableEntryKind::FreeFunction:
        stream << "free function";
        break;

    case SymbolTableEntryKind::Parameter:
        stream << "parameter";
        break;

    case SymbolTableEntryKind::LocalVariable:
        stream << "local variable";
        break;

    case SymbolTableEntryKind::MemVar:
        stream << "member variable";
        break;

    case SymbolTableEntryKind::MemFuncDecl:
        stream << "member function";
        break;

    case SymbolTableEntryKind::ConstructorDecl:
        stream << "constructor";
        break;

    case SymbolTableEntryKind::InheritanceList:
        stream << "inherites";
        break;

    default:
        DEBUG_BREAK();
        break;
    }
    return stream;
}

// SymbolTableEntry //////////////////////////////////////////

SymbolTableEntry::SymbolTableEntry(SymbolTableEntryKind kind) 
    : m_kind(kind) { }

SymbolTableEntry::~SymbolTableEntry() { }

const std::string& SymbolTableEntry::GetName() const { return m_name; }
SymbolTableEntryKind SymbolTableEntry::GetKind() const { return m_kind; }
void SymbolTableEntry::SetName(const std::string& name) { m_name = name; }

// VarSymbolTableEntry /////////////////////////////////////////////////////////////////
VarSymbolTableEntry::VarSymbolTableEntry(VarDeclNode* node, const std::string& typeStr,
    SymbolTableEntryKind kind) : SymbolTableEntry(kind), m_node(node), m_typeStr(typeStr)
{
    SetName(m_node->GetID()->GetID().GetLexeme());
}


const std::string& VarSymbolTableEntry::GetType() const 
{ 
    return m_typeStr; 
}

SymbolTable* VarSymbolTableEntry::GetSubTable() { return nullptr; }

ASTNode* VarSymbolTableEntry::GetNode() { return m_node; }

std::string VarSymbolTableEntry::ToString()
{
    std::stringstream ss;
    ss << GetKind() << s_seperator << GetName() << s_seperator << GetType();
    return ss.str();
}

// FreeFuncTableEntry /////////////////////////////////////////////////////////////
FreeFuncTableEntry::FreeFuncTableEntry(FunctionDefNode* node, 
    const std::string& parametersType, SymbolTable* subTable) 
    : SymbolTableEntry(SymbolTableEntryKind::FreeFunction), m_subTable(subTable), 
    m_paramTypes(parametersType), m_funcNode(node)
{
    SetName(node->GetID()->GetID().GetLexeme());
}

FreeFuncTableEntry::~FreeFuncTableEntry() { delete m_subTable; }

const std::string& FreeFuncTableEntry::GetReturnType() const 
{ 
    return m_funcNode->GetReturnType()->GetType().GetLexeme(); 
}

const std::string& FreeFuncTableEntry::GetParamTypes() const { return m_paramTypes; }

SymbolTable* FreeFuncTableEntry::GetSubTable() { return m_subTable; }

ASTNode* FreeFuncTableEntry::GetNode() { return m_funcNode; }

std::string FreeFuncTableEntry::ToString() 
{
    std::stringstream ss;
    ss << GetKind() << s_seperator << GetName() << s_seperator << 
        "(" << GetParamTypes() << "):" << GetReturnType();
    return ss.str();
}

// ClassTableEntry ////////////////////////////////////////////////////////////////
ClassTableEntry::ClassTableEntry(ClassDefNode* node, SymbolTable* subTable) 
    : SymbolTableEntry(SymbolTableEntryKind::Class), m_classNode(node), 
    m_subTable(subTable) 
{
    SetName(m_classNode->GetID()->GetID().GetLexeme());
}

ClassTableEntry::~ClassTableEntry()
{
    delete m_subTable;
}

SymbolTable* ClassTableEntry::GetSubTable() { return m_subTable; }
ASTNode* ClassTableEntry::GetNode() { return m_classNode; }

std::string ClassTableEntry::ToString()
{
    std::stringstream ss;
    ss << GetKind() << s_seperator << GetName();
    return ss.str();
}

// InheritanceListEntry /////////////////////////////////////////////////

InheritanceListEntry::InheritanceListEntry(InheritanceListNode* node) 
    : SymbolTableEntry(SymbolTableEntryKind::InheritanceList), m_node(node) 
{
    SetName(GetClassID() + "::InheritanceList");
}
    
const std::string& InheritanceListEntry::GetClassID() const
{
    return ((ClassDefNode*)GetNonConstNode()->GetParent())->GetID()->GetID().GetLexeme();
}

bool InheritanceListEntry::IsChildOf(const std::string& parentClassName) const
{
    return m_node->ContainsClassName(parentClassName);
}

SymbolTable* InheritanceListEntry::GetSubTable() { return nullptr; }
ASTNode* InheritanceListEntry::GetNode() { return m_node; }
    
std::string InheritanceListEntry::ToString()
{
    std::stringstream ss;
    ss << GetKind() << s_seperator;

    bool isEmpty = true;
    for (ASTNode* node : m_node->GetChildren())
    {
        IDNode* id = dynamic_cast<IDNode*>(node);
        ASSERT(id != nullptr);
        ss << id->GetID().GetLexeme() << ", ";
        isEmpty = false;
    }

    if (isEmpty)
    {
        ss << "None";
    }
    else
    {
        // remove last ", "
        std::string tempCopy = ss.str();
        ss.str(tempCopy.substr(0, tempCopy.length() - 2));
    }

    return ss.str();
}

InheritanceListNode* InheritanceListEntry::GetNonConstNode() const
{
    return const_cast<InheritanceListNode*>(m_node);
}

// MemVarTableEntry /////////////////////////////////////////////////////////////////

MemVarTableEntry::MemVarTableEntry(MemVarNode* node, const std::string& typeStr) 
    : VarSymbolTableEntry(node, typeStr, SymbolTableEntryKind::MemVar) { }

const std::string& MemVarTableEntry::GetClassID() const
{
    return ((ClassDefNode*)GetMemVarNode()->GetParent())->GetID()->GetID().GetLexeme();
}

const std::string& MemVarTableEntry::GetVisibility() const
{
    return GetMemVarNode()->GetVisibility()->GetVisibility();
}

std::string MemVarTableEntry::ToString()
{
    std::stringstream ss;
    ss << GetKind() << s_seperator << GetName() << s_seperator 
        << GetType() << s_seperator << GetVisibility();
    return ss.str();
}

MemVarNode* MemVarTableEntry::GetMemVarNode() const
{
    return (MemVarNode*)const_cast<MemVarTableEntry*>(this)->GetNode();
}

// MemFuncTableEntry /////////////////////////////////////////////////////////

MemFuncTableEntry::MemFuncTableEntry(MemFuncDeclNode* node, 
    const std::string& parameterTypes) : SymbolTableEntry(SymbolTableEntryKind::MemFuncDecl), 
    m_declaration(node), m_definition(nullptr), 
    m_definitionSubTable(nullptr), m_parameterTypes(parameterTypes)
{
    SetName(node->GetID()->GetID().GetLexeme());
}

MemFuncTableEntry::~MemFuncTableEntry()
{
    delete m_definitionSubTable;
}

const std::string& MemFuncTableEntry::GetClassID() const
{
    return ((ClassDefNode*)m_declaration->GetParent())->GetID()->GetID().GetLexeme();
}

const std::string& MemFuncTableEntry::GetVisibility() const
{
    return m_declaration->GetVisibility()->GetVisibility();
}

const std::string& MemFuncTableEntry::GetReturnType() const
{
    return m_declaration->GetReturnType()->GetType().GetLexeme();
}

const std::string& MemFuncTableEntry::GetParamTypes() const
{
    return m_parameterTypes;
}

void MemFuncTableEntry::SetDefinition(MemFuncDefEntry* defEntry)
{
    m_definition = defEntry->m_defNode;
    m_definitionSubTable = defEntry->m_subTable;
    defEntry->m_defNode = nullptr;
    defEntry->m_subTable = nullptr;
}

bool MemFuncTableEntry::HasDefinition() const { return m_definition != nullptr; }
ASTNode* MemFuncTableEntry::GetNode() { return m_definition; }
SymbolTable* MemFuncTableEntry::GetSubTable() { return m_definitionSubTable; }

std::string MemFuncTableEntry::ToString()
{
    ASSERT(HasDefinition());
    std::stringstream ss;
    ss << GetKind() << s_seperator << GetName() << s_seperator 
        << "(" << GetParamTypes() << "):" << GetReturnType() 
        << s_seperator << GetVisibility();

    return ss.str();
}

// MemFuncDefEntry ///////////////////////////////////////////////////////

MemFuncDefEntry::MemFuncDefEntry(MemFuncDefNode* def, const std::string& parameterTypes, 
    SymbolTable* subTable) : SymbolTableEntry(SymbolTableEntryKind::MemFuncDef), 
    m_defNode(def), m_subTable(subTable), m_parameterTypes(parameterTypes)
{
    SetName(def->GetID()->GetID().GetLexeme());
}

MemFuncDefEntry::~MemFuncDefEntry()
{
    delete m_subTable;
}

const std::string& MemFuncDefEntry::GetClassID() const
{
    return m_defNode->GetClassID()->GetID().GetLexeme();
}

const std::string& MemFuncDefEntry::GetReturnType() const
{
    return m_defNode->GetReturnType()->GetType().GetLexeme();
}

const std::string& MemFuncDefEntry::GetParamTypes() const { return m_parameterTypes; }

ASTNode* MemFuncDefEntry::GetNode() { return m_defNode; }
SymbolTable* MemFuncDefEntry::GetSubTable() { return m_subTable; }

std::string MemFuncDefEntry::ToString()
{
    DEBUG_BREAK();
    return "";
}

// ConstructorTableEntry ///////////////////////////////////////////////////////////////
ConstructorTableEntry::ConstructorTableEntry(ConstructorDeclNode* node, 
    const std::string& parameterTypes) : SymbolTableEntry(SymbolTableEntryKind::ConstructorDecl), 
    m_declaration(node), m_definition(nullptr), m_definitionSubTable(nullptr), 
    m_parameterTypes(parameterTypes) 
{
    SetName("constructor");
}

ConstructorTableEntry::~ConstructorTableEntry()
{
    delete m_definitionSubTable;
}

const std::string& ConstructorTableEntry::GetClassID() const
{
    return ((ClassDefNode*)m_declaration->GetParent())->GetID()->GetID().GetLexeme();
}

const std::string& ConstructorTableEntry::GetVisibility() const
{
    return m_declaration->GetVisibility()->GetVisibility();
}

const std::string& ConstructorTableEntry::GetReturnType() const
{
    return GetClassID();
}

const std::string& ConstructorTableEntry::GetParamTypes() const { return m_parameterTypes; }

void ConstructorTableEntry::SetDefinition(ConstructorDefEntry* defEntry)
{
    m_definition = defEntry->m_defNode;
    m_definitionSubTable = defEntry->m_subTable;
    defEntry->m_defNode = nullptr;
    defEntry->m_subTable = nullptr;
}

bool ConstructorTableEntry::HasDefinition() const 
{
    return m_definition != nullptr;
}

ASTNode* ConstructorTableEntry::GetNode() { return m_definition; }

SymbolTable* ConstructorTableEntry::GetSubTable() { return m_definitionSubTable; }
std::string ConstructorTableEntry::ToString() 
{
    ASSERT(HasDefinition());
    std::stringstream ss;
    ss << GetKind() << s_seperator << GetName() << s_seperator 
        << "(" << GetParamTypes() << "):" << GetReturnType() 
        << s_seperator << GetVisibility();

    return ss.str();
}

// ConstructorDefEntry ////////////////////////////////////////////////////////////////////
ConstructorDefEntry::ConstructorDefEntry(ConstructorDefNode* node, 
    const std::string& parameterTypes, SymbolTable* subTable) 
    : SymbolTableEntry(SymbolTableEntryKind::ConstructorDef), m_defNode(node), 
    m_parameterTypes(parameterTypes), m_subTable(subTable) 
{
    SetName("constructor");
}

ConstructorDefEntry::~ConstructorDefEntry()
{
    delete m_subTable;
}

const std::string& ConstructorDefEntry::GetClassID() const
{
    return m_defNode->GetID()->GetID().GetLexeme();
}

const std::string& ConstructorDefEntry::GetReturnType() const
{
    return GetClassID();
}

const std::string& ConstructorDefEntry::GetParamTypes() const { return m_parameterTypes; }
ASTNode* ConstructorDefEntry::GetNode() { return m_defNode; }
SymbolTable* ConstructorDefEntry::GetSubTable() { return m_subTable; }
std::string ConstructorDefEntry::ToString() 
{
    DEBUG_BREAK();
    return "";
}


// SymbolTable ////////////////////////////////////////////////////////////////////

SymbolTable::SymbolTable(const std::string& name) : m_name(name) { }

SymbolTable::~SymbolTable()
{
    for (SymbolTableEntry* entry : m_entries)
    {
        delete entry;
    }
}

const std::string& SymbolTable::GetName() const { return m_name; }

void SymbolTable::AddEntry(SymbolTableEntry* entry) { m_entries.push_back(entry); }
void SymbolTable::AddEntryFirst(SymbolTableEntry* entry) { m_entries.push_front(entry); }

SymbolTable::TableIterator SymbolTable::begin() { return m_entries.begin(); }
SymbolTable::TableIterator SymbolTable::end() { return m_entries.end(); }


// SymbolTableDisplayManager //////////////////////////////////////////////////////////////
std::string SymbolTableDisplayManager::TableToStr(SymbolTable* table, size_t depth)
{
    if (depth == 0)
    {
        return MainTableToStr(table);
    }
    else if (depth == 1)
    {
        return SubTableToStr(table);
    }
    else if (depth == 2)
    {
        return SubSubTableToStr(table);
    }
    else
    {
        DEBUG_BREAK();
        return "";
    }
}

std::string SymbolTableDisplayManager::MainTableToStr(SymbolTable* table)
{
    std::stringstream ss;
    ss << CreateHeader(s_mainTableWidth) << "\n";
    
    ss << FitInTable(std::string(s_namePrefix) + table->GetName(), 
        s_mainTableWidth, s_mainTableIndent) << "\n";

    ss << CreateHeader(s_mainTableWidth) << "\n";

    for (SymbolTableEntry* entry : *table)
    {
        ss << FitInTable(entry->ToString(), s_mainTableWidth, s_mainTableIndent) << "\n";
        
        if (entry->GetSubTable() != nullptr)
        {
            ss << TableToStr(entry->GetSubTable(), 1);
        }
    }

    ss << CreateHeader(s_mainTableWidth) << "\n";

    return ss.str();
}

std::string SymbolTableDisplayManager::SubTableToStr(SymbolTable* table)
{
    std::stringstream ss;
    ss << FitInTable(CreateHeader(s_subTableWidth), s_mainTableWidth, 
        s_subTableIndent) << "\n";
    
    ss << FitInTable(FitInTable(std::string(s_namePrefix) + table->GetName(), 
        s_subTableWidth, s_mainTableIndent), s_mainTableWidth, s_subTableIndent) << "\n";

    ss << FitInTable(CreateHeader(s_subTableWidth), s_mainTableWidth, 
        s_subTableIndent) << "\n";

    for (SymbolTableEntry* entry : *table)
    {
        ss << FitInTable(FitInTable(entry->ToString(), s_subTableWidth, 
            s_mainTableIndent), s_mainTableWidth, s_subTableIndent) << "\n";

        if (entry->GetSubTable() != nullptr)
        {
            ss << TableToStr(entry->GetSubTable(), 2);
        }
    }

    ss << FitInTable(CreateHeader(s_subTableWidth), s_mainTableWidth, 
        s_subTableIndent) << "\n";

    return ss.str();
}

std::string SymbolTableDisplayManager::SubSubTableToStr(SymbolTable* table)
{
    std::stringstream ss;
    ss << FitInTable(FitInTable(CreateHeader(s_subSubTableWidth), s_subTableWidth, 
        s_subSubTableIndent), s_mainTableWidth, s_subTableIndent) << "\n";
    
    ss << FitInTable(FitInTable(FitInTable(std::string(s_namePrefix) + table->GetName(), 
        s_subSubTableWidth, s_mainTableIndent), s_subTableWidth, s_subSubTableIndent), 
        s_mainTableWidth, s_subTableIndent) << "\n";

    ss << FitInTable(FitInTable(CreateHeader(s_subSubTableWidth), s_subTableWidth, 
        s_subSubTableIndent), s_mainTableWidth, s_subTableIndent) << "\n";

    for (SymbolTableEntry* entry : *table)
    {
        ss << FitInTable(FitInTable(FitInTable(entry->ToString(), s_subSubTableWidth, 
            s_mainTableIndent), s_subTableWidth, s_subSubTableIndent), s_mainTableWidth, 
            s_subTableIndent) << "\n";
    }

    ss << FitInTable(FitInTable(CreateHeader(s_subSubTableWidth), s_subTableWidth, 
        s_subSubTableIndent), s_mainTableWidth, s_subTableIndent) << "\n";

    return ss.str();
}

std::string SymbolTableDisplayManager::CreateHeader(size_t headerLength)
{
    std::stringstream ss;
    for (size_t i = 0; i < headerLength; i++)
    {
        ss << "=";
    }
    return ss.str();
}

std::string SymbolTableDisplayManager::FitInTable(const std::string& content, 
    size_t tableWidth, size_t indent)
{
    std::stringstream ss;
    ss << "|";

    for (size_t i = 0; i < indent; i++)
    {
        ss << " ";
    }

    ss << content;
    size_t usedSpace = 2 + content.length() + indent;
    ASSERT(usedSpace <= tableWidth);
    size_t padding = tableWidth - usedSpace;

    for (size_t i = 0; i < padding; i++)
    {
        ss << " ";
    }
    ss << "|";
    return ss.str();
}