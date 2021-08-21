#include "candidates.h"

#include "../utils.h"
#include "button.h"

#include <gtkmm/cellrenderertext.h>
#include <gtkmm/cellrenderertoggle.h>

namespace widget
{
Candidates::Candidates(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder) :
	Gtk::TreeView(cobject), _rows(Gtk::ListStore::create(_rowData)), _sortModel(nullptr)
{
	(void)refBuilder;

	set_model(_rows);

	{
		Gtk::CellRendererToggle* render = Gtk::manage(new Gtk::CellRendererToggle);
		render->set_fixed_size(80, 48);

		Gtk::TreeViewColumn* col =
			Gtk::manage(new Gtk::TreeViewColumn("CHECKED", *render));
		setColumnRender(col, render);

		append_column(*col);

		col->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
		col->set_fixed_width(80);
		col->set_resizable(false);
	}
	{
		Gtk::CellRendererText* render = Gtk::manage(new Gtk::CellRendererText);
		render->set_fixed_size(-1, 48);

		Gtk::TreeViewColumn* col = Gtk::manage(new Gtk::TreeViewColumn("NAME", *render));
		setColumnRender(col, render);
		col->add_attribute(render->property_text(), _rowData.Name);

		append_column(*col);
	}
	{
		Gtk::CellRendererText* render = Gtk::manage(new Gtk::CellRendererText);
		render->set_fixed_size(360, 48);

		Gtk::TreeViewColumn* col =
			Gtk::manage(new Gtk::TreeViewColumn("VERSION", *render));
		setColumnRender(col, render);
		col->add_attribute(render->property_text(), _rowData.Version);

		append_column(*col);

		col->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
		col->set_fixed_width(360);
		col->set_resizable(true);
	}
	{
		Gtk::CellRendererText* render = Gtk::manage(new Gtk::CellRendererText);
		render->set_fixed_size(380, 48);

		Gtk::TreeViewColumn* col =
			Gtk::manage(new Gtk::TreeViewColumn("ORIGIN", *render));
		setColumnRender(col, render);
		col->add_attribute(render->property_text(), _rowData.Origin);

		append_column(*col);

		col->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
		col->set_fixed_width(380);
		col->set_resizable(true);
	}
	{
		Gtk::CellRendererText* render = Gtk::manage(new Gtk::CellRendererText);
		render->set_fixed_size(120, 48);

		Gtk::TreeViewColumn* col =
			Gtk::manage(new Gtk::TreeViewColumn("ARCHITECTURE", *render));
		setColumnRender(col, render);
		col->add_attribute(render->property_text(), _rowData.Architecture);

		append_column(*col);

		col->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
		col->set_fixed_width(120);
		col->set_resizable(false);
	}
	{
		Gtk::CellRendererText* render = Gtk::manage(new Gtk::CellRendererText);
		render->set_fixed_size(120, 48);

		Gtk::TreeViewColumn* col = Gtk::manage(new Gtk::TreeViewColumn("SIZE", *render));
		setColumnRender(col, render);
		col->add_attribute(render->property_text(), _rowData.Size);

		append_column(*col);

		col->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
		col->set_fixed_width(120);
		col->set_resizable(false);
	}

	widget::Button* btnUpdate =
		utils::GetWidget<widget::Button>("ButtonUpdateAction", widget::Derived);
	btnUpdate->signal_clicked().connect(sigc::mem_fun(*this, &Candidates::refreshActual));
}

void Candidates::generate(package::CandidateType type, bool force)
{
	if(_candidates.find(type) == _candidates.end() || force) {
		// auto refresh!

		package::Cache cache;
		if(!cache.IsValid()) {
			// todo: show error
			return;
		}

		bool ok;
		_candidates[type] = cache.getCandidates(type, ok);
		if(!ok) {
			// todo: show error
			return;
		}
	}

	_rows->clear();

	switch(type) {
	case package::Upgradable: {
		get_column(0)->set_visible(true);

		_sortModel = ObjPtr<RowSort>(new RowSort(_rows));
		_sortModel->set_sort_column(_rowData.Name, Gtk::SORT_ASCENDING);
		set_model(_sortModel);

		break;
	}
	case package::Installed: {
		get_column(0)->set_visible(false);

		_sortModel.reset();
		set_model(_rows);

		break;
	}
	}

	for(const package::Candidate& candidate : _candidates.at(type)) {
		Gtk::TreeModel::Row row = *(_rows->append());

		row[_rowData.Checked] = true;
		row[_rowData.Name] = candidate.FullName;
		row[_rowData.Version] = candidate.Version;
		row[_rowData.Architecture] = candidate.Architecture;
		row[_rowData.Origin] = candidate.Origin;
		row[_rowData.Number] = candidate.Number;
		row[_rowData.Size] = candidate.SizeKB;
		row[_rowData.Align] = 0.5;

		setRowStyle(row);
	}

	_currentType = type;
}

void Candidates::refreshActual()
{
	generate(static_cast<package::CandidateType>(_currentType), true);
}

void Candidates::setRowStyle(Gtk::TreeModel::Row row)
{
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
}

void Candidates::setColumnRender(Gtk::TreeViewColumn* column, Gtk::CellRenderer* render)
{
	if(Gtk::CellRendererText* renderText = dynamic_cast<Gtk::CellRendererText*>(render)) {
		column->add_attribute(renderText->property_background_rgba(),
							  _rowData.BackgroundColor);
		column->add_attribute(renderText->property_foreground_rgba(),
							  _rowData.ForegroundColor);
		column->add_attribute(renderText->property_font_desc(), _rowData.Font);
	} else if(Gtk::CellRendererToggle* renderToggle =
				  dynamic_cast<Gtk::CellRendererToggle*>(render)) {
		column->add_attribute(renderToggle->property_active(), _rowData.Checked);
		// column->add_attribute(renderToggle->property_radio(), true);
	}
	column->add_attribute(render->property_xalign(), _rowData.Align);

	column->set_alignment(0.5);
}

Candidates::RowType::RowType()
{
	add(Checked);
	add(Name);
	add(Version);
	add(Architecture);
	add(Origin);
	add(Number);
	add(Size);

	add(BackgroundColor);
	add(ForegroundColor);
	add(Font);
	add(Align);
}

Candidates::RowSort::RowSort(const ObjPtr<Gtk::ListStore>& model) :
	Gtk::TreeModelSort(model)
{}
} // namespace widget
