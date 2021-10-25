#ifndef CANDIDATES_H
#define CANDIDATES_H

#include "../type.h"
#include "../package/cache.h"

#include <gtkmm/treeview.h>
#include <gtkmm/builder.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treemodelsort.h>
#include <gtkmm/treemodelfilter.h>

namespace widget
{
class Candidates : public Gtk::TreeView
{
  public:
    Candidates(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder);
    virtual ~Candidates() = default;

	void generate(package::CandidateType type, bool force = false);
	void sortByPattern(const Glib::ustring& pattern);

  private:
    sigc::signal<void(size_t)> _sig_generated;

  public:
    decltype(_sig_generated) signal_generated();

  private:
    void setRowStyle(Gtk::TreeModel::Row row);
    void setColumnRender(Gtk::TreeViewColumn* column, Gtk::CellRenderer* render);
    void refreshActual();
    void onToggleColumn(const Glib::ustring& path);
    void selectAll();
    void processSelected();
    void waitForProgress(bool on);
    void setModelByType(package::CandidateType type);

  private:
    class RowType : public Gtk::TreeModel::ColumnRecord
    {
      public:
        RowType();

	  public:
		Gtk::TreeModelColumn<bool> Checked;
		Gtk::TreeModelColumn<Glib::ustring> Name, Version, Architecture, Origin, Size;
		Gtk::TreeModelColumn<size_t> Number;

		Gtk::TreeModelColumn<Gdk::RGBA> BackgroundColor, ForegroundColor;
		Gtk::TreeModelColumn<Pango::FontDescription> Font;
		Gtk::TreeModelColumn<float> Align;
	};

	class RowSort : public Gtk::TreeModelSort
	{
	  public:
		RowSort(const ObjPtr<Gtk::ListStore>& model);
	};

	class RowFilter : public Gtk::TreeModelFilter
	{
	  public:
		RowFilter(const ObjPtr<Gtk::ListStore>& model, const std::string& pattern = "");

		Glib::ustring CurrentPattern;
	};

  private:
    RowType _rowData;
    ObjPtr<Gtk::ListStore> _rows;
    std::map<package::CandidateType, package::CandidateList> _candidates;
    int32_t _currentType{-1};
    ObjPtr<RowSort> _sortModel;
    ObjPtr<RowFilter> _filterModel;
};
} // namespace widget

#endif // CANDIDATES_H
