#include "sections.h"

#include "../package/cache.h"
#include "../utils.h"

#include <gtkmm/cellrenderertext.h>

namespace widget
{
Sections::Sections(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder) :
	Gtk::TreeView(cobject), _rows(Gtk::ListStore::create(_rowData))
{
	(void)refBuilder;

	set_model(_rows);

	{
		Gtk::CellRendererText* render = Gtk::manage(new Gtk::CellRendererText);
		render->set_fixed_size(-1, 48);

		Gtk::TreeViewColumn* col =
			Gtk::manage(new Gtk::TreeViewColumn("PACKAGES", *render));
		col->add_attribute(render->property_background_rgba(), _rowData.BackgroundColor);
		col->add_attribute(render->property_text(), _rowData.Title);
		col->add_attribute(render->property_foreground_rgba(), _rowData.ForegroundColor);
		col->add_attribute(render->property_font_desc(), _rowData.Font);
		col->set_alignment(0.5);

		append_column(*col);
	}

	for(int32_t t = package::FirstType; t <= package::LastType; ++t) {
		Gtk::TreeModel::Row row = *(_rows->append());

		switch(t) {
		case package::Installed: {
			row[_rowData.Title] = "Show installed";
			break;
		}
		case package::Upgradable: {
			row[_rowData.Title] = "Suggested for updating";
			break;
		}
		}

		Gdk::RGBA background;
		// #E4E1ED
		background.set_rgba(228, 225, 237, 0.04);

		row[_rowData.BackgroundColor] = background;
		row[_rowData.ForegroundColor] = Gdk::RGBA("#F5F2FF");
		row[_rowData.Font] = Pango::FontDescription("ROBOTO 16");
	}
}

void Sections::on_row_activated(const Gtk::TreeModel::Path& path,
								Gtk::TreeViewColumn* column)
{
	(void)path;
	(void)column;
}

Sections::RowType::RowType()
{
	add(Title);
	add(BackgroundColor);
	add(ForegroundColor);
	add(Font);
}
} // namespace widget
