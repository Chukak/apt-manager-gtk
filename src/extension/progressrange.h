#ifndef PROGRESSRANGE_H
#define PROGRESSRANGE_H

#include "../type.h"

#include <glibmm/signalproxy.h>

namespace extension
{
class ProgressRange : public Progress
{
  public:
    void setRange(int startVal, int endVal) override;
    void increment() override;
    void decrement() override;
    bool inProgress();
    void reset() override;

  private: /* signals */
    sigc::signal<void(double /* current val */, double /* step */)> _sig_changed;
    sigc::signal<void()> _sig_finished;
    sigc::signal<
        void(double /* start val */, double /* end val */, double /* step val */)>
        _sig_rangeChanged;
    sigc::signal<void()> _sig_reseted;

  public:
    decltype(_sig_changed) signal_changed();
    decltype(_sig_finished) signal_finished();
    decltype(_sig_rangeChanged) signal_rangeChanged();
    decltype(_sig_reseted) signal_reseted();

  private:
    void check();

  private:
    double _val{0}, _step{0};
    std::atomic_bool _locked{false};
};
} // namespace extension
#endif // PROGRESSRANGE_H
