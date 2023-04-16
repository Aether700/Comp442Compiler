#include "MessagePrinter.h"
#include <iostream>


void MessagePrinter::AddMessage(size_t line, const std::string& message)
{
	MessagePrinter& printer = GetInstance();
	auto it = printer.m_sortedMessageList.begin();
	for (; it != printer.m_sortedMessageList.end(); it++)
	{
		if ((*it).first > line)
		{
			break;
		}
	}
	printer.m_sortedMessageList.insert(it, std::make_pair(line, message));
}

void MessagePrinter::PrintToConsole()
{
	MessagePrinter& printer = GetInstance();

	for (auto& pair : printer.m_sortedMessageList)
	{
		std::cout << pair.second << "\n";
	}

	if (printer.m_sortedMessageList.size() > 0)
	{
		std::cout << "\n\n";
	}
}

void MessagePrinter::ClearMessages()
{
	GetInstance().m_sortedMessageList.clear();
}

MessagePrinter::MessagePrinter() { }

MessagePrinter& MessagePrinter::GetInstance()
{
	static MessagePrinter printer;
	return printer;
}