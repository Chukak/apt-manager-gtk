#ifndef PROGRESSPULSE_H
#define PROGRESSPULSE_H

#include "../type.h"

#include <glibmm/signalproxy.h>

namespace extension
{
class ProgressPulse : public Progress
{
  public:
    void setRange(int startVal, int endVal) override;
    void increment() override;
    void decrement() override;
    void reset() override;

  private:
    sigc::signal<void()> _sig_pulsed;

  public:
    decltype(_sig_pulsed) signal_pulsed();
};
} // namespace extension

#endif // PROGRESSPULSE_H
