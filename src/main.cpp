#include "package/cache.h"
#include "utils.h"

#include "widget/sections.h"
#include "widget/mainwindow.h"
#include "widget/button.h"
#include "widget/progressbar.h"
#include "widget/logwindow.h"
#include "widget/candidates.h"

#include <gtkmm/application.h>
#include <gtkmm/builder.h>
#include <gtkmm/applicationwindow.h>
#include <giomm/resource.h>

int main(int argc, char** argv)
{
#ifdef NDEBUG
	utils::SetLogFlags(utils::LogFile);
#else
	utils::SetLogFlags(utils::LogFile | utils::LogLine | utils::LogVerbose);
#endif
	// load resources
	WRAP_EXCPT(std::exception, {
		ObjPtr<Gio::Resource> resource_bundle =
			Gio::Resource::create_from_file(UI_RESOURCE_FILE);
		resource_bundle->register_global();
	})

	ObjPtr<Gtk::Application> app(Gtk::Application::create(argc, argv));

	widget::LogWindow* logView =
		utils::GetWidgetDerived<widget::LogWindow>("LogTextView");
	utils::SetErrorLog(logView->log());
	utils::SetLog(logView->log());

	widget::MainWindow* appWin =
		utils::GetWidgetDerived<widget::MainWindow>("MainWindow");
	appWin->initUI();

	widget::Button* btnOpenLog = utils::GetWidgetDerived<widget::Button>("ButtonOpenLog");
	btnOpenLog->onClicked([logView, btnOpenLog]() {
		Gtk::Window* t = nullptr;
		utils::GetBuilderUI()->get_widget("LogWindow", t);

		if(t && !t->is_visible()) t->show();
	});

	widget::Candidates* candidatesView =
		utils::GetWidgetDerived<widget::Candidates>("CandidatesTree");
	(void)candidatesView;

	widget::Sections* sectionsView =
		utils::GetWidgetDerived<widget::Sections>("SectionsTree");
	(void)sectionsView;

	widget::ProgressBar* progressBar =
		utils::GetWidgetDerived<widget::ProgressBar>("MainProgressBar");
	progressBar->set_fraction(1.0);

	widget::Button* btnExit = utils::GetWidgetDerived<widget::Button>("ButtonExitAction");
	btnExit->onClicked([&app]() { app->quit(); });

	return app->run(*appWin);
}
