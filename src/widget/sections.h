#ifndef SECTIONS_H
#define SECTIONS_H

#include "../type.h"

#include <gtkmm/treeview.h>
#include <gtkmm/builder.h>
#include <gtkmm/liststore.h>

namespace widget
{
class Sections : public Gtk::TreeView
{
  public:
    Sections(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder);
    virtual ~Sections() = default;

  protected:
    void on_row_activated(const Gtk::TreeModel::Path& path,
                          Gtk::TreeViewColumn* column) override;

  private:
    class RowType : public Gtk::TreeModel::ColumnRecord
    {
      public:
        RowType();

	  public:
		Gtk::TreeModelColumn<Glib::ustring> Title;
	};

  private:
    RowType _rowData;
    ObjPtr<Gtk::ListStore> _rows;
};

} // namespace widget

#endif // SECTIONS_H
