#ifndef CACHE_H
#define CACHE_H

#include <apt-pkg/cachefile.h>

#include <memory>
#include <iostream>

namespace package
{
class Cache
{
    friend class CandidateList;

  public:
    Cache();
    /**
     * @brief IsValid
     * @return The cache correctness.
     */
    inline bool IsValid() const { return _isValid; };

  private:
    bool _isValid{false};
    std::unique_ptr<pkgCacheFile> _cacheFile{nullptr};
};

/**
 * @brief The Candidate struct
 * Describes a package.
 */
struct Candidate
{
	size_t Number;        //! The number of candidate
	std::string FullName, //! Candidate Full name
		Version,          //! Candidate Version
		Architecture,     //! Architecture
		Archive,          //! Archive name
		Origin,           //! Origin
		Component;        //!
};

enum PackageFilter
{
	InstalledPackages,
	NewAvailablePackages
};

/**
 * @brief The CandidateList class
 * A pacakge list.
 */
class CandidateList : public std::list<Candidate>
{
	friend std::ostream& operator<<(std::ostream& stream,
									const CandidateList& candidateList);

  public:
    CandidateList(const Cache& cache, PackageFilter filterOption);

  private:
    void filterInstalledPkgs(const Cache& cache);
    void filterNewAvailablePkgs(const Cache& cache);
    void pushCandidate(pkgPolicy* policy, pkgCache::PkgIterator packet);
};

} // namespace package

#endif // CACHE_H
