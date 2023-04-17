#pragma once
#include <string>


std::string SimplifyFilename(const std::string& filepath);

std::string LeftTrimStr(const std::string& str);
std::string RightTrimStr(const std::string& str);
std::string TrimStr(const std::string& str);

void FloatToRepresentationStr(const std::string& floatStr, std::string& outMantissa, std::string& outExponent);

// generates tags for memory addresses or temporary variables
class TagGenerator
{
public:
    TagGenerator(const std::string& prefix);
    std::string GetNextTag();

private:
    size_t m_counter;
    std::string m_prefix;
};