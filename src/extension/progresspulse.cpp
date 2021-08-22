#include "progresspulse.h"

#include "../utils.h"

namespace extension
{
void ProgressPulse::setRange(int startVal, int endVal)
{
	(void)startVal;
	(void)endVal;
}

void ProgressPulse::increment()
{
	_sig_pulsed.emit();
}

void ProgressPulse::decrement()
{
	_sig_pulsed.emit();
}

void ProgressPulse::reset()
{}

decltype(ProgressPulse::_sig_pulsed) ProgressPulse::signal_pulsed()
{
	return _sig_pulsed;
}
} // namespace extension
