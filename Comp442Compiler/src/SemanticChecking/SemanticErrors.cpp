#include "SemanticErrors.h"

#include <sstream>

// SemanticError //////////////////////////////////////////////////////////////
SemanticError::SemanticError(SemanticErrorCode errorCode) : m_errorCode(errorCode) { }
SemanticErrorCode SemanticError::GetErrorCode() const { return m_errorCode; }

// TokenBasedError //////////////////////////////////////////////////////////// 
TokenBasedError::TokenBasedError(SemanticErrorCode errorCode, const Token& token) 
    : SemanticError(errorCode), m_token(token) { }
    
const Token& TokenBasedError::GetToken() const { return m_token; }

// UnknownSymbolError ///////////////////////////////////////////////////////////
UnknownSymbolError::UnknownSymbolError(const Token& token) 
    : TokenBasedError(SemanticErrorCode::UnknownSymbol, token) { }
std::string UnknownSymbolError::GetMessage() const
{
    std::stringstream ss;
    ss << "Unknown symbol \"" << GetToken().GetLexeme() << "\" found at line " 
        << GetToken().GetLine() << ": \"" << GetToken().GetStrOfLine() << "\"";

    return ss.str();
}

// DuplicateSymbolError ///////////////////////////////////////////////////////////////
DuplicateSymbolError::DuplicateSymbolError(const Token& token1, 
    const Token& token2) : SemanticError(SemanticErrorCode::DuplicateSymbolName)
{
    if (token1.GetLine() < token2.GetLine())
    {
        m_originalToken = token1;
        m_duplicateToken = token2;
    }
    else
    {
        m_originalToken = token2;
        m_duplicateToken = token1;
    }
}

std::string DuplicateSymbolError::GetMessage() const
{
    std::stringstream ss;
    ss << "Duplicate token \"" << m_originalToken.GetLexeme() << "\" found at line " 
        << m_duplicateToken.GetLine() << ": " << m_duplicateToken.GetStrOfLine() 
        << ". Symbol has already been defined at line " << m_originalToken.GetLine() 
        << ": " << m_originalToken.GetStrOfLine(); 

    return ss.str();
}

// SemanticErrorManager ////////////////////////////////////////////////////////
void SemanticErrorManager::AddError(SemanticError* error)
{
    GetInstance().m_errors.push_back(error);
}

bool SemanticErrorManager::HasError() { return GetInstance().m_errors.size() > 0; }

void SemanticErrorManager::LogErrors() 
{
    for (SemanticError* error : GetInstance().m_errors)
    {
        std::cout << error->GetMessage() << "\n";
    }
}

void SemanticErrorManager::Clear()
{
    SemanticErrorManager& manager = GetInstance();
    for (SemanticError* error : manager.m_errors)
    {
        delete error;
    }
    manager.m_errors.clear();
}

SemanticErrorManager::SemanticErrorManager() { }
SemanticErrorManager::~SemanticErrorManager() { Clear(); }

SemanticErrorManager& SemanticErrorManager::GetInstance()
{
    static SemanticErrorManager manager;
    return manager;
}