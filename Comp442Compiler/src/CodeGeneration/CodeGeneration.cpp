#include "CodeGeneration.h"
#include "../Parser/AST.h"
#include "../Parser/ASTUtil.h"
#include "../Core/Core.h"
#include "../Core/Util.h"

#include <fstream>

// SizeGenerator ////////////////////////////////////////////////////////////////////////
SizeGenerator::SizeGenerator(SymbolTable* globalTable) : m_globalTable(globalTable), m_funcTagGen("funcTag") { }

void SizeGenerator::Visit(LiteralNode* element)
{
	if (element->GetEvaluatedType() == "float")
	{
		CreateTempVar(element);
	}
}

void SizeGenerator::Visit(DotNode* element)
{
	if (IsRootDot(element))
	{
		CreateReference(element);
	}
}

void SizeGenerator::Visit(ExprNode* element)
{
	TryAddTempVar(element);
}

void SizeGenerator::Visit(ModifiedExpr* element)
{
	CreateTempVar(element);
}

void SizeGenerator::Visit(BaseBinaryOperator* element)
{
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

void SizeGenerator::Visit(FuncCallNode* element)
{
	if (element->GetEvaluatedType() != "void")
	{
		CreateTempVar(element);
	}
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

void SizeGenerator::TryAddTempVar(TempVarNodeBase* element)
{
	size_t size = ComputeSize(element->GetEvaluatedType());
	if (size == InvalidSize)
	{
		size = FindSize(GetGlobalTable(element->GetSymbolTable()), element->GetEvaluatedType());
	}

	if (size != InvalidSize)
	{
		CreateTempVar(element);
	}
	else
	{
		m_toRevisit.push_back(element);
	}
}

size_t SizeGenerator::ComputeSizeOfFunc(SymbolTable* funcTable)
{
	{
		std::string typeStr = funcTable->GetParentEntry()->GetEvaluatedType();
		if (typeStr != "void")
		{
			size_t returnValueSize = ComputeSize(typeStr);
			if (returnValueSize == InvalidSize)
			{
				returnValueSize = FindSize(GetGlobalTable(funcTable), typeStr);
			}

			if (returnValueSize == InvalidSize)
			{
				return InvalidSize;
			}

			funcTable->FindEntryInTable("returnValue")->SetSize(returnValueSize);
		}
	}

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
	TagTableEntry* funcEntry = (TagTableEntry*)func->GetSymbolTable()->GetParentEntry();
	SymbolTable* funcTable = funcEntry->GetSubTable();
	if (funcEntry->GetName() != "main")
	{
		funcTable->AddEntry(new ReturnAddressEntry());
		funcEntry->SetTag(m_funcTagGen.GetNextTag());
	}

	if (funcEntry->GetEvaluatedType() != "void")
	{
		funcTable->AddEntry(new ReturnValueEntry(funcEntry));
	}

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
		case SymbolTableEntryKind::ReturnAddress:
		case SymbolTableEntryKind::ReturnValue:
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
	if (size == InvalidSize)
	{
		size = FindSize(GetGlobalTable(node->GetSymbolTable()), typeStr);
	}
	ASSERT(size != InvalidSize);
	TempVarEntry* entry = new TempVarEntry(typeStr, size);
	node->GetSymbolTable()->AddEntry(entry);
	node->SetTempVarName(entry->GetName());
}

void SizeGenerator::CreateReference(RefVarNode* node)
{
	RefEntry* entry = new RefEntry(PlatformSpecifications::GetAddressSize());
	node->GetSymbolTable()->AddEntry(entry);
	node->SetRefVarName(entry->GetName());
}

// CodeGenerator ////////////////////////////////////////////////////////////////////////////////////////////

CodeGenerator::CodeGenerator(SymbolTable* globalTable, const std::string& filepath) : m_filepath(filepath), 
	m_topOfStackRegister(14), m_zeroRegister(0), m_jumpReturnRegister(15), m_returnValRegister(13),
	m_registerStack({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}), m_tagGen("memAddress") 
{
}

void CodeGenerator::Visit(LiteralNode* element)
{
	if (element->GetEvaluatedType() == "float")
	{
		GetCurrStatBlock(element) += LoadFloatToAddr(GetOffset(element), element);
	}
}

void CodeGenerator::Visit(DotNode* element)
{
	if (IsRootDot(element))
	{
		std::stringstream ss;
		RegisterID ref = m_registerStack.front();
		ss << "addi r" << ref << ", r" << m_topOfStackRegister << ", " << GetOffsetOfExpr(element) << "\n";
		ss << "sw " << GetOffset(element) << "(r" << m_topOfStackRegister << "), r" << ref << "\n";
		GetCurrStatBlock(element) += ss.str();
	}
}

void CodeGenerator::Visit(ExprNode* element)
{
	std::stringstream ss;
	if (ComputeSize(element->GetEvaluatedType()) <= PlatformSpecifications::GetAddressSize())
	{
		RegisterID exprRoot;
		ss << "\n" << LoadVarInRegister(element->GetRootOfExpr(), exprRoot);
		ss << "sw " << GetOffset(element) << "(r" << m_topOfStackRegister << "), r" << exprRoot << "\n";
		m_registerStack.push_front(exprRoot);
	}
	else if (dynamic_cast<TempVarNodeBase*>(element->GetRootOfExpr()) != nullptr)
	{
		SymbolTable* table = element->GetSymbolTable();
		SymbolTableEntry* dest = table->FindEntryInTable(element->GetTempVarName());
		SymbolTableEntry* data = table->FindEntryInTable(((TempVarNodeBase*)element->GetRootOfExpr())
			->GetTempVarName());
		ss << CopyData(data, dest);
	}
	else if (dynamic_cast<LiteralNode*>(element->GetRootOfExpr()) != nullptr)
	{
		LiteralNode* floatLiteral = (LiteralNode*)element->GetRootOfExpr();
		ss << LoadFloatToAddr(GetOffset(element), floatLiteral);
	}
	else if (dynamic_cast<VariableNode*>(element->GetRootOfExpr()) != nullptr)
	{
		VariableNode* var = (VariableNode*)element->GetRootOfExpr();
		SymbolTable* table = element->GetSymbolTable();
		SymbolTableEntry* dest = table->FindEntryInTable(element->GetTempVarName());
		SymbolTableEntry* data = table->FindEntryInScope(var->GetVariable()->GetID().GetLexeme());
		ss << CopyData(data, dest);
	}
	else
	{
		DEBUG_BREAK();
	}
	GetCurrStatBlock(element) += ss.str();
}

void CodeGenerator::Visit(ModifiedExpr* element)
{
	GetCurrStatBlock(element) += GenerateModifiedExpr(element);
}

void CodeGenerator::Visit(BaseBinaryOperator* element)
{
	GetCurrStatBlock(element) += GenerateBinaryOp(element);
}

void CodeGenerator::Visit(FuncCallNode* element)
{
	if (dynamic_cast<StatBlockNode*>(element->GetParent()) != nullptr)
	{
		GetCurrStatBlock(element) += CallFunc(element);
	}
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
		TagTableEntry* funcEntry = (TagTableEntry*)element->GetSymbolTable()->GetParentTable()
			->FindEntryInTable(element->GetID()->GetID().GetLexeme());
		m_executableCode += GenerateFunc(funcEntry, funcStatBlock);
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
	size_t exprSize = ComputeSize(element->GetRight()->GetEvaluatedType());

	ss << "\n%assignment\n";
	if (exprSize <= PlatformSpecifications::GetAddressSize())
	{
		RegisterID right;
		ss << ComputeVal(element->GetRight(), right);
		ASSERT(right != NullRegister);

		ss << "sw " << GetOffset(element) << "(r" << m_topOfStackRegister << "), r" << right << "\n";

		m_registerStack.push_front(right);
	}
	else if (dynamic_cast<FuncCallNode*>(element->GetRight()->GetRootOfExpr()) != nullptr)
	{
		FuncCallNode* function = (FuncCallNode*)element->GetRight()->GetRootOfExpr();
		SymbolTable* table = element->GetSymbolTable();
		SymbolTableEntry* dest = table->FindEntryInScope(((VariableNode*)element->GetLeft())
			->GetVariable()->GetID().GetLexeme());

		ss << CallFunc(function);
		ss << CopyData(GetOffset(function), dest->GetSize(), dest->GetOffset());
	}
	else if (dynamic_cast<TempVarNodeBase*>(element->GetRight()->GetRootOfExpr()) != nullptr)
	{
		SymbolTable* table = element->GetSymbolTable();
		SymbolTableEntry* dest = table->FindEntryInScope(((VariableNode*)element->GetLeft())
			->GetVariable()->GetID().GetLexeme());

		SymbolTableEntry* data = table->FindEntryInTable(((TempVarNodeBase*)element->GetRight())->GetTempVarName());
		ss << CopyData(data, dest);
	}
	else if (dynamic_cast<LiteralNode*>(element->GetRight()->GetRootOfExpr()) != nullptr)
	{
		LiteralNode* floatLiteral = (LiteralNode*)element->GetRight()->GetRootOfExpr();
		int offset = GetOffset(element);
		ss << LoadFloatToAddr(offset, floatLiteral);
	}
	GetCurrStatBlock(element) += ss.str();
}

void CodeGenerator::Visit(WriteStatNode* element)
{
	std::stringstream ss;
	size_t currFrameSize = GetCurrFrameSize(element);
	ss << "\n%write stat\n";
	if (element->GetExpr()->GetEvaluatedType() == "integer")
	{
		RegisterID exprValRegister;
		ss << LoadTempVarInRegister(element->GetExpr(), exprValRegister);
		ASSERT(exprValRegister != NullRegister);
		ss << WriteNum(exprValRegister, currFrameSize);
		m_registerStack.push_front(exprValRegister);
	}
	else if (element->GetExpr()->GetEvaluatedType() == "float")
	{
		RegisterID exprValRegister = m_registerStack.front();
		int offset = GetOffset(element->GetExpr());
		
		// write mantissa
		ss << "lw r" << exprValRegister << ", " << offset << "(r" << m_topOfStackRegister << ")\n";
		ss << WriteNum(exprValRegister, currFrameSize);
		offset -= (int)PlatformSpecifications::GetAddressSize();

		//write "*10^"
		constexpr const char constStr[] = "*10^";
		constexpr size_t constStrLen = sizeof(constStr)/sizeof(char);

		for (size_t i = 0; i < constStrLen; i++)
		{
			ss << "addi r" << exprValRegister << ", r" << m_zeroRegister 
				<< ", " << (int)constStr[i] << "\n";
			ss << "putc r" << exprValRegister << "\n";
		}

		// write exponent
		ss << "lw r" << exprValRegister << ", " << offset << "(r" << m_topOfStackRegister << ")\n";
		ss << WriteNum(exprValRegister, currFrameSize);
	}
	else
	{
		DEBUG_BREAK(); // type not supported
	}
	GetCurrStatBlock(element) += ss.str();
}

void CodeGenerator::Visit(ReturnStatNode* element)
{
	std::stringstream ss;
	SymbolTable* table = element->GetSymbolTable();
	SymbolTableEntry* exprEntry = table->FindEntryInTable(element->GetExpr()->GetTempVarName());
	SymbolTableEntry* returnValEntry = table->FindEntryInTable("returnValue");
	ss << CopyData(exprEntry, returnValEntry);
	GetCurrStatBlock(element) += ss.str();
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
		ASSERT(modExpr->GetEvaluatedType() == "integer"); // the not operator is only defined for integers
		return GenerateNotInt(modExpr);
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
	ss << "\n% minus expr\n";
	if (modExpr->GetExpr()->GetEvaluatedType() == "integer")
	{
		RegisterID exprReg;
		ss << ComputeVal(modExpr->GetExpr(), exprReg);
		RegisterID tempVarReg = m_registerStack.front();
		ss << "muli r" << tempVarReg << ", r" << exprReg << ", -1\n";
		ss << "sw " << GetOffset(modExpr) << "(r" << m_topOfStackRegister << "), r" << tempVarReg << "\n";
		m_registerStack.push_front(exprReg);
	}
	else if (modExpr->GetExpr()->GetEvaluatedType() == "float")
	{
		RegisterID reg = m_registerStack.front();
		m_registerStack.pop_front();
		int offset = GetOffset(modExpr);
		ss << LoadFloatToAddr(offset, modExpr->GetExpr());
		ss << "lw r" << reg << ", " << offset << "(r" << m_topOfStackRegister << ")\n";
		ss << "muli r" << reg << ", r" << reg << ", -1\n";
		ss << "sw " << offset << "(r" << m_topOfStackRegister << "), r" << reg << "\n";
	}
	else
	{
		DEBUG_BREAK(); // type not supported
	}

	return ss.str();
}

std::string CodeGenerator::GenerateBinaryOp(BaseBinaryOperator* opNode)
{
	std::string type = opNode->GetEvaluatedType();
	if (type == "integer")
	{
		return GenerateBinaryOpInt(opNode);
	}
	else
	{
		ASSERT(type == "float");
		return GenerateBinaryOpFloat(opNode);
	}
}

std::string CodeGenerator::GenerateBinaryOpInt(BaseBinaryOperator* opNode)
{
	const std::string& op = opNode->GetOperator()->GetOperator().GetLexeme();
	if (op == "+")
	{
		return GenerateAddOpInt(opNode);
	}
	else if (op == "-")
	{
		return GenerateSubOpInt(opNode);
	}
	else if (op == "*")
	{
		return GenerateMultOpInt(opNode);
	}
	else if (op == "/")
	{
		return GenerateDivOpInt(opNode);
	}
	else if (op == "or")
	{
		return GenerateOrInt(opNode);
	}
	else if (op == "and")
	{
		return GenerateAndInt(opNode);
	}
	else if (op == "==")
	{
		return GenerateEqualInt(opNode);
	}
	else if (op == "<>")
	{
		return GenerateNotEqualInt(opNode);
	}
	else if (op == "<")
	{
		return GenerateLessThanInt(opNode);
	}
	else if (op == ">")
	{
		return GenerateGreaterThanInt(opNode);
	}
	else if (op == "<=")
	{
		return GenerateLessOrEqualInt(opNode);
	}
	else if (op == ">=")
	{
		return GenerateGreaterOrEqualInt(opNode);
	}
	else
	{
		// unknown operator
		DEBUG_BREAK();
	}
	return "";
}

std::string CodeGenerator::GenerateBinaryOpFloat(BaseBinaryOperator* opNode)
{
	const std::string& op = opNode->GetOperator()->GetOperator().GetLexeme();
	if (op == "+")
	{
		return GenerateAddOpFloat(opNode);
	}
	else if (op == "-")
	{
		return GenerateSubOpFloat(opNode);
	}
	else if (op == "*")
	{
		return GenerateMultOpFloat(opNode);
	}
	else if (op == "/")
	{
		return GenerateDivOpFloat(opNode);
	}
	else
	{
		// operator not supported for floats
		DEBUG_BREAK();
	}
	return "";
}

std::string CodeGenerator::GenerateAddOpInt(BaseBinaryOperator* opNode)
{
	return GenerateArithmeticOpInt(opNode, "add");
}

std::string CodeGenerator::GenerateSubOpInt(BaseBinaryOperator* opNode)
{
	return GenerateArithmeticOpInt(opNode, "sub");
}

std::string CodeGenerator::GenerateMultOpInt(BaseBinaryOperator* opNode)
{
	return GenerateArithmeticOpInt(opNode, "mul");
}

std::string CodeGenerator::GenerateDivOpInt(BaseBinaryOperator* opNode)
{
	return GenerateArithmeticOpInt(opNode, "div");
}

std::string CodeGenerator::GenerateOrInt(BaseBinaryOperator* opNode)
{
	return "\n%or operator\n" + GenerateAndOrInt(opNode, "add");
}

std::string CodeGenerator::GenerateAndInt(BaseBinaryOperator* opNode)
{
	return "\n%and operator\n" + GenerateAndOrInt(opNode, "mul");
}

std::string CodeGenerator::GenerateNotInt(ModifiedExpr* expr)
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

std::string CodeGenerator::GenerateEqualInt(BaseBinaryOperator* opNode)
{
	return "\n%equals\n" + GenerateRelOpInt(opNode, "ceq");
}

std::string CodeGenerator::GenerateNotEqualInt(BaseBinaryOperator* opNode)
{
	return "\n%not equals\n" + GenerateRelOpInt(opNode, "cne");
}

std::string CodeGenerator::GenerateLessThanInt(BaseBinaryOperator* opNode)
{
	return "\n%less than\n" + GenerateRelOpInt(opNode, "clt");
}

std::string CodeGenerator::GenerateGreaterThanInt(BaseBinaryOperator* opNode)
{
	return "\n%greater than\n" + GenerateRelOpInt(opNode, "cgt");
}

std::string CodeGenerator::GenerateLessOrEqualInt(BaseBinaryOperator* opNode)
{
	return "\n%less or equal\n" + GenerateRelOpInt(opNode, "cle");
}

std::string CodeGenerator::GenerateGreaterOrEqualInt(BaseBinaryOperator* opNode)
{
	return "\n%greater or equal\n" + GenerateRelOpInt(opNode, "cge");
}

std::string CodeGenerator::GenerateArithmeticOpInt(BaseBinaryOperator* opNode, const char* commandName)
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

std::string CodeGenerator::GenerateAndOrInt(BaseBinaryOperator* opNode, const char* commandName)
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

std::string CodeGenerator::GenerateRelOpInt(BaseBinaryOperator* opNode, const char* commandName)
{
	std::stringstream ss;
	RegisterID left;
	ss << ComputeVal(opNode->GetLeft(), left);
	RegisterID right;
	ss << ComputeVal(opNode->GetRight(), right);
	RegisterID result = m_registerStack.front();

	ss << commandName << " r" << result << ", r" << left << ", r" << right << "\n";
	ss << "sw " << GetOffset(opNode) << "(r" << m_topOfStackRegister << "), r" << result << "\n";

	m_registerStack.push_front(right);
	m_registerStack.push_front(left);

	return ss.str();
}

std::string CodeGenerator::GenerateAddOpFloat(BaseBinaryOperator* opNode)
{
	return "\n% adding floats\n" + GenerateAddSubFloat(opNode, "add");
}

std::string CodeGenerator::GenerateSubOpFloat(BaseBinaryOperator* opNode)
{
	return "\n% subtracting floats\n" + GenerateAddSubFloat(opNode, "sub");
}

std::string CodeGenerator::GenerateMultOpFloat(BaseBinaryOperator* opNode)
{
	DEBUG_BREAK();
	return "";
}

std::string CodeGenerator::GenerateDivOpFloat(BaseBinaryOperator* opNode)
{
	DEBUG_BREAK();
	return "";
}

std::string CodeGenerator::GenerateAddSubFloat(BaseBinaryOperator* opNode, const char* commandName)
{
	size_t tempStartNumRegisters = m_registerStack.size();

	std::stringstream ss;
	int destOffset = GetOffset(opNode);
	int leftOffset = GetOffset(opNode->GetLeft());
	int rightOffset = GetOffset(opNode->GetRight());

	RegisterID exponentLeft = m_registerStack.front();
	m_registerStack.pop_front();
	RegisterID exponentRight = m_registerStack.front();
	m_registerStack.pop_front();
	RegisterID exponentDiff = m_registerStack.front();
	m_registerStack.pop_front();
	
	RegisterID smallerFloatMantissa = m_registerStack.front();
	m_registerStack.pop_front();

	RegisterID biggerFloatMantissa = m_registerStack.front();
	m_registerStack.pop_front();

	{
		RegisterID lessThanResult = m_registerStack.front();


		ss << "\n% find smallest exponent\n";
		ss << "lw r" << exponentLeft << ", " << (leftOffset - (int)PlatformSpecifications::GetAddressSize())
			<< "(r" << m_topOfStackRegister << ")\n";
		ss << "lw r" << exponentRight << ", " << (rightOffset - (int)PlatformSpecifications::GetAddressSize())
			<< "(r" << m_topOfStackRegister << ")\n";

		std::string isNotLessThan = m_tagGen.GetNextTag();
		std::string endNotLessThan = m_tagGen.GetNextTag();

		ss << "clt r" << lessThanResult << ", r" << exponentLeft << ", r" << exponentRight << "\n";
		ss << "bz r" << lessThanResult << ", " << isNotLessThan << "\n";


		// if left exponent is bigger than or equal to right exponent
		ss << "sub r" << exponentDiff << ", r" << exponentLeft << ", r" << exponentRight << "\n";
		ss << "lw r" << biggerFloatMantissa << ", " << leftOffset << "(r" << m_topOfStackRegister << ")\n";
		ss << "lw r" << smallerFloatMantissa << ", " << rightOffset << "(r" << m_topOfStackRegister << ")\n";
		ss << "sw " << (destOffset - (int)PlatformSpecifications::GetAddressSize())
			<< "(r" << m_topOfStackRegister << "), r" << exponentRight << "\n";
		if (strcmp(commandName, "sub") == 0)
		{
			ss << "muli r" << smallerFloatMantissa << ", r" << smallerFloatMantissa << ", -1\n";
		}
		ss << "j " << endNotLessThan << "\n";

		ss << isNotLessThan << " sub r" << exponentDiff << ", r" << exponentRight << ", r" << exponentLeft << "\n";
		ss << "lw r" << biggerFloatMantissa << ", " << rightOffset << "(r" << m_topOfStackRegister << ")\n";
		ss << "lw r" << smallerFloatMantissa << ", " << leftOffset << "(r" << m_topOfStackRegister << ")\n";
		ss << "sw " << (destOffset - (int)PlatformSpecifications::GetAddressSize())
			<< "(r" << m_topOfStackRegister << "), r" << exponentLeft << "\n";
		if (strcmp(commandName, "sub") == 0)
		{
			ss << "muli r" << biggerFloatMantissa << ", r" << biggerFloatMantissa << ", -1\n";
		}
		ss << endNotLessThan << " ";
	}

	
	RegisterID forLoopCounter = m_registerStack.front();
	m_registerStack.pop_front();

	RegisterID forLoopConditionResult = m_registerStack.front();
	m_registerStack.pop_front();

	RegisterID operationResultMantissa = m_registerStack.front();
	m_registerStack.pop_front();


	// adjust smaller float mantissa
	std::string startLoop = m_tagGen.GetNextTag();
	std::string afterEndLoop = m_tagGen.GetNextTag();

	ss << "\n% adjust mantissa\n";
	ss << " addi r" << forLoopCounter << ", r" << m_zeroRegister << ", 0\n";
	ss << startLoop << " clt r" << forLoopConditionResult << ", r" << forLoopCounter << ", r" << exponentDiff << "\n";
	ss << "bz r" << forLoopConditionResult << ", " << afterEndLoop << "\n";
	ss << "muli r" << smallerFloatMantissa << ", r" << smallerFloatMantissa << ", 10\n"; // divide by 10
	ss << "addi r" << forLoopCounter << ", r" << forLoopCounter << ", 1\n";
	ss << "j " << startLoop << "\n";
	ss << afterEndLoop << " ";
	

	// adjust mantissa to use correct dot positioning

	// retrieve num digit per mantissa
	RegisterID numDigitsMantissaSmallFloat = exponentLeft;
	RegisterID numDigitsMantissaBigFloat = exponentRight;
	RegisterID numDigitDiff = exponentDiff;
	ss << "\n% adjust mantissa position\n";
	ss << GetNumDigitsInNum(smallerFloatMantissa, numDigitsMantissaSmallFloat);
	ss << GetNumDigitsInNum(biggerFloatMantissa, numDigitsMantissaBigFloat);
	ss << "sub r" << numDigitDiff << ", r" << numDigitsMantissaBigFloat
		<< ", r" << numDigitsMantissaSmallFloat << "\n";

	// make sure difference between num digit is positive
	RegisterID shortestMantissa = numDigitsMantissaSmallFloat;
	RegisterID otherMantissa = numDigitsMantissaBigFloat;

	{
		RegisterID lessThanResult = m_registerStack.front();
		ss << "clt r" << lessThanResult << ", r" << numDigitDiff << ", r" << m_zeroRegister << "\n";
		std::string endIf = m_tagGen.GetNextTag();

		ss << "bz r" << lessThanResult << ", " << endIf << "\n";
		ss << "muli r" << numDigitDiff << ", r" << numDigitDiff << ", -1\n";

		// find mantissa with less digits
		ss << "\n% sort what mantissa to use for operation\n";
		ss << endIf << " clt r" << lessThanResult << ", r" << numDigitsMantissaBigFloat
			<< ", r" << numDigitsMantissaSmallFloat << "\n";

		endIf = m_tagGen.GetNextTag();
		std::string endElse = m_tagGen.GetNextTag();

		ss << "bz r" << lessThanResult << ", " << endIf << "\n";
		ss << "add r" << shortestMantissa << ", r" << m_zeroRegister << ", r" << biggerFloatMantissa << "\n";
		ss << "add r" << otherMantissa << ", r" << m_zeroRegister << ", r" << smallerFloatMantissa << "\n";

		ss << "j " << endElse << "\n";
		ss << endIf << " add r" << shortestMantissa << ", r" << m_zeroRegister << ", r" << smallerFloatMantissa << "\n";
		ss << "add r" << otherMantissa << ", r" << m_zeroRegister << ", r" << biggerFloatMantissa << "\n";

		// multiply shortest mantissa by 10 repeatedly until both mantissa have same number of digits 
		startLoop = m_tagGen.GetNextTag();
		afterEndLoop = m_tagGen.GetNextTag();
		{
			RegisterID currExponent = m_registerStack.front();
			ss << "\n% multiply shortest mantissa to adjust it's position\n";
			ss << "lw r" << currExponent << ", "
				<< (destOffset - (int)PlatformSpecifications::GetAddressSize())
				<< "(r" << m_topOfStackRegister << ")\n";
			ss << endElse << " addi r" << forLoopCounter << ", r" << m_zeroRegister << ", 0\n";

			ss << startLoop << " clt r" << forLoopConditionResult << ", r"
				<< forLoopCounter << ", r" << numDigitDiff << "\n";

			ss << "bz r" << forLoopConditionResult << ", " << afterEndLoop << "\n";
			ss << "muli r" << shortestMantissa << ", r" << shortestMantissa << ", 10\n"; // multiply by 10
			ss << "addi r" << forLoopCounter << ", r" << forLoopCounter << ", 1\n";
			ss << "subi r" << currExponent << ", r" << currExponent << ", 1\n";
			ss << "j " << startLoop << "\n";
			ss << afterEndLoop << " sw " << (destOffset - (int)PlatformSpecifications::GetAddressSize())
				<< "(r" << m_topOfStackRegister << "), r" << currExponent << "\n";
		}
	}

	// do operation
	ss << "\n% perform operation\n";
	constexpr size_t underflowShift = 100;

	ss << "muli r" << shortestMantissa << ", r" << shortestMantissa << ", " << underflowShift << "\n";
	ss << "muli r" << otherMantissa << ", r" << otherMantissa << ", " << underflowShift << "\n";
	ss << "add r" << operationResultMantissa << ", r"
		<< shortestMantissa << ", r" << otherMantissa << "\n";

	//compact result
	RegisterID modResult = m_registerStack.front();
	m_registerStack.pop_front();
	std::string startCompactLoop = m_tagGen.GetNextTag();
	std::string afterEndCompactLoop = m_tagGen.GetNextTag();
	RegisterID exponentOffset = shortestMantissa;

	ss << "\n% compact result\n";
	ss << "addi r" << exponentOffset << ", r" << m_zeroRegister << ", 2\n";
	ss << startCompactLoop << " modi r" << modResult << ", r" << operationResultMantissa << ", 10\n";
	ss << "bnz r" << modResult << ", " << afterEndCompactLoop << "\n";
	ss << "bz r" << operationResultMantissa << ", " << afterEndCompactLoop << "\n";
	ss << "divi r" << operationResultMantissa << ", r" << operationResultMantissa << ", 10\n"; // divide by 10
	ss << "subi r" << exponentOffset << ", r" << exponentOffset << ", 1\n";
	ss << "j " << startCompactLoop << "\n";

	std::string endIfExponent = m_tagGen.GetNextTag();

	ss << "\n% compact exponent if mantissa is 0\n";
	ss << afterEndCompactLoop << " bnz r" << operationResultMantissa << ", " << endIfExponent << "\n";
	ss << "lw r" << exponentOffset << ", " << GetOffset(opNode) << "(r" << m_topOfStackRegister << ")\n";
	ss << "muli r" << exponentOffset << ", r" << exponentOffset << ", -1\n";


	// store result
	ss << "\n% store the result\n";
	ss << endIfExponent << " sw " << destOffset << "(r" << m_topOfStackRegister << "), r"
		<< operationResultMantissa << "\n";
	RegisterID operationResultExponent = operationResultMantissa;
	ss << "lw r" << operationResultExponent << ", "
		<< (destOffset - (int)PlatformSpecifications::GetAddressSize()) << "(r" << m_topOfStackRegister << ")\n";
	ss << "add r" << operationResultExponent << ", r" << operationResultExponent << ", r" << exponentOffset << "\n";
	ss << "sw " << (destOffset - (int)PlatformSpecifications::GetAddressSize())
		<< "(r" << m_topOfStackRegister << "), r" << operationResultExponent << "\n";

	m_registerStack.push_front(numDigitsMantissaBigFloat);
	m_registerStack.push_front(numDigitsMantissaSmallFloat);
	m_registerStack.push_front(modResult);
	m_registerStack.push_front(operationResultMantissa);
	m_registerStack.push_front(forLoopConditionResult);
	m_registerStack.push_front(forLoopCounter);
	m_registerStack.push_front(biggerFloatMantissa);
	m_registerStack.push_front(smallerFloatMantissa);
	m_registerStack.push_front(exponentDiff);
	m_registerStack.push_front(exponentLeft);
	m_registerStack.push_front(exponentRight);


	ASSERT(m_registerStack.size() == tempStartNumRegisters);

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
	else if (dynamic_cast<FuncCallNode*>(node) != nullptr)
	{
		return ComputeVal((FuncCallNode*)node, outRegister);
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

std::string CodeGenerator::ComputeVal(FuncCallNode* node, RegisterID& outRegister)
{
	std::stringstream ss;
	ss << CallFunc(node);
	outRegister = m_registerStack.front();
	m_registerStack.pop_front();
	ss << "lw r" << outRegister << ", " << GetOffset(node) << "(r" << m_topOfStackRegister << ")\n";

	return ss.str();
}

std::string CodeGenerator::ComputeVal(ExprNode* node, RegisterID& outRegister)
{
	return ComputeVal(node->GetRootOfExpr(), outRegister);
}

std::string CodeGenerator::ComputeVal(ModifiedExpr* node, RegisterID& outRegister)
{
	std::stringstream ss;
	ss << GenerateModifiedExpr(node);
	ss << LoadTempVarInRegister(node, outRegister);
	return ss.str();
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
	else if (dynamic_cast<RefVarNode*>(n) != nullptr)
	{
		return LoadVarInRegister((RefVarNode*)n, outRegister);
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

std::string CodeGenerator::LoadVarInRegister(RefVarNode* node, RegisterID& outRegister)
{
	std::stringstream ss;
	outRegister = m_registerStack.front();
	m_registerStack.pop_front();
	RegisterID tempAddr = m_registerStack.front();
	ss << "lw r" << tempAddr << ", " << GetOffset(node) << "(r" << m_topOfStackRegister << ")\n";
	ss << "lw r" << outRegister << ", 0(r" << tempAddr << ")\n";
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

std::string CodeGenerator::LoadFloatToAddr(int offset, ASTNode* expr)
{
	if (dynamic_cast<LiteralNode*>(expr) != nullptr)
	{
		return LoadFloatToAddr(offset, (LiteralNode*)expr);
	}
	else if (dynamic_cast<VariableNode*>(expr) != nullptr)
	{
		return LoadFloatToAddr(offset, (VariableNode*)expr);
	}
	else
	{
		DEBUG_BREAK(); // node type not supported
	}
	return "";
}

std::string CodeGenerator::LoadFloatToAddr(int offset, ExprNode* expr)
{
	return LoadFloatToAddr(offset, expr->GetRootOfExpr());
}

std::string CodeGenerator::LoadFloatToAddr(int offset, VariableNode* var)
{
	ASSERT(var->GetEvaluatedType() == "float");
	std::stringstream ss;
	RegisterID reg = m_registerStack.front();
	SymbolTableEntry* varEntry = var->GetSymbolTable()->FindEntryInScope(var->GetVariable()->GetID().GetLexeme());
	ss << CopyData(varEntry->GetOffset(), varEntry->GetSize(), offset);
	return ss.str();
}

std::string CodeGenerator::LoadFloatToAddr(int offset, LiteralNode* floatLiteral)
{
	ASSERT(floatLiteral->GetEvaluatedType() == "float");
	std::stringstream ss;
	RegisterID reg = m_registerStack.front();
	std::string mantissa;
	std::string exponent;
	FloatToRepresentationStr(floatLiteral->GetLexemeNode()->GetID().GetLexeme(), mantissa, exponent);
	
	ss << "addi r" << reg << ", r" << m_zeroRegister << ", " << mantissa << "\n";
	ss << "sw " << offset << "(r" << m_topOfStackRegister << "), r" << reg << "\n";
	offset -= (int)PlatformSpecifications::GetAddressSize();
	ss << "addi r" << reg << ", r" << m_zeroRegister << ", " << exponent << "\n";
	ss << "sw " << offset << "(r" << m_topOfStackRegister << "), r" << reg << "\n";

	return ss.str();
}

std::string CodeGenerator::GenerateFunc(TagTableEntry* funcEntry, const std::string& statBlock)
{
	std::stringstream ss;

	SymbolTable* funcTable = funcEntry->GetSubTable();
	SymbolTableEntry* returnAddressEntry = funcTable->FindEntryInTable("returnAddress");
	
	ss << "\n% start of func \"" << funcEntry->GetName() << "\"\n";
	ss << funcEntry->GetTag() << " sw " << returnAddressEntry->GetOffset() << "(r" 
		<< m_topOfStackRegister << "), r" << m_jumpReturnRegister << "\n";

	ss << statBlock;

	if (funcEntry->GetEvaluatedType() != "void")
	{
		SymbolTableEntry* returnValEntry = funcTable->FindEntryInTable("returnValue");
		ss << "addi r" << m_returnValRegister << ", r" << m_topOfStackRegister 
			<< ", " << returnValEntry->GetOffset() << "\n";
	}

	ss << "lw r" << m_jumpReturnRegister << ", " 
		<< returnAddressEntry->GetOffset() << "(r" << m_topOfStackRegister << ")\n";
	ss << "jr r" << m_jumpReturnRegister << "\n";
	ss << "% end of func \"" << funcEntry->GetName() << "\"\n";
	return ss.str();
}

std::string CodeGenerator::WriteNum(RegisterID numRegister, size_t frameSize)
{
	std::stringstream ss;

	ss << IncrementStackFrame(frameSize);
	ss << "sw " << PlatformSpecifications::GetIntStrArg1Offset() << "(r" << m_topOfStackRegister
		<< "), r" << numRegister << "\n";

	ss << "addi r" << numRegister << ", r" << m_zeroRegister << ", buff\n";
	ss << "sw " << PlatformSpecifications::GetIntStrArg2Offset() << "(r" << m_topOfStackRegister
		<< "), r" << numRegister << "\n";

	ss << "jl r" << m_jumpReturnRegister << ", intstr\n";

	ss << "sw " << PlatformSpecifications::GetPutStrArg1Offset() << "(r" << m_topOfStackRegister
		<< "), r" << m_returnValRegister << "\n";

	ss << "jl r" << m_jumpReturnRegister << ", putstr\n";
	ss << DecrementStackFrame(frameSize);
	return ss.str();
}

std::string CodeGenerator::CopyData(int dataOffset, size_t dataSize, int destOffset)
{
	std::stringstream ss;
	RegisterID reg = m_registerStack.front();

	for (size_t i = 0; i < dataSize; i += PlatformSpecifications::GetAddressSize())
	{
		ss << "lw r" << reg << ", " << (dataOffset - (int)i) << "(r" << m_topOfStackRegister << ")\n";
		ss << "sw " << (destOffset - (int)i) << "(r" << m_topOfStackRegister << "), r" << reg << "\n";
	}
	return ss.str();
}

std::string CodeGenerator::CopyData(SymbolTableEntry* data, SymbolTableEntry* dest)
{
	return CopyData(data->GetOffset(), data->GetSize(), dest->GetOffset());
}

std::string CodeGenerator::CopyDataAtRef(RegisterID dataAddress, size_t dataSize, int destOffset)
{
	std::stringstream ss;
	RegisterID reg = m_registerStack.front();
	ss << "\n% copy data at reference\n";
	for (size_t i = 0; i < dataSize; i += PlatformSpecifications::GetAddressSize())
	{
		ss << "lw r" << reg << ", -" << i << "(r" << dataAddress << ")\n";
		ss << "sw " << (destOffset - (int)i) << "(r" << m_topOfStackRegister << "), r" << reg << "\n";
	}
	return ss.str();
}

std::string CodeGenerator::GetNumDigitsInNum(RegisterID num, RegisterID& outNumDigits)
{
	std::stringstream ss;
	std::string startLoop = m_tagGen.GetNextTag();
	std::string endLoop = m_tagGen.GetNextTag();

	outNumDigits = m_registerStack.front();
	m_registerStack.pop_front();

	RegisterID numCopy = m_registerStack.front();

	ss << "\n% count num digits\n";
	ss << "add r" << numCopy << ", r" << m_zeroRegister << ", r" << num << "\n";
	ss << "addi r" << outNumDigits << ", r" << m_zeroRegister << ", 1\n";
	
	// divide once out of loop to keep in sync the numDigits
	ss << "divi r" << numCopy << ", r" << numCopy << ", 10\n"; 
	
	ss << startLoop << " bz r" << numCopy << ", " << endLoop << "\n";
	ss << "divi r" << numCopy << ", r" << numCopy << ", 10\n";
	ss << "addi r" << outNumDigits << ", r" << outNumDigits << ", 1\n";
	ss << "j " << startLoop << "\n\n";
	ss << endLoop << " ";
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

size_t CodeGenerator::GetCurrFrameSize(ASTNode* node)
{
	return node->GetSymbolTable()->GetParentEntry()->GetSize();
}

std::string CodeGenerator::CallFunc(FuncCallNode* funcCall)
{
	std::stringstream ss;
	TagTableEntry* funcEntry = (TagTableEntry*)funcCall->GetSymbolTable()
		->FindEntryInScope(funcCall->GetID()->GetID().GetLexeme());

	size_t currFrameSize = GetCurrFrameSize(funcCall);

	ss << IncrementStackFrame(currFrameSize);

	// pass parameters here

	ss << "jl r" << m_jumpReturnRegister << ", " << funcEntry->GetTag();
	ss << DecrementStackFrame(currFrameSize);

	// handle return value
	if (funcEntry->GetEvaluatedType() != "void")
	{
		size_t dataSize = ComputeSize(funcCall->GetEvaluatedType());
		if (dataSize == InvalidSize)
		{
			dataSize = FindSize(GetGlobalTable(funcCall->GetSymbolTable()), funcCall->GetEvaluatedType());
		}
		ASSERT(dataSize != InvalidSize);

		SymbolTableEntry* returnValEntry = funcEntry->GetSubTable()->FindEntryInTable("returnValue");
		ss << "\n% handle return value\n";

		ss << CopyDataAtRef(m_returnValRegister, dataSize, GetOffset(funcCall)) << "\n";
	}

	return ss.str();
}