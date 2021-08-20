#include "package/cache.h"
#include "utils.h"

#include "widget/sections.h"
#include "widget/mainwindow.h"
#include "widget/button.h"

#include <gtkmm/application.h>
#include <gtkmm/builder.h>
#include <gtkmm/applicationwindow.h>
#include <giomm/resource.h>

int main(int argc, char** argv)
{
	// load resources
	WRAP_EXCPT(std::exception, {
		ObjPtr<Gio::Resource> resource_bundle =
			Gio::Resource::create_from_file(UI_RESOURCE_FILE);
		resource_bundle->register_global();
	})

	ObjPtr<Gtk::Application> app(Gtk::Application::create(argc, argv));

	widget::MainWindow* appWin =
		utils::GetWidget<widget::MainWindow>("MainWindow", widget::Derived);
	appWin->initUI();

	widget::Sections* sectionsView =
		utils::GetWidget<widget::Sections>("SectionsTree", widget::Derived);
	(void)sectionsView;

	widget::Button* btnExit =
		utils::GetWidget<widget::Button>("ButtonExitAction", widget::Derived);
	btnExit->onClicked([&app]() { app->quit(); });

	return app->run(*appWin);
}
