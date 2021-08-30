#include "progressbar.h"

#include "../utils.h"

#include <gtkmm/main.h>

namespace widget
{
ProgressBar::ProgressBar(BaseObjectType* cobject,
                         const ObjPtr<Gtk::Builder>& refBuilder) :
    Gtk::ProgressBar(cobject),
    _cssProvider(Gtk::CssProvider::create())
{
	(void)refBuilder;
	DEBUG() << "Widget '" << get_name() << "': was created.";

	set_fraction(1.0);

	set_pulse_step(0.1);

	Glib::RefPtr<Gtk::StyleContext> styleContext = get_style_context();
	styleContext->add_provider(_cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	_cssProvider->load_from_data("progress, trough { min-height: 38px; }");
}

void ProgressBar::setRange(double start, double end, double step)
{
	(void)start;
	(void)end;
	(void)step;

	set_fraction(0.0);

	if(!is_visible()) set_visible(true);

	DEBUG() << "Widget '" << get_name() << "': set a new range [" << 0.0 << ":" << 1.0
			<< "].";
}

void ProgressBar::step(double step)
{
	set_fraction(get_fraction() + step);
}
} // namespace widget
