#pragma once
#include <memory>
#include <string>
#include "boost/noncopyable.hpp"
#include "FileInfo.h"
#include "LoginHandlers.h"
#include "SettingsChangeObserver.h"

class ACE_Message_Block;

namespace ui
{
  class IProgressUIObserver;
}

namespace msg
{
  class UIMessageHandler;
  class IServerSettingsHolder;
  class IMessagesHandler;
  class MessageServer;
  class ProgressUpdater;

  class FileTransferManager
    : private boost::noncopyable
    , public login::ILoginStateObserver
    , public sm::SettingsChangeObserver
  {
  public:
    static FileTransferManager* Instance();
    void SendFile(const std::wstring& uuid, const std::wstring& filePath,
                  ui::IProgressUIObserver* observer);
    virtual void OnlineStateChanged();
    void Activate(UIMessageHandler* handler);
    void Deactivate();

  private:
    FileInfoPtr GetFileInfo(const std::wstring& filePath);
    virtual void SettingsWillBeChanged(int type);
    virtual void SettingsChanged(int type);
    void ResetServer();
    FileTransferManager();
    ~FileTransferManager();

  private:
    std::unique_ptr<IServerSettingsHolder> m_settingsHolder;
    bool m_activated;
    std::unique_ptr<IMessagesHandler> m_msgHandler;
    std::unique_ptr<MessageServer> m_server;
  };
}
