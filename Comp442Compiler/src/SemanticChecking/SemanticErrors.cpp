#include "SemanticErrors.h"
#include "../Parser/AST.h"

#include <sstream>
#include <fstream>

// Helpers ///////////////////////////////////////////////////////////////////
std::string AParamToTypeList(AParamListNode* param)
{
    std::stringstream ss;
    bool hasParam = false;
    for (ASTNode* node : param->GetChildren())
    {
        ss << node->GetEvaluatedType() << ", ";
        hasParam = true;
    }

    if (hasParam)
    {
        std::string temp = ss.str();
        ss.str(temp.substr(0, temp.length() - 2));
    }
    return ss.str();
}

// SemanticWarning ///////////////////////////////////////////////////////////
SemanticWarning::SemanticWarning(SemanticWarningCode code) : m_warningCode(code) { }
SemanticWarningCode SemanticWarning::GetWarningCode() const { return m_warningCode; }
std::string SemanticWarning::ToString() const
{
    std::stringstream ss;
    ss << "Warning: " << GetMessage();
    return ss.str();
}

// TokenBasedWarning ///////////////////////////////////////////////////////////////
TokenBasedWarning::TokenBasedWarning(SemanticWarningCode code, const Token& t) 
    : SemanticWarning(code), m_token(t) { }
const Token& TokenBasedWarning::GetToken() const { return m_token; }

// TokenPairBasedWarning ////////////////////////////////////////////////////////////
TokenPairBasedWarning::TokenPairBasedWarning(SemanticWarningCode code, const Token& t1, 
    const Token& t2) : SemanticWarning(code), m_t1(t1), m_t2(t2) { }

const Token& TokenPairBasedWarning::GetToken1() const { return m_t1; }
const Token& TokenPairBasedWarning::GetToken2() const { return m_t2; }

// OverloadedFreeFuncWarn ////////////////////////////////////////////////////
OverloadedFreeFuncWarn::OverloadedFreeFuncWarn(const Token& funcName) 
    : TokenBasedWarning(SemanticWarningCode::OverloadedFreeFunc, funcName) { }

std::string OverloadedFreeFuncWarn::GetMessage() const
{
    std::stringstream ss;
    ss << "The free function \"" << GetToken().GetLexeme() 
        << "\" is defined multiple times with different parameters";

    return ss.str();
}

// OverloadedMemFuncWarn /////////////////////////////////////////////
OverloadedMemFuncWarn::OverloadedMemFuncWarn(const Token& classID, const Token& funcName) 
    : TokenPairBasedWarning(SemanticWarningCode::OverloadedMemFunc, classID, funcName) { }

std::string OverloadedMemFuncWarn::GetMessage() const
{
    std::stringstream ss;
    ss << "The member function \"" << GetToken1().GetLexeme() << "::" 
        << GetToken2().GetLexeme() 
        << "\" is defined multiple times with different parameters";

    return ss.str();
}

// OverloadedConstructorWarn ///////////////////////////////////////////////////
OverloadedConstructorWarn::OverloadedConstructorWarn(const Token& classID) 
    : TokenBasedWarning(SemanticWarningCode::OverloadedConstructor, classID) { }

std::string OverloadedConstructorWarn::GetMessage() const
{
    std::stringstream ss;
    ss << "The constructor for class \"" << GetToken().GetLexeme() 
        << "\" is defined multiple times with different parameters";

    return ss.str();
}

// OverShadowedMemWarn ////////////////////////////////////////////////////////
OverShadowedMemWarn::OverShadowedMemWarn(const Token& classID, const Token& member) 
    : TokenPairBasedWarning(SemanticWarningCode::OverShadowedMem, classID, member) { }

std::string OverShadowedMemWarn::GetMessage() const
{
    std::stringstream ss;
    ss << "The member \"" << GetToken2().GetLexeme() 
        << "\" of the \"" << GetToken1().GetLexeme() 
        << "\" is overshadowing a member from a parent class. At line " 
        << GetToken2().GetLine() << ": \""  
        << GetToken2().GetStrOfLine() << "\"";

    return ss.str();
}

// SemanticError //////////////////////////////////////////////////////////////
SemanticError::SemanticError(SemanticErrorCode errorCode) : m_errorCode(errorCode) { }
SemanticErrorCode SemanticError::GetErrorCode() const { return m_errorCode; }
std::string SemanticError::ToString() const
{
    std::stringstream ss;
    ss << "Error: " << GetMessage();
    return ss.str();
}

// TokenBasedError //////////////////////////////////////////////////////////// 
TokenBasedError::TokenBasedError(SemanticErrorCode errorCode, const Token& token) 
    : SemanticError(errorCode), m_token(token) { }
    
const Token& TokenBasedError::GetToken() const { return m_token; }

// UndeclaredSymbolError ///////////////////////////////////////////////////////////
UndeclaredSymbolError::UndeclaredSymbolError(const Token& token) 
    : TokenBasedError(SemanticErrorCode::UndeclaredSymbol, token) { }
std::string UndeclaredSymbolError::GetMessage() const
{
    std::stringstream ss;
    ss << "Undeclared symbol \"" << GetToken().GetLexeme() << "\" found at line " 
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
    ss << "Duplicate symbol \"" << m_originalToken.GetLexeme() << "\" found at line " 
        << m_duplicateToken.GetLine() << ": " << m_duplicateToken.GetStrOfLine() 
        << ". Symbol has already been defined at line " << m_originalToken.GetLine() 
        << ": " << m_originalToken.GetStrOfLine(); 

    return ss.str();
}

// ConstructorDeclNotFound ////////////////////////////////////////////////////////
ConstructorDeclNotFound::ConstructorDeclNotFound(const Token& classID) 
    : TokenBasedError(SemanticErrorCode::ConstructorDeclNotFound, classID) { }

std::string ConstructorDeclNotFound::GetMessage() const
{
    std::stringstream ss;
    ss << "Cannot find declaration for constructor from class \"" 
        << GetToken().GetLexeme() 
        << "\". Definition found at line " << GetToken().GetLine() 
        << ": " << GetToken().GetStrOfLine();

    return ss.str(); 
}

// MemberFunctionDeclNotFound /////////////////////////////////////////////////////
MemberFunctionDeclNotFound::MemberFunctionDeclNotFound(const Token& classID, 
    const Token& funcName) : SemanticError(SemanticErrorCode::MemberFunctionDeclNotFound), 
    m_classIDToken(classID), m_funcNameToken(funcName) { }

std::string MemberFunctionDeclNotFound::GetMessage() const
{
    std::stringstream ss;
    ss << "Cannot find declaration for the member function \"" 
        << m_classIDToken.GetLexeme() << "::" << m_funcNameToken.GetLexeme() 
        << "\" at line " << m_classIDToken.GetLine() 
        << ": " << m_classIDToken.GetStrOfLine();

    return ss.str();
}

// MemFuncDefNotFoundError ////////////////////////////////////////////////////////
MemFuncDefNotFoundError::MemFuncDefNotFoundError(const Token& classID, const Token& funcName) 
    : SemanticError(SemanticErrorCode::MemFuncDefNotFound), m_classID(classID), 
    m_funcDecl(funcName) {}

std::string MemFuncDefNotFoundError::GetMessage() const
{
    std::stringstream ss;
    ss << "Cannot find definition for the member function \"" 
        << m_classID.GetLexeme() << "::" << m_funcDecl.GetLexeme() 
        << "\" declared at line " << m_funcDecl.GetLine() 
        << ": " << m_funcDecl.GetStrOfLine();

    return ss.str();
}

// ConstructorDefNotFoundError ////////////////////////////////////////////////////
ConstructorDefNotFoundError::ConstructorDefNotFoundError(const Token& classID, 
    const Token& constructorToken) 
    : SemanticError(SemanticErrorCode::ConstructorDefNotFound), m_classID(classID), 
    m_constructor(constructorToken) { }

std::string ConstructorDefNotFoundError::GetMessage() const
{
    std::stringstream ss;
    ss << "Cannot find definition for constructor from class \"" 
        << m_classID.GetLexeme() 
        << "\". Declaration found at line " << m_constructor.GetLine() 
        << ": " << m_constructor.GetStrOfLine();

    return ss.str(); 
}

// CircularClassDependencyError ////////////////////////////////////////////////
CircularClassDependencyError::CircularClassDependencyError(const Token& classID) 
    : TokenBasedError(SemanticErrorCode::CircularClassDependency, classID) { }

std::string CircularClassDependencyError::GetMessage() const
{
    std::stringstream ss;
    ss << "Circular dependancy in the inheritance tree of class \"" 
        << GetToken().GetLexeme() 
        << "\" detected. At line " << GetToken().GetLine() 
        << ": " << GetToken().GetStrOfLine();

    return ss.str(); 
}

// InvalidOperandForOperatorError ///////////////////////////////////////////////////////
InvalidOperandForOperatorError::InvalidOperandForOperatorError(
    BaseBinaryOperator* operatorNode) 
    : SemanticError(SemanticErrorCode::InvalidOperandForOperator), 
    m_node(operatorNode) { }

std::string InvalidOperandForOperatorError::GetMessage() const
{
    std::string typeLeft = m_node->GetLeft()->GetEvaluatedType();
    std::string typeRight = m_node->GetRight()->GetEvaluatedType();

    if (typeLeft == ASTNode::InvalidType)
    {
        typeLeft = "Unknown Type";
    }

    if (typeRight == ASTNode::InvalidType)
    {
        typeRight = "Unknown Type";
    }

    const Token& op = m_node->GetOperator()->GetOperator();

    std::stringstream ss;
    ss << "The operator \"" 
        << op.GetLexeme() 
        << "\" is undefined for operands of type \"" << typeLeft << "\" and \"" << typeRight 
        << "\". At Line " << op.GetLine() << ": \"" << op.GetStrOfLine() 
        << "\"";

    return ss.str(); 
}

// InvalidTypeMatchupForAssignError ////////////////////////////////////////////////
InvalidTypeMatchupForAssignError::InvalidTypeMatchupForAssignError(
    AssignStatNode* assignNode) 
    : SemanticError(SemanticErrorCode::InvalidTypeMatchupForAssign), 
    m_node(assignNode) { }

std::string InvalidTypeMatchupForAssignError::GetMessage() const
{
    std::string typeLeft = m_node->GetLeft()->GetEvaluatedType();
    std::string typeRight = m_node->GetRight()->GetEvaluatedType();

    if (typeLeft == ASTNode::InvalidType)
    {
        typeLeft = "Unknown Type";
    }

    if (typeRight == ASTNode::InvalidType)
    {
        typeRight = "Unknown Type";
    }

    const Token& t = m_node->GetFirstToken();

    std::stringstream ss;
    ss << "Cannot assign an expression of type \"" 
        << typeRight 
        << "\" to a variable of type \"" << typeLeft << "\". At Line " 
        << t.GetLine() << ": \"" << t.GetStrOfLine() 
        << "\"";

    return ss.str();
}

// IncorrectParametersProvidedToFreeFuncCallError /////////////////////////////////
IncorrectParametersProvidedToFreeFuncCallError
    ::IncorrectParametersProvidedToFreeFuncCallError(FuncCallNode* funcCall) 
    : SemanticError(SemanticErrorCode::IncorrectParametersProvidedToFreeFuncCall), 
    m_node(funcCall) { }

std::string IncorrectParametersProvidedToFreeFuncCallError::GetMessage() const
{
    const Token& t = m_node->GetFirstToken();

    std::string paramStr = AParamToTypeList(m_node->GetParameters());

    std::stringstream ss;
    ss << "Incorrect parameters provided when calling function \"" 
        << t.GetLexeme() 
        << "\".";

    if (paramStr == "")
    {
        ss << " No arguments were provided as arguments but no function \"" 
            << t.GetLexeme() << "\" with no arguments is defined.";
    }
    else
    {
        ss << " Provided \"(" << paramStr << ")\" as arguments but no function \"" 
            << t.GetLexeme() << "\" with such arguments is defined.";
    }

    ss << " At Line " << t.GetLine() << ": \"" << t.GetStrOfLine() << "\"";

    return ss.str();
}

// SemanticErrorManager ////////////////////////////////////////////////////////
void SemanticErrorManager::AddError(SemanticError* error)
{
    GetInstance().m_errors.push_back(error);
}

void SemanticErrorManager::AddWarning(SemanticWarning* warning)
{
    GetInstance().m_warnings.push_back(warning);
}

bool SemanticErrorManager::HasError() { return GetInstance().m_errors.size() > 0; }

void SemanticErrorManager::LogData() 
{
    SemanticErrorManager& manager = GetInstance();
    std::ofstream outFile = std::ofstream(manager.m_filepath);
    for (SemanticWarning* warning : manager.m_warnings)
    {
        outFile << warning->ToString() << "\n";
    }

    for (SemanticError* error : manager.m_errors)
    {
        outFile << error->ToString() << "\n";
    }
}

void SemanticErrorManager::SetFilePath(const std::string& filepath)
{
    GetInstance().m_filepath = filepath;
}

void SemanticErrorManager::Clear()
{
    SemanticErrorManager& manager = GetInstance();
    for (SemanticError* error : manager.m_errors)
    {
        delete error;
    }
    manager.m_errors.clear();

    for (SemanticWarning* warning : manager.m_warnings)
    {
        delete warning;
    }
    manager.m_warnings.clear();
}

SemanticErrorManager::SemanticErrorManager() { }
SemanticErrorManager::~SemanticErrorManager() { Clear(); }

SemanticErrorManager& SemanticErrorManager::GetInstance()
{
    static SemanticErrorManager manager;
    return manager;
}