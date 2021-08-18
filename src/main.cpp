#include "package/cache.h"
#include "utils.h"

#include "widget/sections.h"
#include "widget/mainwindow.h"

#include <gtkmm/application.h>
#include <gtkmm/builder.h>
#include <gtkmm/applicationwindow.h>
#include <giomm/resource.h>

int main(int argc, char** argv)
{
	// load resources
	WRAP_EXCPT(std::exception, {
		Glib::RefPtr<Gio::Resource> resource_bundle =
			Gio::Resource::create_from_file(UI_RESOURCE_FILE);
		resource_bundle->register_global();
	})

	Glib::RefPtr<Gtk::Application> app(Gtk::Application::create(argc, argv));

	widget::MainWindow* appWin =
		utils::GetWidget<widget::MainWindow>("MainWindow", widget::Derived);
	widget::Sections* sectionsView =
		utils::GetWidget<widget::Sections>("SectionsTree", widget::Derived);

	int rc = app->run(*appWin);

	delete appWin;
	delete sectionsView;

	return rc;
}
