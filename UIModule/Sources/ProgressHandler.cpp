#include "ProgressHandler.h"

#include <QCoreApplication>
#include <QMessageBox>
#include "ErrorProgressEvent.h"
#include "FinishedProgressEvent.h"
#include "UpdateProgressEvent.h"


namespace ui
{
  namespace controls
  {
    ProgressUIHandler::ProgressUIHandler()
    {
    }

    ProgressUIHandler::~ProgressUIHandler()
    {
    }

    void ProgressUIHandler::UpdateProgress(int count)
    {
      QCoreApplication::postEvent(this, new UpdateProgressEvent(count));
    }

    void ProgressUIHandler::OnFinished()
    {
      QCoreApplication::postEvent(this, new FinishedProgressEvent());
    }

    void ProgressUIHandler::OnError()
    {
      QCoreApplication::postEvent(this, new ErrorProgressEvent());
    }

    bool ProgressUIHandler::event(QEvent* ev)
    {
      if (ev->type() == UpdateProgressEvent::type)
      {
        UpdateProgressEvent* event = static_cast<UpdateProgressEvent*>(ev);
        setValue(event->m_value);
        return true;
      }

      if (ev->type() == FinishedProgressEvent::type)
      {
        HideProgressBar();
        return true;
      }

      if (ev->type() == ErrorProgressEvent::type)
      {
        ShowMessageBox();
        return true;
      }

      return QWidget::event(ev);
    }

    void ProgressUIHandler::HideProgressBar()
    {
      hide();
    }

    void ProgressUIHandler::ShowMessageBox()
    {
      QMessageBox messageBox(this);
      messageBox.setWindowTitle("Error transferring file.");
      messageBox.setText("Error transferring file");
      messageBox.setMaximumHeight(150);
      messageBox.setMaximumWidth(200);
      messageBox.exec();
    }
  }
}
