#pragma once
#include <string>

namespace ui
{
	class UIMessageHandler
	{
	public:
		virtual void AddNewMessage(const std::wstring& uuid, const std::wstring& message) = 0;
	protected:
		~UIMessageHandler(){}
	};
}