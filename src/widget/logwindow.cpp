#include "logwindow.h"
#include <gtkmm/window.h>

namespace widget
{
LogWindow::LogWindow(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder) :
	Gtk::TextView(cobject)
{
	(void)refBuilder;

	set_editable(false);
	set_wrap_mode(Gtk::WRAP_NONE);

	Gtk::Window* t = nullptr;
	utils::GetBuilderUI()->get_widget("LogWindow", t);
	t->signal_show().connect([this]() { get_buffer()->set_text(_log.str()); });
}

std::ostringstream* LogWindow::log()
{
	return &_log;
}
} // namespace widget
