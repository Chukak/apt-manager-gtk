#ifndef TYPE_H
#define TYPE_H

#include <glibmm/refptr.h>

template<typename TYPE>
using ObjPtr = Glib::RefPtr<TYPE>;

enum Orientation
{
	Horizontal,
	Vertical
};

#endif // TYPE_H
