#ifndef CACHE_H
#define CACHE_H

#include "../type.h"

#include <apt-pkg/cachefile.h>
#include <apt-pkg/acquire.h>

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
	friend std::ostream& operator<<(std::ostream& stream, const Candidate& candidate);

	size_t Number;        //! The number of candidate
	std::string FullName, //! Candidate Full name
		Version,          //! Candidate Version
		Architecture,     //! Architecture
		Archive,          //! Archive name
		Origin,           //! Origin
		SizeKB,           //! Size in kilobytes
		Component;        //!

	bool operator==(const Candidate& rhs);
	/**
	 * @brief isEquals
	 * @param policy A policy object
	 * @param iter Iterator to the candidate
	 * @return Equality of two candidates.
	 */
	bool isEquals(pkgPolicy* policy, pkgCache::PkgIterator iter) const;
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
enum CandidateType : int32_t
{
	List_Of_Installed = 1,
	Update = 2,
	Install = 3,
	Delete = 4,
	FirstType = List_Of_Installed,
	LastType = Delete
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
     * Generates candidates by type. If the pointer to the progress is not null, updates
     * the progress value.
     * @param type Type of candidates
     * @param ok Result of all function operation
     * @param progress The pointer to the Progress object.
     * @param status The status of getting candidate.
     * @return A list of candidates.
     */
    CandidateList getCandidates(CandidateType type,
                                bool& ok,
                                Progress* progress = nullptr,
                                pkgAcquireStatus* status = nullptr);
    /**
     * @brief installCandidates
     * Processes candidates from the passed list.
     * Type value:
     *  * CandidateType::Update - install all the newest versions of candidates.
     *  * CandidateType::Install - install all the candidates.
     *  * CandidateType::Delete - remove all the candidates.
     * @param list List of candidates
     * @param type Type
     * @param progress The pointer to the Progress object.
     * @return Result of installation.
     */
    bool processCandidates(const CandidateList& list,
                           CandidateType type,
                           Progress* progress = nullptr);

  private:
    /**
     * @brief createCandidate
     * Creates a new candidate using VerIterator.
     * @param verIter
     * @param ok
     * @return A new candidate.
     */
    Candidate createCandidate(pkgCache::VerIterator verIter, bool& ok);
    /**
     * @brief createCandidatesByType
     * Creates a new candidate using a type and appends it to the list.
     * @param list
     * @param type
     * @param ok
     * @param progress
     */
    void createCandidatesByType(CandidateList& list,
                                CandidateType type,
                                bool& ok,
                                Progress* progress = nullptr);

  private:
    bool _isValid{false};
    std::unique_ptr<pkgCacheFile> _cacheFile{nullptr};
};

} // namespace package

#endif // CACHE_H
