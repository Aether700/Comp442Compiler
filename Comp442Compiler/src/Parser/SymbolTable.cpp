#include "SymbolTable.h"

// SymbolTableEntry //////////////////////////////////////////

SymbolTableEntry::SymbolTableEntry(const std::string& name, const std::string& kind, 
    const std::string& type, SymbolTable* subTable) 
    : m_name(name), m_kind(kind), m_type(type), m_subTable(subTable) { }

SymbolTableEntry::~SymbolTableEntry()
{
    delete m_subTable;
}

const std::string& SymbolTableEntry::GetName() const { return m_name; }
const std::string& SymbolTableEntry::GetKind() const { return m_kind; }
const std::string& SymbolTableEntry::GetType() const { return m_type; }
SymbolTable* SymbolTableEntry::GetSubTable() { return m_subTable; }
const SymbolTable* SymbolTableEntry::GetSubTable() const { return m_subTable; }


// SymbolTable ////////////////////////////////////////////////////////////////////
void SymbolTable::AddEntry(const std::string& name, const std::string& kind, 
    const std::string& type, SymbolTable* subTable)
{
    m_entries.emplace_front(name, kind, type, subTable);
}
