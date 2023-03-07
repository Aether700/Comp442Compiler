#pragma once
#include <string>
#include <list>
#include <iostream>

class ASTNode;
class VarDeclNode;
class FunctionDefNode;

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
    SymbolTableEntry(SymbolTableEntryKind kind);
    virtual ~SymbolTableEntry();

    const std::string& GetName() const;
    SymbolTableEntryKind GetKind() const;
    virtual ASTNode* GetNode() = 0;
    virtual SymbolTable* GetSubTable() = 0;
    
    virtual std::string ToString() = 0;

protected:
    static constexpr const char* s_seperator = "    | ";

    void SetName(const std::string& name);
private:
    std::string m_name;
    SymbolTableEntryKind m_kind;
};

class VarSymbolTableEntry : public SymbolTableEntry
{
public:
    VarSymbolTableEntry(VarDeclNode* node, SymbolTableEntryKind kind);

    const std::string& GetType() const;
    virtual SymbolTable* GetSubTable() override;
    virtual ASTNode* GetNode() override;
    
    virtual std::string ToString() override;

private:
    VarDeclNode* m_node;
};

class FreeFuncTableEntry : public SymbolTableEntry
{
public:
    FreeFuncTableEntry(FunctionDefNode* node, const std::string& parametersType, 
        SymbolTable* subTable);
    ~FreeFuncTableEntry();

    const std::string& GetReturnType() const;
    const std::string& GetParamTypes() const;

    virtual SymbolTable* GetSubTable() override;
    virtual ASTNode* GetNode() override;
    
    virtual std::string ToString() override;
private:
    SymbolTable* m_subTable;
    FunctionDefNode* m_funcNode;
    std::string m_paramTypes;
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
    static std::string TableToStr(SymbolTable* table, size_t depth = 0);

private:
    static constexpr size_t s_mainTableWidth = 120;
    static constexpr size_t s_mainTableIndent = 1;
    
    static constexpr size_t s_subTableWidth = 110;
    static constexpr size_t s_subTableIndent = 4;

    static constexpr size_t s_subSubTableWidth = 100;
    static constexpr size_t s_subSubTableIndent = 4;

    static std::string MainTableToStr(SymbolTable* table);
    static std::string SubTableToStr(SymbolTable* table);
    static std::string SubSubTableToStr(SymbolTable* table);
    static std::string CreateHeader(size_t headerLength);
    static std::string FitInTable(const std::string& content, size_t tableWidth, 
        size_t indent);
};