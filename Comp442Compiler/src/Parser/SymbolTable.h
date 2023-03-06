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

class SymbolTable
{
    
public:
    using TableList = std::list<SymbolTableEntry*>;
    using TableIterator = TableList::iterator;
    SymbolTable(const std::string& name);
    ~SymbolTable();

    const std::string& GetName() const;

    void AddEntry(SymbolTableEntry* entry);

    TableIterator begin();
    TableIterator end();

private:
    std::string m_name;
    TableList m_entries;
};

class SymbolTableDisplayManager
{
public:
    static std::string TableToStr(SymbolTable* table, bool isMain = true);

private:
    static constexpr size_t s_mainTableWidth = 120;
    static constexpr size_t s_mainTableIndent = 1;
    // column size - indent 
    static constexpr size_t s_mainTableKindWidth = 25; 
    static constexpr size_t s_mainTableNameWidth = 30; 
    
    static constexpr size_t s_subTableWidth = 110;
    static constexpr size_t s_subTableIndent = 4;
    // column size - indent 
    static constexpr size_t s_subTableKindWidth = 25;
    static constexpr size_t s_subTableNameWidth = 30;

    static std::string MainTableToStr(SymbolTable* table);
    static std::string SubTableToStr(SymbolTable* table);
    static std::string CreateHeader(size_t headerLength);
    static std::string FitInTable(const std::string& content, size_t tableWidth, 
        size_t indent);
    
    static std::string FormatEntryRow(SymbolTableEntry* entry, 
        size_t tableWidth, size_t indent, size_t kindWidth, size_t nameWidth);
};