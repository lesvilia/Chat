#include "DnDTextEdit.h"

#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QList>
#include <QMimeData>
#include <QUrl>

#include "IDropResultHandler.h"

namespace ui
{
  namespace controls
  {
    DnDTextEdit::DnDTextEdit(IDropResultHandler* handler)
      : m_handler(handler)
    {
      setAcceptDrops(true);
    }

    void DnDTextEdit::dragEnterEvent(QDragEnterEvent* event)
    {
      if (event->mimeData()->hasUrls())
      {
        event->acceptProposedAction();
      }
    }

    void DnDTextEdit::dropEvent(QDropEvent* event)
    {
      QList<QUrl> urls(event->mimeData()->urls());
      QString filePath(QDir::toNativeSeparators(urls.begin()->toLocalFile())); //only one files
      if (QFile::exists(filePath))
      {
        m_handler->HandleDropFileResult(filePath.toStdWString());
      }
    }
  }
}
