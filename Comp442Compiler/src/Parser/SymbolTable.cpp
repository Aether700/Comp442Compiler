#include "SymbolTable.h"
#include "../Core/Core.h"

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

std::ostream& operator<<(std::ostream& stream, const SymbolTableEntry& entry)
{
    stream << entry.GetName() << " " << entry.GetKind() << " " << entry.GetType();
    return stream;
}

// SymbolTable ////////////////////////////////////////////////////////////////////
SymbolTable::~SymbolTable()
{
    for (SymbolTableEntry* entry : m_entries)
    {
        delete entry;
    }
}

void SymbolTable::AddEntry(SymbolTableEntry* entry) { m_entries.push_front(entry); }

SymbolTable::TableIterator SymbolTable::begin() { return m_entries.begin(); }
SymbolTable::TableIterator SymbolTable::end() { return m_entries.end(); }
