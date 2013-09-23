#include "ProgressHandler.h"

#include <QMessageBox>

namespace ui
{
  namespace controls
  {
    ProgressUIHandler::ProgressUIHandler()
    {
      connect(this, SIGNAL(IncrementProgress(int)), this, SLOT(setValue(int)));
      connect(this, SIGNAL(TransferFinished()), this, SLOT(HideProgressBar()));
      connect(this, SIGNAL(TransferError()), this, SLOT(ShowMessageBox()));
    }

    ProgressUIHandler::~ProgressUIHandler()
    {
    }

    void ProgressUIHandler::UpdateProgress(int count)
    {
      emit IncrementProgress(count);
    }

    void ProgressUIHandler::OnFinished()
    {
      emit TransferFinished();
    }

     void ProgressUIHandler::OnError()
     {
       emit TransferError();
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
