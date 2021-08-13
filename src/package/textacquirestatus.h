#ifndef TEXTACQUIRESTATUS_H
#define TEXTACQUIRESTATUS_H

#include <apt-pkg/acquire.h>

namespace package
{
using ErrorStringList = std::vector<std::string>;

class TextAcquireStatus : public pkgAcquireStatus
{
  public:
    TextAcquireStatus(std::ostream &outStream);
    /**
     * @brief IsAnyUpdates
     * @return If any package can be updated.
     */
    inline bool IsAnyUpdates() const { return _isAnyUpd; };
    /**
     * @brief IsAllUpdates
     * @return If all pacakges can be updated.
     */
    inline bool IsAllUpdates() const { return _isAllUpd; };
    /**
     * @brief IsStarted
     * @return If download pacakge information started.
     */
    inline bool IsStarted() const { return !_downloadComplete; }
    /**
     * @brief IsFinished
     * @return If finished.
     */
    inline bool IsFinished() const { return _downloadComplete; };
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
    std::ostream &_outStream;
    bool _isAnyUpd{false}, _isAllUpd{false}, _downloadComplete{false};
    ErrorStringList _errors;
};

} // namespace package
#endif // TEXTACQUIRESTATUS_H
