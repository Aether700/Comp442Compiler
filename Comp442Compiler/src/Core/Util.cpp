#include "Util.h"
#include "../Lexer/Lexer.h"

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