#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../type.h"

#include <gtkmm/window.h>
#include <gtkmm/builder.h>

namespace widget
{
class MainWindow : public Gtk::Window
{
  public:
    MainWindow(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder);
    virtual ~MainWindow() = default;

    void initUI();
};
} // namespace widget

#endif // MAINWINDOW_H
