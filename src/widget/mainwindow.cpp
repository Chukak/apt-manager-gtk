#include "mainwindow.h"
#include "button.h"

#include "menu.h"
#include "../utils.h"

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
	btnExit->set_label("Exit");
	btnExit->set_tooltip_text(btnExit->get_label());
	DEBUG() << "Widget '" << btnExit->get_name() << "': was configured.";

	widget::Button* btnOpenMenu =
		utils::GetWidgetDerived<widget::Button>("ButtonOpenMenu");
	btnOpenMenu->set_label("Menu");
	btnOpenMenu->set_tooltip_text(btnOpenMenu->get_label());
	DEBUG() << "Widget '" << btnOpenMenu->get_name() << "': was configured.";

	widget::Button* btnOpenLog = utils::GetWidgetDerived<widget::Button>("ButtonOpenLog");
	btnOpenLog->set_label("Open Log");
	btnOpenLog->set_tooltip_text(btnOpenLog->get_label());
	DEBUG() << "Widget '" << btnOpenLog->get_name() << "': was configured.";

	widget::Button* btnUpdate = utils::GetWidgetDerived<widget::Button>("ButtonUpdate");
	btnUpdate->set_label("Update");
	btnUpdate->set_tooltip_text(btnUpdate->get_label());
	DEBUG() << "Widget '" << btnUpdate->get_name() << "': was configured.";
}
} // namespace widget
