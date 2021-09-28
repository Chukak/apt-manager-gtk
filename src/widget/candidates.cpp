#include "candidates.h"

#include "../utils.h"
#include "button.h"
#include "progressbar.h"
#include "menu.h"
#include "../extension/progressrange.h"
#include "../extension/progresspulse.h"

#include "../package/progressacquirestatus.h"

#include <gtkmm/cellrenderertext.h>
#include <gtkmm/cellrenderertoggle.h>
#include <gtkmm/main.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/entry.h>

#include <thread>

static const int _MaxCountRows_signal_changed =
#ifdef _FIXED_ROWS_SIGNAL_CHANGED
    _FIXED_ROWS_SIGNAL_CHANGED;
#else
    11000;
#endif

namespace widget
{
Candidates::Candidates(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder) :
    Gtk::TreeView(cobject), _rows(Gtk::ListStore::create(_rowData)), _sortModel(nullptr),
    _filterModel(nullptr)
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

		Gtk::TreeViewColumn* col = Gtk::manage(new Gtk::TreeViewColumn("", *render));
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

	widget::Button* btnUpdate = utils::GetWidgetDerived<widget::Button>("ButtonUpdate");
	btnUpdate->signal_clicked().connect(sigc::mem_fun(*this, &Candidates::refreshActual));
	DEBUG()
		<< "Widget '" << btnUpdate->get_name()
		<< "': connected to signal_clicked(), using the slot Candidates::refreshActual.";

	widget::Menu* menu = utils::GetCustomWidget<widget::Menu>("MainMenu");
	if(Gtk::CheckMenuItem* item =
		   menu->getItem<Gtk::CheckMenuItem>("MenuSelectAllAction")) {
		item->signal_toggled().connect(sigc::mem_fun(*this, &Candidates::selectAll));
		DEBUG() << "Widget '" << item->get_name()
				<< "': connected to signal_clicked(), using the slot "
				   "Candidates::selectAll.";
	}

	if(Gtk::MenuItem* item = menu->getItem<Gtk::MenuItem>("MenuInstallAction")) {
		item->signal_activate().connect(
			sigc::mem_fun(*this, &Candidates::installSelected));
		DEBUG() << "Widget '" << item->get_name()
				<< "': connected to signal_clicked(), using the slot "
				   "Candidates::installSelected.";
	}

	Gtk::Entry* patternSearchEntry = nullptr;
	utils::GetBuilderUI()->get_widget<Gtk::Entry>("EntryFind", patternSearchEntry);
	if(patternSearchEntry) {
		patternSearchEntry->signal_changed().connect([this, patternSearchEntry]() {
			/*
			 * If candidates more than _MaxCountRows_signal_changed, we have a slow
			 * performance of a filter when the signal changed emited. But, we has the
			 * Enter key, to find this pattern.
			 */
			if(_MaxCountRows_signal_changed != -1 &&
			   this->_candidates
					   .at(static_cast<package::CandidateType>(this->_currentType))
					   .size() < _MaxCountRows_signal_changed /* TODO: */)
				this->sortByPattern(patternSearchEntry->get_text());
		});
		patternSearchEntry->signal_activate().connect([this, patternSearchEntry]() {
			this->sortByPattern(patternSearchEntry->get_text());
		});
	} else {
		INFO() << "Widget 'EntryFind' not configured.";
	}

	Gtk::Box* bottomEntryBox = nullptr;
	utils::GetBuilderUI()->get_widget<Gtk::Box>("BottomEntryBox", bottomEntryBox);
	if(bottomEntryBox) {
		bottomEntryBox->signal_hide().connect([this]() { this->sortByPattern(""); });
	} else {
		INFO() << "Widget 'BottomEntryBox' not configured.";
	}
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

	waitForProgress(true);

	if(_candidates.find(type) == _candidates.end() || force) {
		// auto refresh!

		package::Cache cache;
		if(!cache.IsValid()) {
			Gtk::MessageDialog dialog("The package cache is invalid.");
			dialog.set_title("Warning!");
			dialog.run();

			waitForProgress(false);
			return;
		}

		package::ProgressAcquireStatus status(&progressPulse);
		bool ok;
		_candidates[type] = cache.getCandidates(type, ok, &progressRange, &status);

		if(!ok) {
			Gtk::MessageDialog dialog("Errors occurred while updating the cache.");
			dialog.set_title("Warning!");
			dialog.run();

			waitForProgress(false);
			return;
		}
	}

	_rows->clear();

	switch(type) {
	case package::Update: {
		get_column(0)->set_visible(true);
		break;
	}
	case package::Cached: {
		get_column(0)->set_visible(false);
		break;
	}
	}

	setModelByType(type);

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

	waitForProgress(false);

	DEBUG() << "Widget '" << get_name() << "': added new rows successfully.";

	_currentType = type;

	utils::GetCustomWidget<widget::Menu>("MainMenu")->rebuildByType(type);

	_sig_generated.emit(_candidates.at(type).size());

	if(_filterModel) {
		std::string pattern = _filterModel->CurrentPattern;

		_filterModel.reset();

		sortByPattern(pattern);
	}
}

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

	bool activeSelectAll = false;
	if(Gtk::CheckMenuItem* item =
		   utils::GetCustomWidget<widget::Menu>("MainMenu")
			   ->getItem<Gtk::CheckMenuItem>("MenuSelectAllAction")) {
		activeSelectAll = item->get_active();
	}

	for(Gtk::TreeModel::Row row : _rows->children()) {
		if(activeSelectAll /* menu item is toggled */)
			row[_rowData.Checked] = true;
		else
			row[_rowData.Checked] = false;
	}
}

void Candidates::installSelected()
{
	DEBUG() << "Widget '" << get_name() << "': Candidates::installSelected was called.";

	if(_currentType != package::Update) return;

	decltype(_candidates)::const_iterator iterAllCandidates =
		_candidates.find(static_cast<package::CandidateType>(_currentType));
	if(iterAllCandidates == _candidates.end()) {
		INFO() << "iterAllCandidates == _candidates.end(): no candidates";
		return;
	}

	const package::CandidateList& allCandidates = iterAllCandidates->second;

	waitForProgress(true);

	package::CandidateList listSelected;
	for(const Gtk::TreeModel::Row& row : _rows->children()) {
		if(!row[_rowData.Checked]) continue;

		package::CandidateList::const_iterator iterFoundCandidate =
			std::find_if(allCandidates.cbegin(),
						 allCandidates.cend(),
						 [this, row](const package::Candidate& candidate) {
							 return (row[_rowData.Name] == candidate.FullName) &&
									(row[_rowData.Version] == candidate.Version) &&
									(row[_rowData.Architecture] ==
									 candidate.Architecture) &&
									(row[_rowData.Origin] == candidate.Origin) &&
									(row[_rowData.Number] == candidate.Number) &&
									(row[_rowData.Size] == candidate.SizeKB);
						 });
		if(iterFoundCandidate != allCandidates.cend()) {
			listSelected.push_back(*iterFoundCandidate);
		}
	}

	extension::ProgressRange progressRange;
	widget::ProgressBar* progressBar =
		utils::GetWidgetDerived<widget::ProgressBar>("MainProgressBar");
	progressBar->set_fraction(0.0);

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

	DEBUG() << "Widget '" << progressBar->get_name() << "': was configured.";

	package::Cache cache;
	if(!cache.IsValid()) {
		Gtk::MessageDialog dialog("The package cache is invalid.");
		dialog.set_title("Warning!");
		dialog.run();

		waitForProgress(false);
		return;
	}

	if(!cache.installCandidates(listSelected, &progressRange)) {
		Gtk::MessageDialog dialog("Errors occurred when installing packages.");
		dialog.set_title("Error!");
		dialog.run();
	}

	waitForProgress(false);

	generate(static_cast<package::CandidateType>(_currentType), true);
}

void Candidates::waitForProgress(bool on)
{
	utils::widget::EnableWidgets(!on,
								 "ButtonOpenMenu",
								 "ButtonOpenLog",
								 "ButtonUpdate",
								 "SectionsTree",
								 "ButtonOpenSearch");
}

void Candidates::sortByPattern(const Glib::ustring& pattern)
{
	if(pattern.empty()) {
		setModelByType(static_cast<package::CandidateType>(_currentType));
		_filterModel.reset();

		return;
	}

	utils::widget::EnableWidgets(false, "CandidatesTree");

	if(_filterModel) {
		_filterModel->CurrentPattern = pattern;
		_filterModel->refilter();
	} else {
		_filterModel = ObjPtr<RowFilter>(new RowFilter(_rows, pattern));

		_filterModel->set_visible_func([this](const Gtk::TreeModel::const_iterator& row) {
			return (row->get_value(_rowData.Name)
						.lowercase()
						.find(_filterModel->CurrentPattern.lowercase()) !=
					Glib::ustring::npos) ||
				   (row->get_value(_rowData.Version)
						.lowercase()
						.find(_filterModel->CurrentPattern.lowercase()) !=
					Glib::ustring::npos) ||
				   (row->get_value(_rowData.Architecture)
						.lowercase()
						.find(_filterModel->CurrentPattern.lowercase()) !=
					Glib::ustring::npos) ||
				   (row->get_value(_rowData.Origin)
						.lowercase()
						.find(_filterModel->CurrentPattern.lowercase()) !=
					Glib::ustring::npos);
		});

		set_model(_filterModel);
	}

	utils::widget::EnableWidgets(true, "CandidatesTree");
}

void Candidates::setModelByType(package::CandidateType type)
{
	switch(type) {
	case package::Update: {
		_sortModel = ObjPtr<RowSort>(new RowSort(_rows));
		_sortModel->set_sort_column(_rowData.Name, Gtk::SORT_ASCENDING);
		set_model(_sortModel);

		break;
	}
	case package::Cached: {
		_sortModel.reset();
		set_model(_rows);

		break;
	}
	}
}

decltype(Candidates::_sig_generated) Candidates::signal_generated()
{
	return _sig_generated;
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

Candidates::RowFilter::RowFilter(const ObjPtr<Gtk::ListStore>& model,
                                 const std::string& pattern) :
    Gtk::TreeModelFilter(model),
    CurrentPattern(pattern)
{}
} // namespace widget
