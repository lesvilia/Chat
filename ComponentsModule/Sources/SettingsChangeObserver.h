#pragma once

namespace sm
{
  enum ChangeType
  {
    NOT_CHANGING,
    NET_SETTINGS = 2,
    UI_SETTINGS = 4
  };

  class SettingsChangeObserver
  {
  public:
    virtual void SettingsWillBeChanged(int type) = 0;
    virtual void SettingsChanged(int type) = 0;
  protected:
    ~SettingsChangeObserver() { }
  };
}
