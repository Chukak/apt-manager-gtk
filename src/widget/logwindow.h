#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include "../type.h"

#include <gtkmm/builder.h>
#include <gtkmm/textview.h>

#include "../utils.h"

#include <sstream>

namespace widget
{
class LogWindow : public Gtk::TextView
{
  public:
    LogWindow(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder);
    virtual ~LogWindow() = default;
    std::ostringstream* log();

  private:
    std::ostringstream _log;
    sigc::connection _timer;
};
} // namespace widget

#endif // LOGWINDOW_H
