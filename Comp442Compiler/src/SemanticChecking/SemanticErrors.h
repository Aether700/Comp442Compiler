#pragma once
#include "../Core/Token.h"

#include <string>
#include <list>

class BaseBinaryOperator;
class AssignStatNode;
class FuncCallNode;
class TypeNode;
class AParamListNode;

// Codes ////////////////////////////////////////////////////////////////////////
enum class SemanticErrorCode
{
    UndeclaredSymbol,
    DuplicateSymbolName,
    ConstructorDeclNotFound,
    MemberFunctionDeclNotFound,
    MemFuncDefNotFound,
    ConstructorDefNotFound,
    CircularInheritanceDependency,
    CircularClassMemberDependency,
    InvalidOperandForOperator,
    InvalidTypeMatchupForAssign,
    IncorrectParametersProvidedToFuncCall,
    UnknownMember,
    IncorrectReturnType,
    MissingReturnStat,
    IncorrectNumberOfMainFunc,
    OperationOnArray,
    ArrayIndexingType,
    InvalidDotOperatorUsage,
    ProhibitedAccessToPrivateMember,
    IncorrectArrayDimensionUsed
};

enum class SemanticWarningCode
{
    OverloadedFreeFunc,
    OverloadedMemFunc,
    OverloadedConstructor,
    MemOverShadowingMem,
    LocalVarOverShadowingMem,
    MainHasParameters,
    OverridenFunc,
};

class SemanticMessage
{
public:
    virtual void SendToMessagePrinter() = 0;
};

// Warnings ////////////////////////////////////////////////////////////////////////
class SemanticWarning : public SemanticMessage
{
public:
    SemanticWarning(SemanticWarningCode code);
    SemanticWarningCode GetWarningCode() const;
    virtual std::string GetMessage() const = 0;
    std::string ToString() const;

private:
    SemanticWarningCode m_warningCode;
};

class TokenBasedWarning : public SemanticWarning
{
public:
    TokenBasedWarning(SemanticWarningCode code, const Token& t);
    const Token& GetToken() const;

    virtual void SendToMessagePrinter() override;

private:
    Token m_token;
};

class TokenPairBasedWarning : public SemanticWarning
{
public:
    TokenPairBasedWarning(SemanticWarningCode code, const Token& t1, const Token& t2);
    const Token& GetToken1() const;
    const Token& GetToken2() const;

private:
    Token m_t1;
    Token m_t2;
};

class OverloadedFreeFuncWarn : public TokenBasedWarning
{
public:
    OverloadedFreeFuncWarn(const Token& funcName);
    virtual std::string GetMessage() const override;
};

class OverloadedMemFuncWarn : public TokenPairBasedWarning
{
public:
    OverloadedMemFuncWarn(const Token& classID, const Token& funcName);
    virtual std::string GetMessage() const override;
    virtual void SendToMessagePrinter() override;
};

class OverloadedConstructorWarn : public TokenBasedWarning
{
public:
    OverloadedConstructorWarn(const Token& classID);
    virtual std::string GetMessage() const override;
};

class MemOverShadowingMemWarn : public TokenPairBasedWarning
{
public:
    MemOverShadowingMemWarn(const Token& classID, const Token& member);
    virtual std::string GetMessage() const override;
    virtual void SendToMessagePrinter() override;
};

class LocalVarOverShadowingMem : public TokenPairBasedWarning
{
public:
    LocalVarOverShadowingMem(const Token& classID, const Token& localVar);
    virtual std::string GetMessage() const override;
    virtual void SendToMessagePrinter() override;
};

class MainHasParametersWarn : public SemanticWarning
{
public:
    MainHasParametersWarn();
    virtual std::string GetMessage() const override;
    virtual void SendToMessagePrinter() override;
};

class OverridenFuncWarn : public TokenBasedWarning
{
public:
    OverridenFuncWarn(const std::string& currClassID, const std::string& parentClassID, 
        const Token& funcName, const std::string& paramStr);
    virtual std::string GetMessage() const override;

private:
    std::string m_currClassID;
    std::string m_parentClassID;
    std::string m_params;
};

// Errors ////////////////////////////////////////////////////////////////////////
class SemanticError : public SemanticMessage
{
public:
    SemanticError(SemanticErrorCode errorCode);
    SemanticErrorCode GetErrorCode() const;
    virtual std::string GetMessage() const = 0;
    std::string ToString() const;

private:
    SemanticErrorCode m_errorCode;
};

class TokenBasedError : public SemanticError
{
public:
    TokenBasedError(SemanticErrorCode errorCode, const Token& token);
    
    const Token& GetToken() const;
    virtual void SendToMessagePrinter() override;
private:
    Token m_token;
};

class UndeclaredSymbolError : public TokenBasedError
{
public:
    UndeclaredSymbolError(const Token& token);
    virtual std::string GetMessage() const override;
};

class DuplicateSymbolError : public SemanticError
{
public:
    DuplicateSymbolError(const Token& token1, const Token& token2);
    virtual std::string GetMessage() const override; 
    virtual void SendToMessagePrinter() override;
private:
    Token m_originalToken;
    Token m_duplicateToken;
};

class ConstructorDeclNotFound : public TokenBasedError
{
public:
    ConstructorDeclNotFound(const Token& classID);
    virtual std::string GetMessage() const override;
};


class MemberFunctionDeclNotFound : public SemanticError
{
public:
    MemberFunctionDeclNotFound(const Token& classID, const Token& funcName);
    virtual std::string GetMessage() const override;

    virtual void SendToMessagePrinter() override;

private:
    Token m_classIDToken;
    Token m_funcNameToken;
};

class MemFuncDefNotFoundError : public SemanticError
{
public:
    MemFuncDefNotFoundError(const Token& classID, const Token& funcName);
    virtual std::string GetMessage() const override;
    virtual void SendToMessagePrinter() override;

private:
    Token m_classID;
    Token m_funcDecl;
};

class ConstructorDefNotFoundError : public SemanticError
{
public:
    ConstructorDefNotFoundError(const Token& classID, const Token& constructorToken);
    virtual std::string GetMessage() const override;
    virtual void SendToMessagePrinter() override;

private:
    Token m_classID;
    Token m_constructor;
};

class CircularInheritanceDependencyError : public TokenBasedError
{
public:
    CircularInheritanceDependencyError(const Token& classID);
    virtual std::string GetMessage() const override;
};

class CircularClassMemberDependencyError : public TokenBasedError
{
public:
    CircularClassMemberDependencyError(const std::string& classID, 
        const std::string& classOfMember, const Token& member);
    virtual std::string GetMessage() const override;

private:
    std::string m_classOfMember;
    std::string m_classID;
};

class InvalidOperandForOperatorError : public SemanticError
{
public:
    InvalidOperandForOperatorError(BaseBinaryOperator* operatorNode);
    virtual std::string GetMessage() const override;
    virtual void SendToMessagePrinter() override;

private:
    BaseBinaryOperator* m_node;
};

class InvalidTypeMatchupForAssignError : public SemanticError
{
public:
    InvalidTypeMatchupForAssignError(AssignStatNode* assignNode);
    virtual std::string GetMessage() const override;
    virtual void SendToMessagePrinter() override;

private:
    AssignStatNode* m_node;
};

class IncorrectParametersProvidedToFuncCallError : public SemanticError
{
public:
    IncorrectParametersProvidedToFuncCallError(FuncCallNode* funcCall);
    IncorrectParametersProvidedToFuncCallError(TypeNode* classType, AParamListNode* params);
    virtual std::string GetMessage() const override;
    virtual void SendToMessagePrinter() override;

private:
    FuncCallNode* m_node;
    TypeNode* m_type;
    AParamListNode* m_params;
};

class UnknownMemberError : public TokenBasedError
{
public:
    UnknownMemberError(const std::string& classID, const Token& member);
    virtual std::string GetMessage() const override;
    
private:
    std::string m_classID;
};

class IncorrectReturnTypeError : public TokenBasedError
{
public:
    IncorrectReturnTypeError(const std::string& functionName, 
        const std::string& funcReturnType, const std::string providedType, 
        const Token& returnStat);
    virtual std::string GetMessage() const override;
    
private:
    std::string m_funcName;
    std::string m_funcReturnType;
    std::string m_providedType;
};

class MissingReturnStatError : public TokenBasedError
{
public:
    MissingReturnStatError(const Token& func);
    virtual std::string GetMessage() const override;
};

class OperationOnArrayError : public TokenBasedError
{
public:
    OperationOnArrayError(const Token& t);
    virtual std::string GetMessage() const override;
};

class IncorrectNumberOfMainFuncError : public SemanticError
{
public:
    IncorrectNumberOfMainFuncError();
    virtual std::string GetMessage() const override;
    virtual void SendToMessagePrinter() override;
};

class ArrayIndexingTypeError : public TokenBasedError
{
public:
    ArrayIndexingTypeError(const Token& t);
    virtual std::string GetMessage() const override;
};

class InvalidDotOperatorUsageError : public TokenBasedError
{
public:
    InvalidDotOperatorUsageError(const Token& t);
    virtual std::string GetMessage() const override;
};


class ProhibitedAccessToPrivateMemberError : public TokenBasedError
{
public:
    ProhibitedAccessToPrivateMemberError(const Token& t);
    virtual std::string GetMessage() const override;
};

class IncorrectArrayDimensionUsedError : public TokenBasedError
{
public:
    IncorrectArrayDimensionUsedError(const Token& t);
    virtual std::string GetMessage() const override;
};

class SemanticErrorManager
{
public:
    static void AddError(SemanticError* error);
    static void AddWarning(SemanticWarning* warning);
    static bool HasError();
    static void LogData();
    static void SetFilePath(const std::string& filepath);
    static void Clear();

private:
    SemanticErrorManager();
    ~SemanticErrorManager();
    

    static SemanticErrorManager& GetInstance();

    std::list<SemanticError*> m_errors;
    std::list<SemanticWarning*> m_warnings;
    std::string m_filepath;
};