#pragma once
#include <string>

namespace ui
{
	class UIMessageHandler
	{
	public:
		virtual void AddNewUser(const std::wstring& uuid, const std::wstring& name) = 0;
		virtual void RemoveUser(const std::wstring& uuid) = 0;
		virtual void AddNewMessage(const std::wstring& uuid, const std::wstring& message) = 0;

	protected:
		~UIMessageHandler(){}
	};
}