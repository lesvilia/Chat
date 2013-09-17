#pragma once
#include <string>
#include <vector>
#include <memory>
#include "boost/thread/mutex.hpp"

namespace sm
{
  namespace impl
  {
    class AdaptersAddressHolder;
  }

  class SettingsManager
  {
    typedef boost::mutex Mutex;
    typedef boost::unique_lock<Mutex> Lock;

  public:
    static SettingsManager* Instance();
    std::wstring GetCurrentNetAddres() const;
    unsigned short GetCurrentStatesPort() const;
    unsigned short GetCurrentMessagesPort() const;
    void SetCurrentAddress(const std::wstring& addr);
    void SetCurrentStatesPort(unsigned short statePort);
    void SetCurrentMessagesPort(unsigned short chatPort);
    std::vector<std::wstring> GetActiveAddresses() const;

  private:
    SettingsManager();
    ~SettingsManager();
    void Initialize();
    void SetDefaultValue();
    void SaveSettings();

  private:
    std::unique_ptr<impl::AdaptersAddressHolder> m_addressHolder;
    std::wstring m_currentNetAddress;
    unsigned short m_stateMsgPort;
    unsigned short m_chatMsgPort;
    mutable Mutex m_mutex;
  };
}

