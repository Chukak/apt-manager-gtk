#include "progressrange.h"

namespace extension
{
void ProgressRange::setRange(int startVal, int endVal)
{
	_step = 1.0 / static_cast<double>(endVal - startVal);

	ATOMIC_WAIT_FOR(_locked, false, true, false, {
		_sig_rangeChanged.emit(Start, End, _step);
	});
}

bool ProgressRange::inProgress()
{
	bool prog{false};
	ATOMIC_WAIT_FOR(_locked, false, true, false, { prog = _val - End < 0; });
	return prog;
}

void ProgressRange::reset()
{
	ATOMIC_WAIT_FOR(_locked, false, true, false, {
		_val = Start;
		_sig_reseted.emit();
	});
}

void ProgressRange::increment()
{
	ATOMIC_WAIT_FOR(_locked, false, true, false, {
		_val += _step;
		check();
	});
}

void ProgressRange::decrement()
{
	ATOMIC_WAIT_FOR(_locked, false, true, false, {
		_val -= _step;
		check();
	});
}

decltype(ProgressRange::_sig_changed) ProgressRange::signal_changed()
{
	return _sig_changed;
}

decltype(ProgressRange::_sig_finished) ProgressRange::signal_finished()
{
	return _sig_finished;
}

decltype(ProgressRange::_sig_rangeChanged) ProgressRange::signal_rangeChanged()
{
	return _sig_rangeChanged;
}

decltype(ProgressRange::_sig_reseted) ProgressRange::signal_reseted()
{
	return _sig_reseted;
}

void ProgressRange::check()
{
	if(_val <= End) {
		if(_val < Start) _val = Start;

		_sig_changed.emit(_val, _step);
	}

	if(_val - End >= 0) {
		_sig_finished.emit();
	}
}
} // namespace extension
