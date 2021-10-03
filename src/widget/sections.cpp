#include "sections.h"
#include "candidates.h"

#include "../package/cache.h"
#include "../utils.h"

#include <gtkmm/cellrenderertext.h>

namespace widget
{
Sections::Sections(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder) :
	Gtk::TreeView(cobject), _rows(Gtk::ListStore::create(_rowData))
{
	(void)refBuilder;
	DEBUG() << "Widget '" << get_name() << "': was created.";

	set_model(_rows);

	{
		Gtk::CellRendererText* render = Gtk::manage(new Gtk::CellRendererText);
		render->set_fixed_size(-1, 48);
		render->set_alignment(0.5, 0.5);

		Gtk::TreeViewColumn* col = Gtk::manage(new Gtk::TreeViewColumn("", *render));
		col->add_attribute(render->property_background_rgba(), _rowData.BackgroundColor);
		col->add_attribute(render->property_text(), _rowData.Title);
		col->add_attribute(render->property_foreground_rgba(), _rowData.ForegroundColor);
		col->add_attribute(render->property_font_desc(), _rowData.Font);

		append_column(*col);
	}

	for(int32_t t = package::FirstType; t <= package::LastType; ++t) {
		Gtk::TreeModel::Row row = *(_rows->append());

		switch(t) {
		case package::List_Of_Installed: {
			row[_rowData.Title] = "LIST OF INSTALLED";
			break;
		}
		case package::Update: {
			row[_rowData.Title] = "UPDATE";
			break;
		}
		case package::Install: {
			row[_rowData.Title] = "INSTALL";
			break;
		}
		}

		Gdk::RGBA background;
		// #E4E1ED
		background.set_rgba(228, 225, 237, 0.04);

		row[_rowData.BackgroundColor] = background;
		if(getuid() > 0 /* non-root */) {
			row[_rowData.ForegroundColor] = Gdk::RGBA("#F5F2FF");
		} else {
			row[_rowData.ForegroundColor] = Gdk::RGBA("#0F0F0F");
		}
		row[_rowData.Font] = Pango::FontDescription("ROBOTO 16");
		row[_rowData.PackageType] = t;
	}

	get_selection()->signal_changed().connect(
		sigc::mem_fun(*this, &Sections::onRowSelected));
	DEBUG()
		<< "Widget '" << get_name()
		<< "': connected to signal_changed(), using the slot Sections::onRowSelected.";
} // namespace widget

void Sections::onRowSelected()
{
	Gtk::TreeModel::iterator iter = get_selection()->get_selected();
	if(iter) {
		int32_t type = (*iter)[_rowData.PackageType];
		DEBUG() << "Widget '" << get_name() << "': '" << (*iter)[_rowData.Title]
				<< "' was selected.";

		widget::Candidates* cand =
			utils::GetWidgetDerived<widget::Candidates>("CandidatesTree");
		cand->generate(static_cast<package::CandidateType>(type));
	}
}

Sections::RowType::RowType()
{
	add(Title);
	add(BackgroundColor);
	add(ForegroundColor);
	add(Font);
	add(PackageType);
}
} // namespace widget
