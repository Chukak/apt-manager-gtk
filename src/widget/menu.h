#ifndef MENU_H
#define MENU_H

#include "../type.h"
#include "../package/cache.h"

#include <gtkmm/builder.h>
#include <gtkmm/menu.h>
#include <gtkmm/checkmenuitem.h>

namespace widget
{
class Menu : public Gtk::Menu
{
  public:
    Menu();

	template<typename TYPE>
	TYPE* getItem(const Glib::ustring& name);
	void rebuildByType(package::CandidateType type);
	/**
	 * @brief is_popup_at_widget
	 * @return Is this menu is popped up.
	 */
	bool is_popup_at_widget();

  private:
    template<typename... Args>
    typename std::
        enable_if_t<std::conjunction_v<std::is_constructible<std::string, Args>...>, void>
        showItems(bool show, const Args&... args);
    void checkItemsVisible();

  private:
    std::vector<Gtk::MenuItem*> _items;
    bool _isPopupAtWidget{false};
};

template<typename TYPE>
TYPE* Menu::getItem(const Glib::ustring& name)
{
	decltype(_items)::iterator found =
		std::find_if(_items.begin(),
					 _items.end(),
					 [name](const Gtk::MenuItem* const item) {
						 return item->get_name() == name;
					 });
	return (found != _items.end()) ? dynamic_cast<TYPE*>(*found) : nullptr;
}

template<typename... Args>
typename std::enable_if_t<std::conjunction_v<std::is_constructible<std::string, Args>...>,
						  void>
Menu::showItems(bool show, const Args&... args)
{
	for(const std::string& name : {args...}) {
		Gtk::MenuItem* item = getItem<Gtk::MenuItem>(name);
		if(item) {
			show ? item->show() : item->hide();
		}
	}
}
} // namespace widget
#endif // MENU_H
