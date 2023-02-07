#include "Util.h"

std::string SimplifyFilename(const std::string& filepath)
{
	size_t lastExtention = filepath.find_last_of(".");
	return filepath.substr(0, lastExtention);
}