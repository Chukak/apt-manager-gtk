#include "package/cache.h"
#include "utils.h"

#include "widget/sections.h"
#include "widget/mainwindow.h"
#include "widget/button.h"
#include "widget/progressbar.h"
#include "widget/logwindow.h"
#include "widget/candidates.h"
#include "widget/menu.h"

#include <gtkmm/box.h>
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

	widget::Menu* menu = utils::GetCustomWidget<widget::Menu>("MainMenu");

	widget::Button* btnOpenMenu =
		utils::GetWidgetDerived<widget::Button>("ButtonOpenMenu");
	btnOpenMenu->signal_clicked().connect([btnOpenMenu, menu]() {
		if(!menu->is_popup_at_widget())
			menu->popup_at_widget(btnOpenMenu,
								  Gdk::GRAVITY_NORTH_EAST,
								  Gdk::GRAVITY_NORTH_WEST,
								  nullptr);
		else
			menu->hide();
	});

	widget::Button* btnUpdate = utils::GetWidgetDerived<widget::Button>("ButtonUpdate");
	btnUpdate->set_sensitive(false);

	widget::Candidates* candidatesView =
		utils::GetWidgetDerived<widget::Candidates>("CandidatesTree");

	widget::Sections* sectionsView =
		utils::GetWidgetDerived<widget::Sections>("SectionsTree");
	(void)sectionsView;

	widget::ProgressBar* progressBar =
		utils::GetWidgetDerived<widget::ProgressBar>("MainProgressBar");
	progressBar->set_fraction(1.0);

	widget::Button* btnExit = utils::GetWidgetDerived<widget::Button>("ButtonExitAction");
	btnExit->onClicked([&app]() { app->quit(); });

	Gtk::Box* bottomEntryBox = nullptr;
	utils::GetBuilderUI()->get_widget<Gtk::Box>("BottomEntryBox", bottomEntryBox);
	if(!bottomEntryBox) {
		INFO() << "Widget 'BottomEntryBox' not configured.";
	}

	widget::ToggleButton* btnOpenSearch =
		utils::GetWidgetDerived<widget::ToggleButton>("ButtonOpenSearch");
	btnOpenSearch->set_sensitive(false);
	btnOpenSearch->signal_pressed().connect([bottomEntryBox, btnOpenSearch]() {
		if(!bottomEntryBox->is_visible()) {
			bottomEntryBox->show();
		} else {
			bottomEntryBox->hide();
		}
	});

	candidatesView->signal_generated().connect(
		[btnOpenSearch](size_t count) { btnOpenSearch->set_sensitive(count > 0); });

	return app->run(*appWin);
}
