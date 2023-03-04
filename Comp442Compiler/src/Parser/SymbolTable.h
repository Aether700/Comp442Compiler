#pragma once
#include <string>
#include <list>

class SymbolTable;

class SymbolTableEntry
{
public:
    SymbolTableEntry(const std::string& name, const std::string& kind, 
        const std::string& type, SymbolTable* subTable = nullptr);

    ~SymbolTableEntry();

    const std::string& GetName() const;
    const std::string& GetKind() const;
    const std::string& GetType() const;
    SymbolTable* GetSubTable();
    const SymbolTable* GetSubTable() const;

private:
    std::string m_name;
    std::string m_kind;
    std::string m_type;
    SymbolTable* m_subTable;
};

class SymbolTable
{
    using TableList = std::list<SymbolTableEntry>;
    
public:
    void AddEntry(const std::string& name, const std::string& kind, 
        const std::string& type, SymbolTable* subTable = nullptr);

private:
    TableList m_entries;
};