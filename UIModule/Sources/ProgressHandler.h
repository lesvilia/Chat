#pragma once
#include <QProgressBar>

#include "IProgressUIObserver.h"

namespace ui
{
  namespace controls
  {
    class ProgressUIHandler
      : public QProgressBar
      , public IProgressUIObserver
    {
      Q_OBJECT

    public:
      ProgressUIHandler();
      ~ProgressUIHandler();

      //IProgressUIObserver interface
      virtual void UpdateProgress(int count);
      virtual void OnFinished();
      virtual void OnError();

    private slots:
      void HideProgressBar();

    signals:
      void IncrementProgress(int);
      void TransferFinished();
    };
  }
}
