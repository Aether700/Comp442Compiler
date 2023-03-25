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
		size_t currClassSize = FindSize(GetGlobalTable(element->GetSymbolTable()), id->GetID().GetLexeme());
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

CodeGenerator::CodeGenerator(SymbolTable* globalTable, const std::string& filepath) : m_filepath(filepath), 
	m_topOfStackRegister(14), m_zeroRegister(0), m_jumpReturnRegister(15), m_returnValRegister(13),
	m_registerStack({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}), m_tagGen("memAddress") 
{
	m_scopeSizeStack.push_front(globalTable->FindEntryInTable("main")->GetSize());
}

void CodeGenerator::Visit(ExprNode* element)
{
	std::stringstream ss;
	RegisterID exprRoot;
	ss << "\n" << LoadVarInRegister(element->GetRootOfExpr(), exprRoot);
	ss << "sw " << GetOffset(element) << "(r" << m_topOfStackRegister << "), r" << exprRoot << "\n";

	GetCurrStatBlock(element) += ss.str();
	m_registerStack.push_front(exprRoot);
}

void CodeGenerator::Visit(ModifiedExpr* element)
{
	GetCurrStatBlock(element) += GenerateModifiedExpr(element);
}

void CodeGenerator::Visit(BaseBinaryOperator* element)
{
	GetCurrStatBlock(element) += GenerateBinaryOp(element);
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
	ss << ComputeVal(element->GetCondition(), relExpr);
	const std::string& ifBlock = m_statBlocks[element->GetIfBranch()];
	const std::string& elseBlock = m_statBlocks[element->GetElseBranch()];

	std::string startElse = m_tagGen.GetNextTag();
	std::string endElse = m_tagGen.GetNextTag();

	ss << "bz r" << relExpr << ", " << startElse << "\n";
	ss <<  "%if block\n" << ifBlock << "%end if block\n";
	ss << "j " << endElse << "\n%else block\n";
	ss << startElse << " " << elseBlock << "%end else block\n";
	ss << endElse << " ";

	GetCurrStatBlock(element) += ss.str();

	m_registerStack.push_back(relExpr);
}

void CodeGenerator::Visit(WhileStatNode* element)
{
	std::stringstream ss;
	ss << "\n%while statement\n";
	RegisterID relExpr;
	const std::string& loopBody = m_statBlocks[element->GetStatBlock()];

	std::string topLoop = m_tagGen.GetNextTag();
	std::string endLoop = m_tagGen.GetNextTag();

	ss << topLoop << " " << ComputeVal(element->GetCondition(), relExpr);
	ss << "bz r" << relExpr << ", " << endLoop << "\n";
	ss << loopBody;
	ss << "j " << topLoop << "\n";
	ss << endLoop << " ";

	GetCurrStatBlock(element) += ss.str();

	m_registerStack.push_back(relExpr);
}

void CodeGenerator::Visit(AssignStatNode* element)
{
	std::stringstream ss;
	RegisterID right;
	ss << "\n%assignment\n";
	ss << ComputeVal(element->GetRight(), right);
	ASSERT(right != NullRegister);
	
	ss << "sw " << GetOffset(element) << "(r" << m_topOfStackRegister << "), r" << right << "\n";
	GetCurrStatBlock(element) += ss.str();

	m_registerStack.push_front(right);
}

void CodeGenerator::Visit(WriteStatNode* element)
{
	std::stringstream ss;
	
	RegisterID exprValRegister;
	std::string codeToAdd = LoadTempVarInRegister(element->GetExpr(), exprValRegister);
	ASSERT(exprValRegister != NullRegister);
	ss << "\n%write stat\n" << codeToAdd;
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
	ss << DecrementStackFrame();

	GetCurrStatBlock(element) += ss.str();
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
	size_t change = m_scopeSizeStack.front();
	m_scopeSizeStack.push_front(frameSize);
	std::stringstream ss;
	ss << "\naddi r" << m_topOfStackRegister << ", r" << m_topOfStackRegister << ", -" << change << "\n";
	return ss.str();
}

std::string CodeGenerator::DecrementStackFrame()
{
	m_scopeSizeStack.pop_front();
	size_t change = m_scopeSizeStack.front();
	std::stringstream ss;
	ss << "\nsubi r" << m_topOfStackRegister << ", r" << m_topOfStackRegister << ", -" << change << "\n";
	return ss.str();
}

std::string CodeGenerator::GenerateModifiedExpr(ModifiedExpr* modExpr)
{
	ASTNode* modifier = modExpr->GetModifier();
	if (dynamic_cast<SignNode*>(modifier) != nullptr)
	{
		SignNode* sign = (SignNode*)modifier;
		if (sign->GetSign().GetLexeme() == "-")
		{
			return GenerateMinusExpr(modExpr);
		}
		else
		{
			RegisterID expr;
			std::stringstream ss;
			ss << "\n% + expr\n";
			ss << ComputeVal(modExpr->GetExpr(), expr);
			ss << "sw " << GetOffset(modExpr) << "(r" << m_topOfStackRegister << "), r" << expr << "\n";
			return ss.str();
		}
	}
	else if (dynamic_cast<NotNode*>(modifier) != nullptr)
	{
		return GenerateNot(modExpr);
	}
	else
	{
		// unknown modifier
		DEBUG_BREAK();
	}
	return "";
}

std::string CodeGenerator::GenerateMinusExpr(ModifiedExpr* modExpr)
{
	std::stringstream ss;
	RegisterID exprReg;
	ss << "\n% minus expr\n" << ComputeVal(modExpr->GetExpr(), exprReg);
	RegisterID tempVarReg = m_registerStack.front();
	ss << "muli r" << tempVarReg << ", r" << exprReg << ", -1\n";
	ss << "sw " << GetOffset(modExpr) << "(r" << m_topOfStackRegister << "), r" << tempVarReg << "\n";

	return ss.str();
}

std::string CodeGenerator::GenerateBinaryOp(BaseBinaryOperator* opNode)
{
	const std::string& op = opNode->GetOperator()->GetOperator().GetLexeme();
	if (op == "+")
	{
		return GenerateAddOp(opNode);
	}
	else if (op == "-")
	{
		return GenerateSubOp(opNode);
	}
	else if (op == "*")
	{
		return GenerateMultOp(opNode);
	}
	else if (op == "/")
	{
		return GenerateDivOp(opNode);
	}
	else if (op == "or")
	{
		return GenerateOr(opNode);
	}
	else if (op == "and")
	{
		return GenerateAnd(opNode);
	}
	else if (op == "==")
	{
		return GenerateEqual(opNode);
	}
	else if (op == "<>")
	{
		return GenerateNotEqual(opNode);
	}
	else if (op == "<")
	{
		return GenerateLessThan(opNode);
	}
	else if (op == ">")
	{
		return GenerateGreaterThan(opNode);
	}
	else if (op == "<=")
	{
		return GenerateLessOrEqual(opNode);
	}
	else if (op == ">=")
	{
		return GenerateGreaterOrEqual(opNode);
	}
	else
	{
		// unknown operator
		DEBUG_BREAK();
	}
	return "";
}

std::string CodeGenerator::GenerateAddOp(BaseBinaryOperator* opNode)
{
	return GenerateArithmeticOp(opNode, "add");
}

std::string CodeGenerator::GenerateSubOp(BaseBinaryOperator* opNode)
{
	return GenerateArithmeticOp(opNode, "sub");
}

std::string CodeGenerator::GenerateMultOp(BaseBinaryOperator* opNode)
{
	return GenerateArithmeticOp(opNode, "mul");
}

std::string CodeGenerator::GenerateDivOp(BaseBinaryOperator* opNode)
{
	return GenerateArithmeticOp(opNode, "div");
}


std::string CodeGenerator::GenerateOr(BaseBinaryOperator* opNode)
{
	return "\n%or operator\n" + GenerateAndOr(opNode, "add");
}

std::string CodeGenerator::GenerateAnd(BaseBinaryOperator* opNode)
{
	return "\n%and operator\n" + GenerateAndOr(opNode, "mul");
}

std::string CodeGenerator::GenerateNot(ModifiedExpr* expr)
{
	std::stringstream ss;
	ss << "\n% not\n";
	RegisterID exprRegister;
	ss << LoadTempVarInRegister(expr, exprRegister);

	std::string isFalse = m_tagGen.GetNextTag();
	std::string endIsFalse = m_tagGen.GetNextTag();

	RegisterID output = m_registerStack.front();

	ss << "bz r" << exprRegister << ", " << isFalse << "\n";
	ss << "addi r" << output << ", r" << m_zeroRegister << ", 0\n";
	ss << "j " << endIsFalse << "\n";
	ss << isFalse << " addi r" << output << ", r" << m_zeroRegister << ", 1\n";
	ss << endIsFalse << " sw " << GetOffset(expr) << "(r" << m_topOfStackRegister << "), r" << output << "\n";

	m_registerStack.push_front(exprRegister);
	return ss.str();
}

std::string CodeGenerator::GenerateEqual(BaseBinaryOperator* opNode)
{
	return "\n%equals\n" + GenerateRelOp(opNode, "ceq");
}

std::string CodeGenerator::GenerateNotEqual(BaseBinaryOperator* opNode)
{
	return "\n%not equals\n" + GenerateRelOp(opNode, "cne");
}

std::string CodeGenerator::GenerateLessThan(BaseBinaryOperator* opNode)
{
	return "\n%less than\n" + GenerateRelOp(opNode, "clt");
}

std::string CodeGenerator::GenerateGreaterThan(BaseBinaryOperator* opNode)
{
	return "\n%greater than\n" + GenerateRelOp(opNode, "cgt");
}

std::string CodeGenerator::GenerateLessOrEqual(BaseBinaryOperator* opNode)
{
	return "\n%less or equal\n" + GenerateRelOp(opNode, "cle");
}

std::string CodeGenerator::GenerateGreaterOrEqual(BaseBinaryOperator* opNode)
{
	return "\n%greater or equal\n" + GenerateRelOp(opNode, "cge");
}


std::string CodeGenerator::GenerateArithmeticOp(BaseBinaryOperator* opNode, const char* commandName)
{
	std::stringstream ss;
	RegisterID left;
	RegisterID right;
	ss << "\n" << LoadVarInRegister(opNode->GetLeft(), left);
	ASSERT(left != NullRegister);

	ss << LoadVarInRegister(opNode->GetRight(), right);
	ASSERT(right != NullRegister);

	RegisterID resultRegister = m_registerStack.front();
	int offset = GetOffset(opNode);

	ss << commandName << " r" << resultRegister << ", r" << left << ", r" << right << "\n";
	ss << "sw " << offset << "(r" << m_topOfStackRegister << "), r" << resultRegister << "\n";

	m_registerStack.push_front(right);
	m_registerStack.push_front(left);

	return ss.str();
}

std::string CodeGenerator::GenerateAndOr(BaseBinaryOperator* opNode, const char* commandName)
{
	std::stringstream ss;
	RegisterID left;
	RegisterID right;
	ss << ComputeVal(opNode->GetLeft(), left);
	ASSERT(left != NullRegister);

	ss << ComputeVal(opNode->GetRight(), right);
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

	return ss.str();
}

std::string CodeGenerator::GenerateRelOp(BaseBinaryOperator* opNode, const char* commandName)
{
	std::stringstream ss;
	RegisterID left;
	ss << ComputeVal(opNode->GetLeft(), left);
	RegisterID right;
	ss << ComputeVal(opNode->GetRight(), right);
	RegisterID result = m_registerStack.front();

	ss << commandName <<" r" << result << ", r" << left << ", r" << right << "\n";
	ss << "sw " << GetOffset(opNode) << "(r" << m_topOfStackRegister << "), r" << result << "\n";

	m_registerStack.push_front(right);
	m_registerStack.push_front(left);

	return ss.str();
}

std::string CodeGenerator::ComputeVal(ASTNode* node, RegisterID& outRegister)
{
	if (dynamic_cast<LiteralNode*>(node) != nullptr)
	{
		return LoadVarInRegister((LiteralNode*)node, outRegister);
	}
	else if (dynamic_cast<BaseBinaryOperator*>(node) != nullptr)
	{
		return ComputeVal((BaseBinaryOperator*)node, outRegister);
	}
	else if (dynamic_cast<ModifiedExpr*>(node) != nullptr)
	{
		return ComputeVal((ModifiedExpr*)node, outRegister);
	}
	else if (dynamic_cast<ExprNode*>(node) != nullptr)
	{
		return ComputeVal((ExprNode*)node, outRegister);
	}
	else if (dynamic_cast<VariableNode*>(node) != nullptr)
	{
		return LoadVarInRegister((VariableNode*)node, outRegister);
	}

	outRegister = NullRegister;
	return "";
}

std::string CodeGenerator::ComputeVal(BaseBinaryOperator* node, RegisterID& outRegister)
{
	std::stringstream ss;
	ss << GenerateBinaryOp(node);
	ss << LoadVarInRegister(node, outRegister);
	return ss.str();
}

std::string CodeGenerator::ComputeVal(ExprNode* node, RegisterID& outRegister)
{
	return ComputeVal(node->GetRootOfExpr(), outRegister);
}

std::string CodeGenerator::ComputeVal(ModifiedExpr* node, RegisterID& outRegister)
{
	Visit(node);
	return LoadTempVarInRegister(node, outRegister);
}

std::string CodeGenerator::LoadVarInRegister(ASTNode* n, RegisterID& outRegister)
{
	if (dynamic_cast<LiteralNode*>(n) != nullptr)
	{
		LiteralNode* literal = (LiteralNode*)n;
		return LoadVarInRegister(literal, outRegister);
	}
	else if (dynamic_cast<TempVarNodeBase*>(n) != nullptr)
	{
		return LoadTempVarInRegister((TempVarNodeBase*)n, outRegister);
	}
	else if (dynamic_cast<VariableNode*>(n) != nullptr)
	{
		VariableNode* var = (VariableNode*)n;
		return LoadVarInRegister(var, outRegister);
	}
	else
	{
		// unexpected node type
		DEBUG_BREAK();
	}
	return "";
}

std::string CodeGenerator::LoadVarInRegister(LiteralNode* node, RegisterID& outRegister)
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

std::string CodeGenerator::LoadVarInRegister(VariableNode* node, RegisterID& outRegister)
{
	std::stringstream ss;
	outRegister = m_registerStack.front();
	m_registerStack.pop_front();
	ss << "lw r" << outRegister << ", " << GetOffset(node) << "(r" << m_topOfStackRegister << ")\n";

	return ss.str();
}

std::string CodeGenerator::LoadTempVarInRegister(TempVarNodeBase* tempVar, RegisterID& outRegister)
{
	std::stringstream ss;
	outRegister = m_registerStack.front();
	m_registerStack.pop_front();
	ss << "lw r" << outRegister << ", " << GetOffset(tempVar) << "(r" << m_topOfStackRegister << ")\n";
	return ss.str();
}

std::string& CodeGenerator::GetCurrStatBlock(ASTNode* node)
{
	StatBlockNode* statBlock = GetParentStatBlock(node);
	ASSERT(statBlock != nullptr);
	
	if (m_statBlocks.find(statBlock) == m_statBlocks.end())
	{
		m_statBlocks[statBlock] = "";
	}

	return m_statBlocks[statBlock];
}