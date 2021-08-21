#include "candidates.h"

#include "../utils.h"
#include "button.h"

#include <gtkmm/cellrenderertext.h>

namespace widget
{
Candidates::Candidates(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder) :
	Gtk::TreeView(cobject), _rows(Gtk::ListStore::create(_rowData))
{
	(void)refBuilder;

	set_model(_rows);

	{
		Gtk::TreeViewColumn* col = createColumn(_rowData.Checked);
		col->set_title("CHECKED");

		append_column(*col);
	}
	{
		Gtk::TreeViewColumn* col = createColumn(_rowData.Name);
		col->set_title("NAME");

		append_column(*col);
	}
	{
		Gtk::TreeViewColumn* col = createColumn(_rowData.Version);
		col->set_title("VERSION");

		append_column(*col);
	}
	{
		Gtk::TreeViewColumn* col = createColumn(_rowData.Architecture);
		col->set_title("Architecture");

		append_column(*col);
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

		for(const package::Candidate& candidate : _candidates.at(type)) {
			Gtk::TreeModel::Row row = *(_rows->append());

			row[_rowData.Checked] = true;
			row[_rowData.Name] = candidate.FullName;
			row[_rowData.Version] = candidate.Version;
			row[_rowData.Architecture] = candidate.Architecture;
			row[_rowData.Number] = candidate.Number;

			setRowStyle(row);
		}
		break;
	}
	case package::Installed: {
		get_column(0)->set_visible(false);

		for(const package::Candidate& candidate : _candidates.at(type)) {
			Gtk::TreeModel::Row row = *(_rows->append());

			// row[_rowData.Checked] = true;
			row[_rowData.Name] = candidate.FullName;
			row[_rowData.Version] = candidate.Version;
			row[_rowData.Architecture] = candidate.Architecture;
			row[_rowData.Number] = candidate.Number;

			setRowStyle(row);
		}
		break;
	}
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

Candidates::RowType::RowType()
{
	add(Checked);
	add(Name);
	add(Version);
	add(Architecture);
	add(Number);

	add(BackgroundColor);
	add(ForegroundColor);
	add(Font);
}
} // namespace widget
