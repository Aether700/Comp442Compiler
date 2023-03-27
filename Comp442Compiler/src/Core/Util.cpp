#include "Util.h"
#include "../Lexer/Lexer.h"
#include "../CodeGeneration/CodeGeneration.h"
#include "Core.h"

#include <sstream>
#include <bitset>

std::string SimplifyFilename(const std::string& filepath)
{
	size_t lastExtention = filepath.find_last_of(".");
	return filepath.substr(0, lastExtention);
}

std::string LeftTrimStr(const std::string& str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (!CharData::IsWhitespace(str[i]) && str[i] != '\n')
		{
			return str.substr(i);
		}
	}
	return "";
}

std::string RightTrimStr(const std::string& str)
{
	for (size_t i = str.length() - 1; i >= 0; i--)
	{
		if (!CharData::IsWhitespace(str[i]) && str[i] != '\n')
		{
			return str.substr(0, i + 1);
		}
	}
	return "";
}

std::string TrimStr(const std::string& str)
{
	return RightTrimStr(LeftTrimStr(str));
}

void FloatToRepresentationStr(const std::string& floatStr, std::string& outMantissa, std::string& outExponent)
{
	size_t dotPos = floatStr.find(".");
	ASSERT(dotPos != std::string::npos);
	std::string firstPart = floatStr.substr(0, dotPos);
	std::string secondPart = floatStr.substr(dotPos + 1);
	outMantissa = firstPart + secondPart;
	std::stringstream ss;
	ss << firstPart.length();
	outExponent = ss.str();
}


// TagGenerator //////////////////////////////////////////////////
TagGenerator::TagGenerator(const std::string& prefix) : m_counter(0), m_prefix(prefix) { }

std::string TagGenerator::GetNextTag()
{
	std::stringstream ss;
	ss << m_prefix << m_counter;
	m_counter++;
	return ss.str();
}