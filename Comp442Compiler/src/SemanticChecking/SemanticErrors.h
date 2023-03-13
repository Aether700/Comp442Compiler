#pragma once
#include "../Core/Token.h"

#include <string>
#include <list>

// Codes ////////////////////////////////////////////////////////////////////////
enum class SemanticErrorCode
{
    UndeclaredSymbol,
    DuplicateSymbolName,
    ConstructorDeclNotFound,
    MemberFunctionDeclNotFound,
    MemFuncDefNotFound,
    ConstructorDefNotFound,
    CircularClassDependency,
};

enum class SemanticWarningCode
{
    OverloadedFreeFunc,
    OverloadedMemFunc,
    OverloadedConstructor,
    OverShadowedMem,
};

// Warnings ////////////////////////////////////////////////////////////////////////
class SemanticWarning
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
};

class OverloadedConstructorWarn : public TokenBasedWarning
{
public:
    OverloadedConstructorWarn(const Token& classID);
    virtual std::string GetMessage() const override;
};

class OverShadowedMemWarn : public TokenPairBasedWarning
{
public:
    OverShadowedMemWarn(const Token& classID, const Token& member);
    virtual std::string GetMessage() const override;
};

// Errors ////////////////////////////////////////////////////////////////////////
class SemanticError
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

private:
    Token m_classIDToken;
    Token m_funcNameToken;
};

class MemFuncDefNotFoundError : public SemanticError
{
public:
    MemFuncDefNotFoundError(const Token& classID, const Token& funcName);
    virtual std::string GetMessage() const override;

private:
    Token m_classID;
    Token m_funcDecl;
};

class ConstructorDefNotFoundError : public SemanticError
{
public:
    ConstructorDefNotFoundError(const Token& classID, const Token& constructorToken);
    virtual std::string GetMessage() const override;

private:
    Token m_classID;
    Token m_constructor;
};

class CircularClassDependencyError : public TokenBasedError
{
public:
    CircularClassDependencyError(const Token& classID);
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