#include "sections.h"

#include "../package/cache.h"
#include "../utils.h"

namespace widget
{
Sections::Sections(BaseObjectType* cobject, const ObjPtr<Gtk::Builder>& refBuilder) :
	Gtk::TreeView(cobject), _rows(Gtk::ListStore::create(_rowData))
{
	(void)refBuilder;

	set_model(_rows);

	append_column("Packages", _rowData.Title);

	for(int32_t t = package::FirstType; t <= package::LastType; ++t) {
		Gtk::TreeModel::Row row = *(_rows->append());

		switch(t) {
		case package::Installed: {
			row[_rowData.Title] = "installed";
			break;
		}
		case package::Upgradable: {
			row[_rowData.Title] = "Upgradable";
			break;
		}
		}
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
}
} // namespace widget
