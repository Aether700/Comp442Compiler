#include "CodeGeneration.h"
#include "../Parser/AST.h"
#include "../Parser/ASTUtil.h"
#include "../Core/Core.h"
#include "../Core/Util.h"

#include <fstream>

// PlatformSpecifications //////////////////////////////////////////////////////
size_t PlatformSpecifications::GetIntStrSize() { return s_intstrStackFrameSize; }
int PlatformSpecifications::GetIntStrArg1Offset() { return s_intstrFirstArgOffset; }
int PlatformSpecifications::GetIntStrArg2Offset() { return s_intstrSecondArgOffset; }

int PlatformSpecifications::GetPutStrArg1Offset() { return s_putstrFirstArgOffset; }

size_t PlatformSpecifications::GetAddressSize() { return s_addressSize; }
size_t PlatformSpecifications::GetIntSize() { return s_intSize; }
size_t PlatformSpecifications::GetFloatSize() { return s_floatSize; }

// SizeGenerator ////////////////////////////////////////////////////////////////////////
SizeGenerator::SizeGenerator(SymbolTable* globalTable) : m_globalTable(globalTable) { }

void SizeGenerator::Visit(ExprNode* element)
{
	CreateTempVar(element);
}

void SizeGenerator::Visit(ModifiedExpr* element)
{
	CreateTempVar(element);
}

void SizeGenerator::Visit(BaseBinaryOperator* element)
{
	auto temp = element->ToString();
	CreateTempVar(element);
}

void SizeGenerator::Visit(VarDeclNode* element)
{
	TryAddTempVar(element);
}

void SizeGenerator::Visit(FParamNode* element)
{
	TryAddTempVar(element);
}

void SizeGenerator::Visit(FunctionDefNode* element)
{
	ProcessFunc(element, SymbolTableEntryKind::FreeFunction);
}

void SizeGenerator::Visit(MemVarNode* element)
{
	TryAddTempVar(element);
}

void SizeGenerator::Visit(MemFuncDeclNode* element)
{
	ProcessFunc(element, SymbolTableEntryKind::MemFuncDecl);
}

void SizeGenerator::Visit(ConstructorDeclNode* element)
{
	ProcessFunc(element, SymbolTableEntryKind::ConstructorDecl);
}

void SizeGenerator::Visit(ClassDefNode* element)
{
	size_t classSize = 0;
	for (ASTNode* baseNode : element->GetInheritanceList()->GetChildren())
	{
		IDNode* id = (IDNode*)baseNode;
		size_t currClassSize = FindSize(id->GetID().GetLexeme());
		if (currClassSize == InvalidSize)
		{
			m_toRevisit.push_back(element);
			return;
		}
		classSize += currClassSize;
	}

	for (SymbolTableEntry* entry : *element->GetSymbolTable())
	{
		if (entry->GetKind() == SymbolTableEntryKind::MemVar)
		{
			if (entry->GetSize() == InvalidSize)
			{
				m_toRevisit.push_back(element);
				return;
			}
			classSize += entry->GetSize();
		}
	}

	element->GetSymbolTable()->GetParentEntry()->SetSize(classSize);
}

void SizeGenerator::Visit(ProgramNode* element)
{
	// revisit nodes that have to be revisited
	while (m_toRevisit.size() > 0)
	{
		std::list<ASTNode*> copy = m_toRevisit;
		m_toRevisit.clear();
		for (ASTNode* n : copy)
		{
			n->AcceptVisit(this);
		}
	}

	// generate offsets
	for (SymbolTableEntry* entry : *m_globalTable)
	{
		switch(entry->GetKind())
		{
		case SymbolTableEntryKind::Class:
			ComputeClassOffsets((ClassTableEntry*)entry);
			break;

		case SymbolTableEntryKind::FreeFunction:
			ComputeOffsets(entry->GetSubTable());
			break;
		}
	}

}

size_t SizeGenerator::ComputeSize(TypeNode* type, DimensionNode* dimensions)
{
	const std::string& typeStr = type->GetType().GetLexeme();
	size_t baseSize;
	if (typeStr == "integer")
	{
		baseSize = PlatformSpecifications::GetIntSize();
	}
	else if (typeStr == "float")
	{
		baseSize = PlatformSpecifications::GetFloatSize();
	}
	else if (typeStr == "bool")
	{
		baseSize = PlatformSpecifications::GetFloatSize();
	}
	else
	{
		baseSize = FindSize(typeStr);
	}

	if (baseSize == InvalidSize)
	{
		return InvalidSize;
	}

	size_t totalSize = baseSize;
	if (dimensions != nullptr)
	{
		for (ASTNode* baseNode : dimensions->GetChildren())
		{
			if (dynamic_cast<UnspecificedDimensionNode*>(baseNode) != nullptr)
			{
				// check what to do with unspecified nodes for now debug break
				DEBUG_BREAK();
			}
			else
			{
				LiteralNode* literal = (LiteralNode*)baseNode;
				ASSERT(literal->GetEvaluatedType() == "integer");
				int lexemInt = std::stoi(literal->GetLexemeNode()->GetID().GetLexeme());
				ASSERT(lexemInt > 0);
				totalSize *= ((size_t)lexemInt);
			}
		}
	}
	return totalSize;
}

size_t SizeGenerator::ComputeSize(const std::string& typeStr)
{
	if (typeStr == "integer")
	{
		return PlatformSpecifications::GetIntSize();
	}
	else if (typeStr == "float")
	{
		return PlatformSpecifications::GetFloatSize();
	}
	else
	{
		return InvalidSize;
	}
}

size_t SizeGenerator::FindSize(const std::string& typeStr)
{
	SymbolTableEntry* classEntry = m_globalTable->FindEntryInTable(typeStr);
	ASSERT(classEntry != nullptr);
	return classEntry->GetSize();
}

void SizeGenerator::TryAddTempVar(VarDeclNode* element)
{
	size_t size = ComputeSize(element->GetType(), element->GetDimension());
	if (size != InvalidSize)
	{
		SymbolTable* table = element->GetSymbolTable();
		SymbolTableEntry* entry = table->FindEntryInTable(element->GetID()->GetID().GetLexeme());
		ASSERT(entry != nullptr);
		entry->SetSize(size);
	}
	else
	{
		m_toRevisit.push_back(element);
	}
}

size_t SizeGenerator::ComputeSizeOfFunc(SymbolTable* funcTable)
{
	size_t totalSize = 0;
	for (SymbolTableEntry* entry : *funcTable)
	{
		if (entry->GetSize() == InvalidSize)
		{
			return InvalidSize;
		}
		totalSize += entry->GetSize();
	}
	return totalSize;
}

void SizeGenerator::ProcessFunc(ASTNode* func, SymbolTableEntryKind expectedKind)
{
	SymbolTableEntry* funcEntry = func->GetSymbolTable()->GetParentEntry();
	ASSERT(funcEntry != nullptr && funcEntry->GetKind() == expectedKind);
	size_t funcSize = ComputeSizeOfFunc(funcEntry->GetSubTable());
	if (funcSize == InvalidSize)
	{
		m_toRevisit.push_back(func);
	}
	else
	{
		funcEntry->SetSize(funcSize);
	}
}

void SizeGenerator::ComputeOffsets(SymbolTable* table, int startOffset)
{
	int offset = startOffset;
	for (SymbolTableEntry* entry : *table)
	{
		switch(entry->GetKind())
		{
		case SymbolTableEntryKind::MemVar:
		case SymbolTableEntryKind::LocalVariable:
		case SymbolTableEntryKind::TempVar:
		case SymbolTableEntryKind::Parameter:
			entry->SetOffset(offset);
			offset -= (int)entry->GetSize();
			break;
		}
	}
}

void SizeGenerator::ComputeClassOffsets(ClassTableEntry* classEntry)
{
	// if the offsets for this class has already been computed return
	if (ClassHasOffsets(classEntry))
	{
		return;
	}

	ClassDefNode* classNode = (ClassDefNode*)classEntry->GetNode();
	if (classNode->GetInheritanceList()->GetNumChild() > 0)
	{
		// compute offsets for every parent class
		int offset = 0;
		for (ASTNode* baseNode : classNode->GetInheritanceList()->GetChildren())
		{
			IDNode* id = (IDNode*)baseNode;
			SymbolTableEntry* parentClassEntry = m_globalTable->FindEntryInTable(id->GetID().GetLexeme());
			ASSERT(parentClassEntry->GetKind() == SymbolTableEntryKind::Class);
			ComputeClassOffsets((ClassTableEntry*)parentClassEntry);
			offset -= (int)parentClassEntry->GetSize();
		}

		// compute offsets for the current class
		ComputeOffsets(classEntry->GetSubTable(), offset);
		m_classesWithOffsets.push_front(classEntry);
	}
	else
	{
		// if this class has no parent class simply compute it's own offsets
		ComputeOffsets(classNode->GetSymbolTable());
		m_classesWithOffsets.push_front(classEntry);
	}

	// compute offsets for member functions and constructors
	for (SymbolTableEntry* entry : *classEntry->GetSubTable())
	{
		if (entry->GetKind() == SymbolTableEntryKind::ConstructorDecl
			|| entry->GetKind() == SymbolTableEntryKind::MemFuncDecl)
		{
			ComputeOffsets(entry->GetSubTable());
		}
	}
}

bool SizeGenerator::ClassHasOffsets(ClassTableEntry* classEntry)
{
	for (ClassTableEntry* entry : m_classesWithOffsets)
	{
		if (entry == classEntry)
		{
			return true;
		}
	}
	return false;
}


void SizeGenerator::CreateTempVar(TempVarNodeBase* node)
{
	std::string typeStr = node->GetEvaluatedType();
	size_t size = ComputeSize(typeStr);
	ASSERT(size != InvalidSize);
	TempVarEntry* entry = new TempVarEntry(typeStr, size);
	node->GetSymbolTable()->AddEntry(entry);
	node->SetTempVarName(entry->GetName());
}

// CodeGenerator ////////////////////////////////////////////////////////////////////////////////////////////

CodeGenerator::CodeGenerator(const std::string& filepath) : m_filepath(filepath), 
	m_topOfStackRegister(14), m_zeroRegister(0), m_jumpReturnRegister(15), m_returnValRegister(13),
	m_registerStack({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}), m_tagGen("memAddress") { }

void CodeGenerator::Visit(ModifiedExpr* element)
{
	ASTNode* modifier = element->GetModifier();
	if (dynamic_cast<SignNode*>(modifier) != nullptr)
	{
		SignNode* sign = (SignNode*)modifier;
		if (sign->GetSign().GetLexeme() == "-")
		{
			GenerateMinusExpr(element);
		}
		else
		{
			RegisterID expr;
			std::stringstream ss;
			ss << "\n% + expr\n";
			ss << StoreValInRegister(element->GetExpr(), expr);
			ss << "sw " << GetOffset(element) << "(r" << m_topOfStackRegister << "), r" << expr << "\n";
			m_currStatBlockCode += ss.str();
			m_generatedTempVarNode.push_back(element);
		}
	}
	else if (dynamic_cast<NotNode*>(modifier) != nullptr)
	{
		GenerateNot(element);
	}
	else
	{
		// unknown modifier
		DEBUG_BREAK();
	}
}

void CodeGenerator::Visit(BaseBinaryOperator* element)
{
	if (TempVarAlreadyOnStack(element))
	{
		return;
	}

	const std::string& op = element->GetOperator()->GetOperator().GetLexeme();
	if (op == "+")
	{
		GenerateAddOp(element);
	}
	else if (op == "-")
	{
		GenerateSubOp(element);
	}
	else if (op == "*")
	{
		GenerateMultOp(element);
	}
	else if (op == "/")
	{
		GenerateDivOp(element);
	}
	else if (op == "or")
	{
		GenerateOr(element);
	}
	else if (op == "and")
	{
		GenerateAnd(element);
	}
	else if (op == "==")
	{
		GenerateEqual(element);
	}
	else if (op == "<>")
	{
		GenerateNotEqual(element);
	}
	else if (op == "<")
	{
		GenerateLessThan(element);
	}
	else if (op == ">")
	{
		GenerateGreaterThan(element);
	}
	else if (op == "<=")
	{
		GenerateLessOrEqual(element);
	}
	else if (op == ">=")
	{
		GenerateGreaterOrEqual(element);
	}
	else
	{
		// unknown operator
		DEBUG_BREAK();
	}
}

void CodeGenerator::Visit(StatBlockNode* element)
{
	m_statBlocks[element] = m_currStatBlockCode;
	m_currStatBlockCode = "";
}

void CodeGenerator::Visit(FunctionDefNode* element)
{
	const std::string& funcStatBlock = m_statBlocks[element->GetBody()];
	if (element->GetID()->GetID().GetLexeme() == "main")
	{
		std::stringstream ss;
		ss << "\nentry\n";
		ss << "addi r" << m_topOfStackRegister << ",r" << m_zeroRegister << ",topaddr\n";

		m_executableCode += ss.str();
		m_executableCode += funcStatBlock;
		m_executableCode += "hlt\n";
	}
	else
	{
		DEBUG_BREAK(); // not supported yet
	}
}

void CodeGenerator::Visit(IfStatNode* element)
{
	std::stringstream ss;
	ss << "\n%if statement\n";
	RegisterID relExpr;
	ss << StoreValInRegister(element->GetCondition(), relExpr);
	const std::string& ifBlock = m_statBlocks[element->GetIfBranch()];
	const std::string& elseBlock = m_statBlocks[element->GetElseBranch()];

	std::string startElse = m_tagGen.GetNextTag();
	std::string endElse = m_tagGen.GetNextTag();

	ss << "bz r" << relExpr << ", " << startElse << "\n";
	ss << ifBlock;
	ss << "j " << endElse << "\n";
	ss << startElse << " " << elseBlock;
	ss << endElse << " ";

	m_currStatBlockCode += ss.str();
}

void CodeGenerator::Visit(AssignStatNode* element)
{
	std::stringstream ss;
	RegisterID right;
	ss << "\n%assignment\n";
	ss << StoreValInRegister(element->GetRight(), right);
	ASSERT(right != NullRegister);
	
	ss << "sw " << GetOffset(element) << "(r" << m_topOfStackRegister << "), r" << right << "\n";
	m_currStatBlockCode += ss.str();

	m_registerStack.push_front(right);
}

void CodeGenerator::Visit(WriteStatNode* element)
{
	std::stringstream ss;
	
	RegisterID exprValRegister;
	std::string codeToAdd = StoreValInRegister((ASTNode*)element->GetExpr(), exprValRegister);
	ASSERT(exprValRegister != NullRegister);
	ss << "\n" << codeToAdd;
	ss << IncrementStackFrame(PlatformSpecifications::GetIntStrSize());
	ss << "sw " << PlatformSpecifications::GetIntStrArg1Offset() << "(r" << m_topOfStackRegister 
		<< "), r" << exprValRegister << "\n";

	ss << "addi r" << exprValRegister << ", r" << m_zeroRegister << ", buff\n";
	ss << "sw " << PlatformSpecifications::GetIntStrArg2Offset() << "(r" << m_topOfStackRegister
		<< "), r" << exprValRegister << "\n";
	
	ss << "jl r" << m_jumpReturnRegister << ", intstr\n";

	ss << "sw " << PlatformSpecifications::GetPutStrArg1Offset() << "(r" << m_topOfStackRegister 
		<< "), r" << m_returnValRegister << "\n";

	ss << "jl r" << m_jumpReturnRegister << ", putstr\n";
	ss << DecrementStackFrame(PlatformSpecifications::GetIntStrSize());

	m_currStatBlockCode += ss.str();
	m_registerStack.push_front(exprValRegister);
}

void CodeGenerator::Visit(ProgramNode* element)
{
	m_dataCode = "buff res 20\n";
}

void CodeGenerator::OutputCode() const
{
	std::string moonFilepath = SimplifyFilename(m_filepath) + ".moon";
	std::ofstream outFile = std::ofstream(moonFilepath);

	outFile << m_executableCode;
	outFile << m_dataCode;
}

std::string CodeGenerator::IncrementStackFrame(size_t frameSize)
{
	std::stringstream ss;
	ss << "\naddi r" << m_topOfStackRegister << ", r" << m_topOfStackRegister << ", -" << frameSize << "\n";
	return ss.str();
}

std::string CodeGenerator::DecrementStackFrame(size_t frameSize)
{
	std::stringstream ss;
	ss << "\nsubi r" << m_topOfStackRegister << ", r" << m_topOfStackRegister << ", -" << frameSize << "\n";
	return ss.str();
}

void CodeGenerator::GenerateMinusExpr(ModifiedExpr* modExpr)
{
	std::stringstream ss;
	RegisterID exprReg;
	ss << "\n% minus expr\n" << StoreValInRegister(modExpr->GetExpr(), exprReg);
	RegisterID tempVarReg = m_registerStack.front();
	ss << "muli r" << tempVarReg << ", r" << exprReg << ", -1\n";
	ss << "sw " << GetOffset(modExpr) << "(r" << m_topOfStackRegister << "), r" << tempVarReg << "\n";

	m_currStatBlockCode += ss.str();
	m_generatedTempVarNode.push_back(modExpr);
}

void CodeGenerator::GenerateAddOp(BaseBinaryOperator* opNode)
{
	GenerateArithmeticOp(opNode, "add");
}

void CodeGenerator::GenerateSubOp(BaseBinaryOperator* opNode)
{
	GenerateArithmeticOp(opNode, "sub");
}

void CodeGenerator::GenerateMultOp(BaseBinaryOperator* opNode)
{
	GenerateArithmeticOp(opNode, "mul");
}

void CodeGenerator::GenerateDivOp(BaseBinaryOperator* opNode)
{
	GenerateArithmeticOp(opNode, "div");
}


void CodeGenerator::GenerateOr(BaseBinaryOperator* opNode)
{
	m_currStatBlockCode += "\n%or operator\n";
	GenerateAndOr(opNode, "add");
}

void CodeGenerator::GenerateAnd(BaseBinaryOperator* opNode)
{
	m_currStatBlockCode += "\n%and operator\n";
	GenerateAndOr(opNode, "mul");
}

void CodeGenerator::GenerateNot(ModifiedExpr* expr)
{
	std::stringstream ss;
	ss << "\n% not\n";
	RegisterID exprRegister;
	ss << StoreValInRegister(expr->GetExpr(), exprRegister);

	std::string isFalse = m_tagGen.GetNextTag();
	std::string endIsFalse = m_tagGen.GetNextTag();

	RegisterID output = m_registerStack.front();

	ss << "bz r" << exprRegister << ", " << isFalse << "\n";
	ss << "addi r" << output << ", r" << m_zeroRegister << ", 0\n";
	ss << "j " << endIsFalse << "\n";
	ss << isFalse << " addi r" << output << ", r" << m_zeroRegister << ", 1\n";
	ss << endIsFalse << " sw " << GetOffset(expr) << "(r" << m_topOfStackRegister << "), r" << output << "\n";

	m_currStatBlockCode += ss.str();
	m_registerStack.push_front(exprRegister);
	m_generatedTempVarNode.push_back(expr);
}

void CodeGenerator::GenerateEqual(BaseBinaryOperator* opNode)
{
	m_currStatBlockCode += "\n%equals\n";
	GenerateRelOp(opNode, "ceq");
}

void CodeGenerator::GenerateNotEqual(BaseBinaryOperator* opNode)
{
	m_currStatBlockCode += "\n%not equals\n";
	GenerateRelOp(opNode, "cne");
}

void CodeGenerator::GenerateLessThan(BaseBinaryOperator* opNode)
{
	m_currStatBlockCode += "\n%less than\n";
	GenerateRelOp(opNode, "clt");
}

void CodeGenerator::GenerateGreaterThan(BaseBinaryOperator* opNode) 
{
	m_currStatBlockCode += "\n%greater than\n";
	GenerateRelOp(opNode, "cgt");
}

void CodeGenerator::GenerateLessOrEqual(BaseBinaryOperator* opNode)
{
	m_currStatBlockCode += "\n%less or equal\n";
	GenerateRelOp(opNode, "cle");
}

void CodeGenerator::GenerateGreaterOrEqual(BaseBinaryOperator* opNode) 
{
	m_currStatBlockCode += "\n%greater or equal\n";
	GenerateRelOp(opNode, "cge");
}


void CodeGenerator::GenerateArithmeticOp(BaseBinaryOperator* opNode, const char* commandName)
{
	std::stringstream ss;
	RegisterID left;
	RegisterID right;
	ss << "\n" << StoreValInRegister(opNode->GetLeft(), left);
	ASSERT(left != NullRegister);

	ss << StoreValInRegister(opNode->GetRight(), right);
	ASSERT(right != NullRegister);

	RegisterID resultRegister = m_registerStack.front();
	int offset = GetOffset(opNode);

	ss << commandName << " r" << resultRegister << ", r" << left << ", r" << right << "\n";
	ss << "sw " << offset << "(r" << m_topOfStackRegister << "), r" << resultRegister << "\n";

	m_registerStack.push_front(right);
	m_registerStack.push_front(left);

	m_currStatBlockCode += ss.str();
}

void CodeGenerator::GenerateAndOr(BaseBinaryOperator* opNode, const char* commandName)
{
	std::stringstream ss;
	RegisterID left;
	RegisterID right;
	ss << StoreValInRegister(opNode->GetLeft(), left);
	ASSERT(left != NullRegister);

	ss << StoreValInRegister(opNode->GetRight(), right);
	ASSERT(right != NullRegister);

	RegisterID tempResult = m_registerStack.front();
	m_registerStack.pop_front();

	RegisterID result = m_registerStack.front();
	m_registerStack.pop_front();

	std::string tag1 = m_tagGen.GetNextTag();
	std::string tag2 = m_tagGen.GetNextTag();

	ss << commandName << " r" << tempResult << ", r" << left << ", r" << right << "\n";
	ss << "bz r" << tempResult << ", " << tag1 << "\n";
	ss << "addi r" << result << ", r" << m_zeroRegister << ", 1\n";
	ss << "j "<< tag2 << "\n";
	ss << tag1 << " addi r" << result << ", r" << m_zeroRegister << ", 0\n";
	ss << tag2 << " sw " << GetOffset(opNode) << "(r" << m_topOfStackRegister << "), r" << result << "\n";

	m_registerStack.push_front(result);
	m_registerStack.push_front(tempResult);
	m_registerStack.push_front(right);
	m_registerStack.push_front(left);

	m_currStatBlockCode += ss.str();
}

void CodeGenerator::GenerateRelOp(BaseBinaryOperator* opNode, const char* commandName)
{
	std::stringstream ss;
	RegisterID left;
	ss << StoreValInRegister(opNode->GetLeft(), left);
	RegisterID right;
	ss << StoreValInRegister(opNode->GetRight(), right);
	RegisterID result = m_registerStack.front();

	ss << commandName <<" r" << result << ", r" << left << ", r" << right << "\n";
	ss << "sw " << GetOffset(opNode) << "(r" << m_topOfStackRegister << "), r" << result << "\n";

	m_currStatBlockCode += ss.str();

	m_registerStack.push_front(right);
	m_registerStack.push_front(left);
}

std::string CodeGenerator::StoreValInRegister(ASTNode* node, RegisterID& outRegister)
{
	if (dynamic_cast<LiteralNode*>(node) != nullptr)
	{
		return StoreValInRegister((LiteralNode*)node, outRegister);
	}
	else if (dynamic_cast<BaseBinaryOperator*>(node) != nullptr)
	{
		return StoreValInRegister((BaseBinaryOperator*)node, outRegister);
	}
	else if (dynamic_cast<ModifiedExpr*>(node) != nullptr)
	{
		return StoreValInRegister((ModifiedExpr*)node, outRegister);
	}
	else if (dynamic_cast<ExprNode*>(node) != nullptr)
	{
		return StoreValInRegister((ExprNode*)node, outRegister);
	}
	else if (dynamic_cast<VariableNode*>(node) != nullptr)
	{
		return StoreValInRegister((VariableNode*)node, outRegister);
	}

	outRegister = NullRegister;
	return "";
}

std::string CodeGenerator::StoreValInRegister(LiteralNode* node, RegisterID& outRegister)
{
	std::stringstream ss;
	if (node->GetEvaluatedType() == "integer")
	{
		outRegister = m_registerStack.front();
		m_registerStack.pop_front();
		ss << "addi r" << outRegister << ", r" << m_zeroRegister << ", " 
			<< node->GetLexemeNode()->GetID().GetLexeme() << "\n";
	}
	else // float
	{
		DEBUG_BREAK(); // not implemented yet
	}
	return ss.str();
}

std::string CodeGenerator::StoreValInRegister(VariableNode* node, RegisterID& outRegister)
{
	std::stringstream ss;
	outRegister = m_registerStack.front();
	m_registerStack.pop_front();
	ss << "lw r" << outRegister << ", " << GetOffset(node) << "(r" << m_topOfStackRegister << ")\n";

	return ss.str();
}

std::string CodeGenerator::StoreValInRegister(BaseBinaryOperator* node, RegisterID& outRegister)
{
	if (TempVarAlreadyOnStack(node))
	{
		return LoadTempVarInRegister(node, outRegister);
	}
	Visit(node);
	return LoadTempVarInRegister(node, outRegister);
}

std::string CodeGenerator::StoreValInRegister(ExprNode* node, RegisterID& outRegister)
{
	if (TempVarAlreadyOnStack(node))
	{
		return LoadTempVarInRegister(node, outRegister);
	}
	m_generatedTempVarNode.push_back(node);
	return StoreValInRegister(node->GetRootOfExpr(), outRegister);
}

std::string CodeGenerator::StoreValInRegister(ModifiedExpr* node, RegisterID& outRegister)
{
	ASSERT(TempVarAlreadyOnStack(node));
	return LoadTempVarInRegister(node, outRegister);
}

std::string CodeGenerator::LoadTempVarInRegister(TempVarNodeBase* tempVar, RegisterID& outRegister)
{
	std::stringstream ss;
	outRegister = m_registerStack.front();
	m_registerStack.pop_front();
	ss << "lw r" << outRegister << ", " << GetOffset(tempVar) << "(r" << m_topOfStackRegister << ")\n";
	return ss.str();
}