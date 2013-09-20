#include "ProgressHandler.h"

namespace ui
{
  namespace controls
  {
    ProgressUIHandler::ProgressUIHandler()
    {
      connect(this, SIGNAL(IncrementProgress(int)), this, SLOT(setValue(int)));
      connect(this, SIGNAL(TransferFinished()), this, SLOT(HideProgressBar()));
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

    void ProgressUIHandler::HideProgressBar()
    {
      hide();
    }
  }
}
