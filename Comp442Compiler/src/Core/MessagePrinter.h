#pragma once
#include "Core.h"
#include <list>
#include <string>
#include <utility>

class MessagePrinter
{
public:
	static void AddMessage(size_t line, const std::string& message);

	static void PrintToConsole(const std::string& filepathOfOutput);
	static void ClearMessages();

private:
	MessagePrinter();

	static MessagePrinter& GetInstance();

	std::list<std::pair<size_t, std::string>> m_sortedMessageList;
};