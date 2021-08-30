#include "logwindow.h"
#include <gtkmm/window.h>
#include <glibmm/main.h>

namespace widget
{
LogWindow::LogWindow(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder) :
	Gtk::TextView(cobject)
{
	(void)refBuilder;
	_log << "Widget '" << get_name() << "': was created." << std::endl;

	set_editable(false);
	set_wrap_mode(Gtk::WRAP_NONE);

	_timer = Glib::signal_timeout().connect(
		[this]() -> bool {
			this->get_buffer()->set_text(this->log()->str());
			return true;
		},
		1000 /* ms */);
	_log << "Widget '" << get_name()
		 << "': connected to signal_timeout(), using the slot <lambda>." << std::endl;
}

std::ostringstream* LogWindow::log()
{
	return &_log;
}
} // namespace widget
