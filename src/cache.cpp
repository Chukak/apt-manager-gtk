#include "cache.h"
#include "utils.h"

#include <apt-pkg/error.h>
#include <apt-pkg/policy.h>

namespace package
{
Cache::Cache()
{
	utils::InitPkgConfiguration();

	_cacheFile.reset(new pkgCacheFile);

	if(!_cacheFile->GetPkgCache() || _error->PendingError()) utils::PrintPkgError();

	_isValid = true;
}

CandidateList::CandidateList(const Cache& cache)
{
	pkgPolicy* policy = cache._cacheFile->GetPolicy();

	size_t index = 1;
	for(pkgCache::GrpIterator group = cache._cacheFile->GetPkgCache()->GrpBegin();
		group != cache._cacheFile->GetPkgCache()->GrpEnd();
		++group) {
		for(pkgCache::PkgIterator packet = group.PackageList(); !packet.end();
			packet = group.NextPkg(packet)) {
			pkgCache::VerIterator candidate = policy->GetCandidateVer(packet);
			if(!candidate || !candidate.FileList()) continue;

			pkgCache::VerFileIterator verFileIt = candidate.FileList();

			std::string archive, origin, component, architecture;

			if(verFileIt.File().Archive()) archive = verFileIt.File().Archive();
			if(verFileIt.File().Origin()) origin = verFileIt.File().Origin();
			if(verFileIt.File().Component()) component = verFileIt.File().Component();
			if(verFileIt.File().Architecture())
				architecture = verFileIt.File().Architecture();

			push_back(Candidate{index,
								packet.FullName(),
								candidate.VerStr(),
								architecture,
								archive,
								origin,
								component});
		}
	}
}

std::ostream& operator<<(std::ostream& stream, const CandidateList& candidateList)
{
	stream << std::string(26, '*') << std::endl;

	for(const Candidate& candidate : candidateList)
		stream << "\n\t"
			   << "FullName: " << candidate.FullName << "\n\t"
			   << "Version: " << candidate.Version << "\n\t"
			   << "Architecture: " << candidate.Architecture << "\n\t"
			   << "Archive: " << candidate.Archive << "\n\t"
			   << "Origin: " << candidate.Origin << "\n\t"
			   << "Component: " << candidate.Component << "\n";

	stream << std::string(26, '*') << std::endl;
	return stream;
}

} // namespace package
