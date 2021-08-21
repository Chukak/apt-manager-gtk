#ifndef CANDIDATES_H
#define CANDIDATES_H

#include "../type.h"
#include "../package/cache.h"

#include <gtkmm/treeview.h>
#include <gtkmm/builder.h>
#include <gtkmm/liststore.h>

namespace widget
{
class Candidates : public Gtk::TreeView
{
  public:
    Candidates(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder);
    virtual ~Candidates() = default;

    void generate(package::CandidateType type, bool force = false);

  private:
    template<typename TYPE>
    Gtk::TreeViewColumn* createColumn(Gtk::TreeModelColumn<TYPE>& colText);

	void setRowStyle(Gtk::TreeModel::Row row);
	void refreshActual();

  private:
    class RowType : public Gtk::TreeModel::ColumnRecord
    {
      public:
        RowType();

	  public:
		Gtk::TreeModelColumn<bool> Checked;
		Gtk::TreeModelColumn<Glib::ustring> Name;
		Gtk::TreeModelColumn<Glib::ustring> Version;
		Gtk::TreeModelColumn<Glib::ustring> Architecture;
		Gtk::TreeModelColumn<size_t> Number;
		Gtk::TreeModelColumn<Gdk::RGBA> BackgroundColor;
		Gtk::TreeModelColumn<Gdk::RGBA> ForegroundColor;
		Gtk::TreeModelColumn<Pango::FontDescription> Font;
	};

  private:
    RowType _rowData;
    ObjPtr<Gtk::ListStore> _rows;
    std::map<package::CandidateType, package::CandidateList> _candidates;
    int32_t _currentType{-1};
};

template<typename TYPE>
Gtk::TreeViewColumn* Candidates::createColumn(Gtk::TreeModelColumn<TYPE>& colText)
{
	Gtk::CellRendererText* render = Gtk::manage(new Gtk::CellRendererText);

	if(colText == _rowData.Architecture) {
		render->set_fixed_size(10, 48);
	} else if(colText == _rowData.Name) {
		render->set_fixed_size(-1, 48);
	}

	Gtk::TreeViewColumn* column = Gtk::manage(new Gtk::TreeViewColumn("", *render));
	column->set_alignment(0.5);

	column->add_attribute(render->property_background_rgba(), _rowData.BackgroundColor);
	column->add_attribute(render->property_foreground_rgba(), _rowData.ForegroundColor);
	column->add_attribute(render->property_text(), colText);
	column->add_attribute(render->property_font_desc(), _rowData.Font);

	return column;
}
} // namespace widget

#endif // CANDIDATES_H
