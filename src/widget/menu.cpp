#include "menu.h"
#include "utils.h"
#include "button.h"

namespace widget
{
Menu::Menu() : Gtk::Menu()
{
	{
		Gtk::CheckMenuItem* actSelectAll =
			Gtk::manage(new Gtk::CheckMenuItem("Select all"));
		actSelectAll->set_name("MenuSelectAllAction");

		append(*actSelectAll);
		_items.push_back(actSelectAll);

		actSelectAll->hide();
	}
	{
		Gtk::MenuItem* actInstall = Gtk::manage(new Gtk::MenuItem("Install"));
		actInstall->set_name("MenuInstallAction");

		append(*actInstall);
		_items.push_back(actInstall);

		actInstall->hide();
	}

	show();

	checkItemsVisible();

	signal_popped_up().connect([this](...) { _isPopupAtWidget = true; });
	signal_hide().connect([this]() { this->_isPopupAtWidget = false; });
}

void Menu::rebuildByType(package::CandidateType type)
{
	switch(type) {
	case package::List_Of_Installed: {
		showItems(false, "MenuSelectAllAction", "MenuInstallAction");
		break;
	}
	case package::Update:
	case package::Install: {
		showItems(true, "MenuSelectAllAction", "MenuInstallAction");
		break;
	}
	}

	checkItemsVisible();
}
void Menu::checkItemsVisible()
{
	int countVisibleItems = 0;
	for(const Gtk::MenuItem* const item : _items)
		if(item->get_visible()) ++countVisibleItems;

	widget::Button* btnOpenMenu =
		utils::GetWidgetDerived<widget::Button>("ButtonOpenMenu");
	btnOpenMenu->set_sensitive(countVisibleItems > 0);
	btnOpenMenu->set_data(utils::widget::property::SetSensitiveSkip,
						  countVisibleItems > 0 ? utils::widget::property::Disable()
												: utils::widget::property::Enable());
}

bool Menu::is_popup_at_widget()
{
	return _isPopupAtWidget;
}
} // namespace widget
