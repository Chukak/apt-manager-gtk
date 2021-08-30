#include "mainwindow.h"
#include "button.h"

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

	widget::Button* btnUpdate =
		utils::GetWidgetDerived<widget::Button>("ButtonUpdateAction");
	btnUpdate->set_label("Refresh");
	btnUpdate->set_tooltip_text(btnUpdate->get_label());
	DEBUG() << "Widget '" << btnUpdate->get_name() << "': was configured.";

	widget::Button* btnInstall =
		utils::GetWidgetDerived<widget::Button>("ButtonInstallAction");
	btnInstall->set_label("Install");
	btnInstall->set_tooltip_text(btnInstall->get_label());
	DEBUG() << "Widget '" << btnInstall->get_name() << "': was configured.";

	widget::ToggleButton* btnSelectAll =
		utils::GetWidgetDerived<widget::ToggleButton>("ToggleButtonSelectAllAction");
	btnSelectAll->set_label("Select All");
	btnSelectAll->set_tooltip_text(btnSelectAll->get_label());
	btnSelectAll->set_visible(false);
	DEBUG() << "Widget '" << btnSelectAll->get_name() << "': was configured.";

	widget::Button* btnOpenLog = utils::GetWidgetDerived<widget::Button>("ButtonOpenLog");
	btnOpenLog->set_label("Open Log");
	btnOpenLog->set_tooltip_text(btnSelectAll->get_label());
	DEBUG() << "Widget '" << btnOpenLog->get_name() << "': was configured.";
}
} // namespace widget
