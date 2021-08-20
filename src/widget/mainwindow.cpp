#include "mainwindow.h"
#include "button.h"

#include "../utils.h"

namespace widget
{
MainWindow::MainWindow(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder) :
	Gtk::Window(cobject)
{
	(void)refBuilder;
}

void MainWindow::initUI()
{
	widget::Button* btnExit =
		utils::GetWidget<widget::Button>("ButtonExitAction", widget::Derived);
	btnExit->set_label("Exit");
	btnExit->set_tooltip_text(btnExit->get_label());

	widget::Button* btnUpdate =
		utils::GetWidget<widget::Button>("ButtonUpdateAction", widget::Derived);
	btnUpdate->set_label("Refresh");
	btnUpdate->set_tooltip_text(btnUpdate->get_label());

	widget::Button* btnInstall =
		utils::GetWidget<widget::Button>("ButtonInstallAction", widget::Derived);
	btnInstall->set_label("Install");
	btnInstall->set_tooltip_text(btnInstall->get_label());
}
} // namespace widget
