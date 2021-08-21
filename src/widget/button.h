#ifndef BUTTON_H
#define BUTTON_H

#include "../type.h"

#include <gtkmm/button.h>
#include <gtkmm/builder.h>
#include <gtkmm/togglebutton.h>

namespace widget
{
template<typename TYPE>
class ButtonInterface : public TYPE
{
  public:
    using BaseCtor = ButtonInterface<TYPE>;

  public:
    ButtonInterface(typename TYPE::BaseObjectType* cobject);
    template<typename SLOT>
    void onClicked(const SLOT& slot);
};

template<typename TYPE>
ButtonInterface<TYPE>::ButtonInterface(typename TYPE::BaseObjectType* cobject) :
    TYPE(cobject)
{}

template<typename TYPE>
template<typename SLOT>
void ButtonInterface<TYPE>::onClicked(const SLOT& slot)
{
    TYPE::signal_clicked().connect(slot);
}

class Button : public ButtonInterface<Gtk::Button>
{
  public:
    Button(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder);
    virtual ~Button() = default;
};
class ToggleButton : public ButtonInterface<Gtk::ToggleButton>
{
  public:
    ToggleButton(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder);
    virtual ~ToggleButton() = default;
};
} // namespace widget

#endif // BUTTON_H
