#pragma once
#include <string>
#include <list>
#include <iostream>

class ASTNode;
class VarDeclNode;
class MemVarNode;
class FunctionDefNode;
class MemFuncDeclNode;
class MemFuncDefNode;
class ConstructorDeclNode;
class ConstructorDefNode;
class ClassDefNode;
class InheritanceListNode;

class SymbolTable;
class MemFuncDefEntry;
class ConstructorDefEntry;

enum class SymbolTableEntryKind
{
    LocalVariable,
    FreeFunction,
    Class,
    InheritanceList,
    MemVar,
    ConstructorDecl,
    ConstructorDef,
    MemFuncDecl,
    MemFuncDef,
    Parameter
};

std::ostream& operator<<(std::ostream& stream, SymbolTableEntryKind kind);

class SymbolTableEntry
{
    friend class SymbolTable;
public:
    SymbolTableEntry(SymbolTableEntryKind kind);
    virtual ~SymbolTableEntry();

    const std::string& GetName() const;
    SymbolTableEntryKind GetKind() const;
    SymbolTable* GetParentTable();
    virtual ASTNode* GetNode() = 0;
    virtual SymbolTable* GetSubTable() = 0;
    
    virtual std::string ToString() = 0;

protected:
    static constexpr const char* s_seperator = "    | ";

    void SetName(const std::string& name);
private:
    std::string m_name;
    SymbolTableEntryKind m_kind;
    SymbolTable* m_parentTable;
};

class VarSymbolTableEntry : public SymbolTableEntry
{
public:
    VarSymbolTableEntry(VarDeclNode* node, const std::string& typeStr, 
        SymbolTableEntryKind kind);

    const std::string& GetType() const;
    virtual SymbolTable* GetSubTable() override;
    virtual ASTNode* GetNode() override;
    
    virtual std::string ToString() override;

private:
    VarDeclNode* m_node;
    std::string m_typeStr;
};

class FreeFuncTableEntry : public SymbolTableEntry
{
public:
    FreeFuncTableEntry(FunctionDefNode* node, const std::string& parametersType, 
        SymbolTable* subTable);
    virtual ~FreeFuncTableEntry();

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

class ClassTableEntry : public SymbolTableEntry
{
public:
    ClassTableEntry(ClassDefNode* node, SymbolTable* subTable);
    ~ClassTableEntry();

    virtual SymbolTable* GetSubTable() override;
    virtual ASTNode* GetNode() override;
    
    virtual std::string ToString() override;

private:
    ClassDefNode* m_classNode;
    SymbolTable* m_subTable;
};

class InheritanceListEntry : public SymbolTableEntry
{
public:
    InheritanceListEntry(InheritanceListNode* node);
    
    const std::string& GetClassID() const; 
    bool IsChildOf(const std::string& parentClassName) const;
    virtual SymbolTable* GetSubTable() override;
    virtual ASTNode* GetNode() override;
    
    virtual std::string ToString() override;

private:
    InheritanceListNode* GetNonConstNode() const;

    InheritanceListNode* m_node;
};

class MemVarTableEntry : public VarSymbolTableEntry
{
public:
    MemVarTableEntry(MemVarNode* node, const std::string& typeStr);
    const std::string& GetClassID() const;
    const std::string& GetVisibility() const;
    virtual std::string ToString() override;

private:
    MemVarNode* GetMemVarNode() const;
};

class MemFuncTableEntry : public SymbolTableEntry
{
public:
    MemFuncTableEntry(MemFuncDeclNode* node, const std::string& parameterTypes);
    ~MemFuncTableEntry();

    const std::string& GetClassID() const;
    const std::string& GetVisibility() const;
    const std::string& GetReturnType() const;
    const std::string& GetParamTypes() const;

    void SetDefinition(MemFuncDefEntry* defEntry);

    bool HasDefinition() const;
    MemFuncDeclNode* GetDeclNode();
    virtual ASTNode* GetNode() override;
    virtual SymbolTable* GetSubTable() override;

    virtual std::string ToString() override;

private:
    std::string m_parameterTypes;
    MemFuncDeclNode* m_declaration;
    MemFuncDefNode* m_definition;
    SymbolTable* m_definitionSubTable;
};

// temporary entry used to store the definition of a MemFunc
class MemFuncDefEntry : public SymbolTableEntry
{
    friend class MemFuncTableEntry;
public:
    MemFuncDefEntry(MemFuncDefNode* def, const std::string& parametersType, 
        SymbolTable* subTable);
    ~MemFuncDefEntry();

    const std::string& GetClassID() const;
    const std::string& GetReturnType() const;
    const std::string& GetParamTypes() const;

    virtual ASTNode* GetNode() override;
    virtual SymbolTable* GetSubTable() override;

    virtual std::string ToString() override;
private:
    MemFuncDefNode* m_defNode;
    SymbolTable* m_subTable;
    std::string m_parameterTypes;
};

class ConstructorTableEntry : public SymbolTableEntry
{
public:
    ConstructorTableEntry(ConstructorDeclNode* node, const std::string& parameterTypes);
    ~ConstructorTableEntry();

    const std::string& GetClassID() const;
    const std::string& GetVisibility() const;
    const std::string& GetReturnType() const;
    const std::string& GetParamTypes() const;

    void SetDefinition(ConstructorDefEntry* defEntry);

    bool HasDefinition() const;
    ConstructorDeclNode* GetDeclNode();
    virtual ASTNode* GetNode() override;
    virtual SymbolTable* GetSubTable() override;

    virtual std::string ToString() override;

private:
    std::string m_parameterTypes;
    ConstructorDeclNode* m_declaration;
    ConstructorDefNode* m_definition;
    SymbolTable* m_definitionSubTable;
};

// temporary entry used to store the definition of a constructor
class ConstructorDefEntry : public SymbolTableEntry
{
    friend class ConstructorTableEntry;
public:
    ConstructorDefEntry(ConstructorDefNode* node, const std::string& parameterTypes, 
        SymbolTable* subTable);
    ~ConstructorDefEntry();

    const std::string& GetClassID() const;
    const std::string& GetReturnType() const;
    const std::string& GetParamTypes() const;

    virtual ASTNode* GetNode() override;
    virtual SymbolTable* GetSubTable() override;

    virtual std::string ToString() override;
private:
    ConstructorDefNode* m_defNode;
    SymbolTable* m_subTable;
    std::string m_parameterTypes;  
};

class SymbolTable
{
public:
    using TableList = std::list<SymbolTableEntry*>;
    using TableIterator = TableList::iterator;
    SymbolTable(const std::string& name);
    ~SymbolTable();

    const std::string& GetName() const;

    // returns the a pointer to an existing entry in the table with the same 
    // name or nullptr if no such entry was found
    SymbolTableEntry* AddEntry(SymbolTableEntry* entry);
    SymbolTableEntry* AddEntryFirst(SymbolTableEntry* entry);
    
    SymbolTable* GetParentTable();
    SymbolTableEntry* GetParentEntry();
    void SetParentEntry(SymbolTableEntry* entry);

    // returns the entry with the provided name if it exists in the scope/ table 
    // or nullptr if it could not be found
    SymbolTableEntry* FindEntryInTable(const std::string& name);
    SymbolTableEntry* FindEntryInScope(const std::string& name);

    bool TableContainsName(const std::string& name);
    bool ScopeContainsName(const std::string& name);

    TableIterator begin();
    TableIterator end();

private:
    // returns the existing with the same name and, if applicable parameters, 
    // if such an entry already exists in the table or nullptr otherwise
    SymbolTableEntry* FindExistingEntry(SymbolTableEntry* entry);

    SymbolTableEntry* FindInInheritanceScope(const std::string& name);

    std::string m_name;
    TableList m_entries;
    SymbolTableEntry* m_parentEntry;
};

class SymbolTableDisplayManager
{
public:
    static std::string TableToStr(SymbolTable* table, size_t depth = 0);

private:
    static constexpr const char* s_namePrefix = "table: ";
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