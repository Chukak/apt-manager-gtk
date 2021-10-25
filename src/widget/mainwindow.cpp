#include "mainwindow.h"
#include "button.h"

#include "menu.h"
#include "../utils.h"

#include <gdkmm/pixbuf.h>

namespace widget
{
MainWindow::MainWindow(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder) :
	Gtk::Window(cobject)
{
	(void)refBuilder;
	DEBUG() << "Widget '" << get_name() << "': was created.";
}

void MainWindow::initUI()
{
	widget::Button* btnExit = utils::GetWidgetDerived<widget::Button>("ButtonExitAction");
	btnExit->set_label("EXIT");
	btnExit->set_tooltip_text(btnExit->get_label());
	DEBUG() << "Widget '" << btnExit->get_name() << "': was configured.";

	widget::Button* btnOpenMenu =
		utils::GetWidgetDerived<widget::Button>("ButtonOpenMenu");
	btnOpenMenu->set_label("MENU");
	btnOpenMenu->set_tooltip_text(btnOpenMenu->get_label());
	DEBUG() << "Widget '" << btnOpenMenu->get_name() << "': was configured.";

	widget::Button* btnOpenLog = utils::GetWidgetDerived<widget::Button>("ButtonOpenLog");
	btnOpenLog->set_label("LOG");
	btnOpenLog->set_tooltip_text(btnOpenLog->get_label());
	DEBUG() << "Widget '" << btnOpenLog->get_name() << "': was configured.";

	widget::Button* btnUpdate = utils::GetWidgetDerived<widget::Button>("ButtonUpdate");
	btnUpdate->set_label("REFRESH");
	btnUpdate->set_tooltip_text(btnUpdate->get_label());
	DEBUG() << "Widget '" << btnUpdate->get_name() << "': was configured.";

	widget::ToggleButton* btnSearch =
		utils::GetWidgetDerived<widget::ToggleButton>("ButtonOpenSearch");
	btnSearch->set_label("SEARCH");
	btnSearch->set_tooltip_text("Open Search");
	DEBUG() << "Widget '" << btnSearch->get_name() << "': was configured.";

	set_icon(Gdk::Pixbuf::create_from_resource("/icon/package_128x128.png"));
}
} // namespace widget
