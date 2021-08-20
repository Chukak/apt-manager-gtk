#include "button.h"

namespace widget
{
Button::Button(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder) :
	Gtk::Button(cobject)
{
	(void)refBuilder;
}
} // namespace widget
