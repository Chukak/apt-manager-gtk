#include "button.h"

namespace widget
{
Button::Button(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder) :
	BaseCtor(cobject)
{
	(void)refBuilder;
}

ToggleButton::ToggleButton(BaseObjectType* cobject,
                           const ObjPtr<Gtk::Builder>& refBuilder) :
    BaseCtor(cobject)
{
    (void)refBuilder;
}
} // namespace widget
