#include "SymbolTable.h"
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

SymbolTableEntry::SymbolTableEntry(const std::string& name, SymbolTableEntryKind kind, 
    const std::string& type, SymbolTable* subTable) 
    : m_name(name), m_kind(kind), m_type(type), m_subTable(subTable) { }

SymbolTableEntry::~SymbolTableEntry()
{
    delete m_subTable;
}

const std::string& SymbolTableEntry::GetName() const { return m_name; }
SymbolTableEntryKind SymbolTableEntry::GetKind() const { return m_kind; }
const std::string& SymbolTableEntry::GetType() const { return m_type; }
SymbolTable* SymbolTableEntry::GetSubTable() { return m_subTable; }
const SymbolTable* SymbolTableEntry::GetSubTable() const { return m_subTable; }


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
std::string SymbolTableDisplayManager::TableToStr(SymbolTable* table, bool isMain)
{
    if (isMain)
    {
        return MainTableToStr(table);
    }
    else
    {
        return SubTableToStr(table);
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
        ss << FormatEntryRow(entry, s_mainTableWidth, 
            s_mainTableIndent, s_mainTableKindWidth, s_mainTableNameWidth) << "\n";
        
        if (entry->GetSubTable() != nullptr)
        {
            ss << TableToStr(entry->GetSubTable(), false);
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
        ss << FitInTable(FormatEntryRow(entry, s_subTableWidth, 
            s_mainTableIndent, s_subTableKindWidth, s_subTableNameWidth), 
            s_mainTableWidth, s_subTableIndent) << "\n";
    }

    ss << FitInTable(CreateHeader(s_subTableWidth), s_mainTableWidth, 
        s_subTableIndent) << "\n";

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

std::string SymbolTableDisplayManager::FormatEntryRow(SymbolTableEntry* entry, 
    size_t tableWidth, size_t indent, size_t kindWidth, size_t nameWidth)
{
    std::stringstream ss;
    
    ss << "|";

    // format kind
    for (size_t i = 0; i < indent; i++)
    {
        ss << " ";
    }

    size_t kindWidthUsed;
    {
        std::stringstream kindStream;
        kindStream << entry->GetKind();
        kindWidthUsed = kindStream.str().length();
        ss << kindStream.str();
    }

    ASSERT(kindWidthUsed < kindWidth)
    for (size_t i = kindWidthUsed; i < kindWidth - 1; i++)
    {
        ss << " ";
    }
    ss << "|";

    // format name 
    for (size_t i = 0; i < indent; i++)
    {
        ss << " ";
    }

    size_t nameWidthUsed = entry->GetName().length();
    ss << entry->GetName();
    ASSERT(nameWidthUsed < nameWidth)
    for (size_t i = nameWidthUsed; i < nameWidth - 1; i++)
    {
        ss << " ";
    }
    ss << "|";

    // format type
    for (size_t i = 0; i < indent; i++)
    {
        ss << " ";
    }

    size_t typeWidthUsed = entry->GetType().length();
    size_t previouslyUsedWidth = 3 * indent + kindWidth + nameWidth + 1; // add the initial "|"
    ss << entry->GetType();
    ASSERT(typeWidthUsed + previouslyUsedWidth < tableWidth)
    
    // remove the last |
    size_t padding = tableWidth - (typeWidthUsed + previouslyUsedWidth) - 1; 
    for (size_t i = 0; i < padding; i++)
    {
        ss << " ";
    }
    ss << "|";

    return ss.str();
}
