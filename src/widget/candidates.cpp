#include "candidates.h"

#include "../utils.h"
#include "button.h"
#include "progressbar.h"
#include "../extension/progressrange.h"
#include "../extension/progresspulse.h"

#include "../package/progressacquirestatus.h"

#include <gtkmm/cellrenderertext.h>
#include <gtkmm/cellrenderertoggle.h>
#include <gtkmm/main.h>
#include <gtkmm/messagedialog.h>

#include <thread>

namespace widget
{
Candidates::Candidates(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder) :
	Gtk::TreeView(cobject), _rows(Gtk::ListStore::create(_rowData)), _sortModel(nullptr)
{
	(void)refBuilder;
	DEBUG() << "Widget '" << get_name() << "': was created.";

	set_model(_rows);

	{
		Gtk::CellRendererToggle* render = Gtk::manage(new Gtk::CellRendererToggle);
		render->set_fixed_size(80, 48);
		render->set_activatable(true);
		render->signal_toggled().connect(
			sigc::mem_fun(*this, &Candidates::onToggleColumn));

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
		col->set_resizable(true);

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
		utils::GetWidgetDerived<widget::Button>("ButtonUpdateAction");
	btnUpdate->signal_clicked().connect(sigc::mem_fun(*this, &Candidates::refreshActual));
	DEBUG()
		<< "Widget '" << btnUpdate->get_name()
		<< "': connected to signal_clicked(), using the slot Candidates::refreshActual.";

	widget::ToggleButton* btnSelectAll =
		utils::GetWidgetDerived<widget::ToggleButton>("ToggleButtonSelectAllAction");
	btnSelectAll->signal_clicked().connect(sigc::mem_fun(*this, &Candidates::selectAll));
	DEBUG() << "Widget '" << btnSelectAll->get_name()
			<< "': connected to signal_clicked(), using the slot Candidates::selectAll.";
}

void Candidates::generate(package::CandidateType type, bool force)
{
	extension::ProgressRange progressRange;
	extension::ProgressPulse progressPulse;

	widget::ProgressBar* progressBar =
		utils::GetWidgetDerived<widget::ProgressBar>("MainProgressBar");
	progressBar->set_fraction(0.0);

	// range
	progressRange.signal_rangeChanged().connect(
		sigc::mem_fun(*progressBar, &ProgressBar::setRange));
	progressRange.signal_changed().connect([progressBar](double, double step) {
		progressBar->step(step);
		// next event
		if(Gtk::Main::events_pending()) {
			Gtk::Main::iteration(false);
		}
	});
	progressRange.signal_reseted().connect(
		[progressBar]() { progressBar->set_fraction(0.0); });

	// pulse
	progressPulse.signal_pulsed().connect([progressBar]() {
		progressBar->pulse(); // next event
		if(Gtk::Main::events_pending()) {
			Gtk::Main::iteration(false);
		}
	});

	DEBUG() << "Widget '" << progressBar->get_name() << "': was configured.";

	utils::widget::EnableWidgets(false,
								 "ButtonUpdateAction",
								 "ButtonInstallAction",
								 "ToggleButtonSelectAllAction",
								 "ButtonOpenLog",
								 "SectionsTree");

	if(_candidates.find(type) == _candidates.end() || force) {
		// auto refresh!

		package::Cache cache;
		if(!cache.IsValid()) {
			Gtk::MessageDialog dialog("The package cache is invalid.");
			dialog.set_title("Warning!");
			dialog.run();

			utils::widget::EnableWidgets(true,
										 "ButtonUpdateAction",
										 "ButtonInstallAction",
										 "ToggleButtonSelectAllAction",
										 "ButtonOpenLog",
										 "SectionsTree");
			return;
		}

		package::ProgressAcquireStatus status(&progressPulse);
		bool ok;
		_candidates[type] = cache.getCandidates(type, ok, &progressRange, &status);

		if(!ok) {
			Gtk::MessageDialog dialog("Errors occurred while updating the cache.");
			dialog.set_title("Warning!");
			dialog.run();

			utils::widget::EnableWidgets(true,
										 "ButtonUpdateAction",
										 "ButtonInstallAction",
										 "ToggleButtonSelectAllAction",
										 "ButtonOpenLog",
										 "SectionsTree");
			return;
		}
	}

	_rows->clear();

	widget::ToggleButton* btnSelectAll =
		utils::GetWidgetDerived<widget::ToggleButton>("ToggleButtonSelectAllAction");

	switch(type) {
	case package::Upgradable: {
		get_column(0)->set_visible(true);

		_sortModel = ObjPtr<RowSort>(new RowSort(_rows));
		_sortModel->set_sort_column(_rowData.Name, Gtk::SORT_ASCENDING);
		set_model(_sortModel);

		btnSelectAll->set_visible(true);

		break;
	}
	case package::Installed: {
		get_column(0)->set_visible(false);

		_sortModel.reset();
		set_model(_rows);

		btnSelectAll->set_active(false);
		btnSelectAll->set_visible(false);

		break;
	}
	}

	progressRange.setRange(0, static_cast<int>(_candidates.at(type).size()));
	progressRange.reset();
	progressBar->set_fraction(0.0);

	for(const package::Candidate& candidate : _candidates.at(type)) {
		Gtk::TreeModel::Row row = *(_rows->append());

		row[_rowData.Checked] = false;
		row[_rowData.Name] = candidate.FullName;
		row[_rowData.Version] = candidate.Version;
		row[_rowData.Architecture] = candidate.Architecture;
		row[_rowData.Origin] = candidate.Origin;
		row[_rowData.Number] = candidate.Number;
		row[_rowData.Size] = candidate.SizeKB;
		row[_rowData.Align] = 0.5;

		setRowStyle(row);

		progressRange.increment();
	}

	utils::widget::EnableWidgets(true,
								 "ButtonUpdateAction",
								 "ButtonInstallAction",
								 "ToggleButtonSelectAllAction",
								 "ButtonOpenLog",
								 "SectionsTree");

	DEBUG() << "Widget '" << get_name() << "': added new rows successfully.";

	_currentType = type;
} // namespace widget

void Candidates::refreshActual()
{
	DEBUG() << "Widget '" << get_name() << "': Candidates::refreshActual was called.";

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
		(void)renderToggle;
		column->add_attribute(renderToggle->property_active(), _rowData.Checked);
	}
	column->add_attribute(render->property_xalign(), _rowData.Align);

	column->set_alignment(0.5);
}

void Candidates::onToggleColumn(const Glib::ustring& path)
{
	Gtk::TreeModel::iterator iter;
	if(get_model() == _sortModel) {
		iter = _sortModel->get_iter(path);
	} else {
		iter = _rows->get_iter(path);
	}

	if(iter) {
		(*iter)[_rowData.Checked] = !(*iter)[_rowData.Checked];
	}
}

void Candidates::selectAll()
{
	DEBUG() << "Widget '" << get_name() << "': Candidates::selectAll was called.";

	if(!get_column(0)->get_visible()) return;

	widget::ToggleButton* btnSelectAll =
		utils::GetWidgetDerived<widget::ToggleButton>("ToggleButtonSelectAllAction");

	for(Gtk::TreeModel::Row row : _rows->children()) {
		if(btnSelectAll->get_active() /* button is pressed */)
			row[_rowData.Checked] = true;
		else
			row[_rowData.Checked] = false;
	}
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
