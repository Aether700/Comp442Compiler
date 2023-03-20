#include "CodeGeneration.h"
#include "../Parser/AST.h"
#include "../Core/Core.h"

// SizeGenerator ////////////////////////////////////////////////////////////////////////
SizeGenerator::SizeGenerator(SymbolTable* globalTable, size_t intSize, size_t floatSize, size_t boolSize)
	: m_globalTable(globalTable), m_intSize(intSize), m_floatSize(floatSize), m_boolSize(boolSize) { }

void SizeGenerator::Visit(BaseBinaryOperator* element)
{
	std::string typeStr = element->GetEvaluatedType();
	size_t size = ComputeSize(typeStr);
	ASSERT(size != InvalidSize);
	element->GetSymbolTable()->AddEntry(new TempVarEntry(typeStr, size));
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
		baseSize = m_intSize;
	}
	else if (typeStr == "float")
	{
		baseSize = m_floatSize;
	}
	else if (typeStr == "bool")
	{
		baseSize = m_boolSize;
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
		return m_intSize;
	}
	else if (typeStr == "float")
	{
		return m_floatSize;
	}
	else if (typeStr == "bool")
	{
		return m_boolSize;
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
			offset -= entry->GetSize();
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
			offset -= parentClassEntry->GetSize();
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