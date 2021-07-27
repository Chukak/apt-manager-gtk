#ifndef CACHE_H
#define CACHE_H

#include <apt-pkg/cachefile.h>

#include <memory>
#include <iostream>

namespace package
{
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

/**
 * @brief The CandidateList class
 * A pacakge list.
 */
class CandidateList : public std::list<Candidate>
{
	friend std::ostream& operator<<(std::ostream& stream,
									const CandidateList& candidateList);
};

/**
 * @brief The CandidateType enum
 */
enum CandidateType
{
	Installed,
	Upgradable
};

/**
 * @brief The Cache class
 */
class Cache
{
  public:
    Cache();
    /**
     * @brief IsValid
     * @return The cache correctness.
     */
    inline bool IsValid() const { return _isValid; };
    /**
     * @brief objectRef
     * @return The cache file (pkgCacheFile) object.
     */
    inline pkgCacheFile& objectRef() { return (*_cacheFile); }
    /**
     * @brief getCandidates
     * @param type Type of candidates
     * @return A list of candidates.
     */
    CandidateList getCandidates(CandidateType type);

  private:
    /**
     * @brief createCandidate
     * Creates a new candidate using PkgIterator.
     * @param packetIter
     * @param ok
     * @return A new candidate.
     */
    Candidate createCandidate(pkgCache::PkgIterator packetIter, bool& ok);

  private:
    bool _isValid{false};
    std::unique_ptr<pkgCacheFile> _cacheFile{nullptr};
};

} // namespace package

#endif // CACHE_H
