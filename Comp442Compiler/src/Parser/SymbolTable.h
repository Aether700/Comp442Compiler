#pragma once
#include <string>
#include <list>
#include <iostream>

class SymbolTable;

enum class SymbolTableEntryKind
{
    LocalVariable,
    FreeFunction,
    Class,
    Parameter
};

std::ostream& operator<<(std::ostream& stream, SymbolTableEntryKind kind);


class SymbolTableEntry
{
public:
    SymbolTableEntry(const std::string& name, SymbolTableEntryKind kind, 
        const std::string& type, SymbolTable* subTable = nullptr);

    ~SymbolTableEntry();

    const std::string& GetName() const;
    SymbolTableEntryKind GetKind() const;
    const std::string& GetType() const;
    SymbolTable* GetSubTable();
    const SymbolTable* GetSubTable() const;

private:
    std::string m_name;
    SymbolTableEntryKind m_kind;
    std::string m_type;
    SymbolTable* m_subTable;
};

std::ostream& operator<<(std::ostream& stream, const SymbolTableEntry& entry);

class SymbolTable
{
    
public:
    using TableList = std::list<SymbolTableEntry*>;
    using TableIterator = TableList::iterator;

    ~SymbolTable();

    void AddEntry(SymbolTableEntry* entry);

    TableIterator begin();
    TableIterator end();

private:
    TableList m_entries;
};