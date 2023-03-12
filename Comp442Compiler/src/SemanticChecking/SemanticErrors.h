#pragma once
#include "../Core/Token.h"

#include <string>
#include <list>

enum class SemanticErrorCode
{
    UnknownSymbol,
    DuplicateSymbolName,
    MemberFunctionDeclNotFound,
};

class SemanticError
{
public:
    SemanticError(SemanticErrorCode errorCode);
    SemanticErrorCode GetErrorCode() const;
    virtual std::string GetMessage() const = 0;

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

class UnknownSymbolError : public TokenBasedError
{
public:
    UnknownSymbolError(const Token& token);
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


class MemberFunctionDeclNotFound : public TokenBasedError
{
public:
    not finished needs to be designed and implemented
};


class SemanticErrorManager
{
public:
    static void AddError(SemanticError* error);
    static bool HasError();
    static void LogErrors();
    static void Clear();

private:
    SemanticErrorManager();
    ~SemanticErrorManager();
    

    static SemanticErrorManager& GetInstance();

    std::list<SemanticError*> m_errors;
};