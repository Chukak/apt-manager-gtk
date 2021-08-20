#ifndef BUTTON_H
#define BUTTON_H

#include "../type.h"

#include <gtkmm/button.h>
#include <gtkmm/builder.h>

namespace widget
{
class Button : public Gtk::Button
{
  public:
    Button(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder);
    virtual ~Button() = default;

	template<typename SLOT>
	void onClicked(const SLOT& slot);
};

template<typename SLOT>
void Button::onClicked(const SLOT& slot)
{
	signal_clicked().connect(slot);
}
} // namespace widget

#endif // BUTTON_H
