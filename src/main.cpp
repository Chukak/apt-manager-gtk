#include "package/cache.h"
#include "utils.h"

#include <gtkmm/application.h>
#include <gtkmm/builder.h>
#include <gtkmm/applicationwindow.h>
#include <giomm/resource.h>

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	// load resources
	Glib::RefPtr<Gio::Resource> resource_bundle =
		Gio::Resource::create_from_file(UI_RESOURCE_FILE);
	resource_bundle->register_global();

	Glib::RefPtr<Gtk::Application> app(Gtk::Application::create(argc, argv));
	Gtk::ApplicationWindow* appWin;

	Glib::RefPtr<Gtk::Builder> ui(Gtk::Builder::create_from_resource("/ui/main.ui"));
	ui->get_widget<Gtk::ApplicationWindow>("Main", appWin);

	/*
	package::Cache cache;
	package::CandidateList candidates = cache.getCandidates(package::Upgradable);
	utils::GetLog() << candidates << std::endl;
	*/

	return app->run(*appWin);
}
