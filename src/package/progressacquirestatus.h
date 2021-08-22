#ifndef PROGRESSACQUIRESTATUS_H
#define PROGRESSACQUIRESTATUS_H

#include <apt-pkg/acquire.h>

#include "../type.h"

namespace package
{
using ErrorStringList = std::vector<std::string>;

class ProgressAcquireStatus : public pkgAcquireStatus
{
  public:
    ProgressAcquireStatus(Progress *pg);
    /**
     * @brief Errors
     * @return Errors after dowloading.
     */
    inline ErrorStringList Errors() const { return _errors; };

	void Start() override;
	void Stop() override;
	void IMSHit(pkgAcquire::ItemDesc &item) override;
	void Fetch(pkgAcquire::ItemDesc &item) override;
	void Done(pkgAcquire::ItemDesc &item) override;
	void Fail(pkgAcquire::ItemDesc &item) override;
	bool Pulse(pkgAcquire *owner) override;
	bool MediaChange(std::string media, std::string drive) final;

  private:
    ErrorStringList _errors;
    Progress *_pg;
};

} // namespace package

#endif // PROGRESSACQUIRESTATUS_H
