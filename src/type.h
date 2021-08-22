#ifndef TYPE_H
#define TYPE_H

#include <glibmm/refptr.h>
#include <atomic>

template<typename TYPE>
using ObjPtr = Glib::RefPtr<TYPE>;

enum Orientation
{
	Horizontal,
	Vertical
};

class Progress
{
  public:
    virtual void setRange(int, int) = 0;
    virtual void increment() = 0;
    virtual void decrement() = 0;
    virtual void reset() = 0;

  protected:
    const double Start{0.0}, End{1.0};
};

#define ATOMIC_WAIT_FOR(                                                                 \
	ATOMIC_VAR, WAIT_VALUE, SET_VALUE_BEFORE_CODE, SET_VALUE_AFTER_CODE, CODE)           \
	{                                                                                    \
		decltype(ATOMIC_VAR.load()) __w = WAIT_VALUE;                                    \
		while(!ATOMIC_VAR.compare_exchange_weak(__w, SET_VALUE_BEFORE_CODE))             \
			;                                                                            \
		{CODE} /* code */                                                                \
		ATOMIC_VAR.store(SET_VALUE_AFTER_CODE);                                          \
	}

#endif // TYPE_H
