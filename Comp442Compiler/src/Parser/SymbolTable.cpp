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
VarSymbolTableEntry::VarSymbolTableEntry(VarDeclNode* node, 
    SymbolTableEntryKind kind) : SymbolTableEntry(kind), m_node(node)
{
    SetName(m_node->GetID()->GetID().GetLexeme());
}


const std::string& VarSymbolTableEntry::GetType() const 
{ 
    return m_node->GetType()->GetType().GetLexeme(); 
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

void SymbolTable::AddEntry(SymbolTableEntry* entry) { m_entries.push_front(entry); }

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
    
    ss << FitInTable(table->GetName(), s_mainTableWidth, s_mainTableIndent) << "\n";

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
    
    ss << FitInTable(FitInTable(table->GetName(), s_subTableWidth, s_mainTableIndent), 
        s_mainTableWidth, s_subTableIndent) << "\n";

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
    
    ss << FitInTable(FitInTable(FitInTable(table->GetName(), s_subSubTableWidth, 
        s_mainTableIndent), s_subTableWidth, s_subSubTableIndent), 
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