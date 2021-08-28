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
	widget::Button* btnExit = utils::GetWidgetDerived<widget::Button>("ButtonExitAction");
	btnExit->set_label("Exit");
	btnExit->set_tooltip_text(btnExit->get_label());

	widget::Button* btnUpdate =
		utils::GetWidgetDerived<widget::Button>("ButtonUpdateAction");
	btnUpdate->set_label("Refresh");
	btnUpdate->set_tooltip_text(btnUpdate->get_label());

	widget::Button* btnInstall =
		utils::GetWidgetDerived<widget::Button>("ButtonInstallAction");
	btnInstall->set_label("Install");
	btnInstall->set_tooltip_text(btnInstall->get_label());

	widget::ToggleButton* btnSelectAll =
		utils::GetWidgetDerived<widget::ToggleButton>("ToggleButtonSelectAllAction");
	btnSelectAll->set_label("Select All");
	btnSelectAll->set_tooltip_text(btnSelectAll->get_label());
	btnSelectAll->set_visible(false);

	widget::Button* btnOpenLog = utils::GetWidgetDerived<widget::Button>("ButtonOpenLog");
	btnOpenLog->set_label("Open Log");
	btnOpenLog->set_tooltip_text(btnSelectAll->get_label());
}
} // namespace widget
