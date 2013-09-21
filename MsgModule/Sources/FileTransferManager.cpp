#include "stdafx.h"
#include "FileTransferManager.h"
#include "FileServerSettingsHolder.h"
#include "FileMessagesHandler.h"
#include "IProgressUIObserver.h"
#include "MessagesServer.h"
#include "SettingsManager.h"
#include "TCPMessagesServer.h"

namespace msg
{
  FileTransferManager* FileTransferManager::Instance()
  {
    static FileTransferManager manager;
    return &manager;
  }

  FileTransferManager::FileTransferManager()
    : m_settingsHolder(new FileServerSettingsHolder(sm::SettingsManager::Instance()))
    , m_activated(false)
  {

  }

  FileTransferManager::~FileTransferManager()
  {
  }

  void FileTransferManager::SendFile(const std::wstring& filePath, ui::IProgressUIObserver* observer)
  {

  }

  void FileTransferManager::OnlineStateChanged()
  {
  }

  void FileTransferManager::Activate(UIMessageHandler* handler)
  {
    if (!m_activated)
    {
      m_msgHandler.reset(new FileMessagesHandler(handler));
      m_server.reset(new TCPMessageServer(m_msgHandler.get(), m_settingsHolder.get()));
      m_activated = true;
    }
  }

  void FileTransferManager::Deactivate()
  {
    m_server->Shutdown();
    m_activated = false;
  }

  void FileTransferManager::ResetServer()
  {
    m_server->Reset();
  }
}