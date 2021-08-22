#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include "../type.h"

#include <gtkmm/progressbar.h>
#include <gtkmm/builder.h>

#include <gtkmm/cssprovider.h>

#include <atomic>

namespace widget
{
class ProgressBar : public Gtk::ProgressBar
{
  public:
    ProgressBar(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder);

	void setRange(double start, double end, double step);
	void step(double step);

  private:
    Glib::RefPtr<Gtk::CssProvider> _cssProvider;
};
} // namespace widget

#endif // PROGRESSBAR_H
