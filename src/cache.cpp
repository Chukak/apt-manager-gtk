#include "cache.h"
#include "utils.h"

#include "textacquirestatus.h"

#include <apt-pkg/error.h>
#include <apt-pkg/policy.h>
#include <apt-pkg/sourcelist.h>
#include <apt-pkg/update.h>
#include <apt-pkg/cachefile.h>

#include <sstream>

namespace package
{
Cache::Cache()
{
	utils::InitPkgConfiguration();

	_cacheFile.reset(new pkgCacheFile);

	if(!_cacheFile->GetPkgCache() || _error->PendingError()) utils::PrintPkgError();

	_isValid = true;
}

CandidateList::CandidateList(const Cache& cache, PackageFilter filterOption)
{
	switch(filterOption) {
	case PackageFilter::InstalledPackages:
		filterInstalledPkgs(cache);
		break;
	case PackageFilter::NewAvailablePackages:
		filterNewAvailablePkgs(cache);
		break;
	}
}

void CandidateList::filterInstalledPkgs(const Cache& cache)
{
	clear();

	pkgPolicy* policy = cache._cacheFile->GetPolicy();

	for(pkgCache::GrpIterator group = cache._cacheFile->GetPkgCache()->GrpBegin();
		group != cache._cacheFile->GetPkgCache()->GrpEnd();
		++group) {
		for(pkgCache::PkgIterator packet = group.PackageList(); !packet.end();
			packet = group.NextPkg(packet))
			pushCandidate(policy, packet);
	}
}

void CandidateList::filterNewAvailablePkgs(const Cache& cache)
{
	clear();

	if(!cache._cacheFile->BuildSourceList()) {
		utils::PrintPkgError();
		return;
	}

	pkgSourceList* sourceListPkgs = cache._cacheFile->GetSourceList();

	{
		std::stringstream textStreamStatus;
		TextAcquireStatus updateStatus(textStreamStatus);
		ListUpdate(updateStatus, *sourceListPkgs);
	}

	cache._cacheFile->RemoveCaches();
	if(!cache._cacheFile->BuildCaches(nullptr, false) &&
	   !cache._cacheFile->Open(nullptr, false)) {
		utils::PrintPkgError();
		return;
	}

	pkgDepCache* packetCache = cache._cacheFile->GetDepCache();
	pkgPolicy* policy = cache._cacheFile->GetPolicy();

	for(pkgCache::PkgIterator packet = packetCache->PkgBegin(); !packet.end(); ++packet) {
		pkgDepCache::StateCache& state = (*packetCache)[packet];
		if(state.Upgradable() && packet->CurrentVer) pushCandidate(policy, packet);
	}
}

void CandidateList::pushCandidate(pkgPolicy* policy, pkgCache::PkgIterator packet)
{
	pkgCache::VerIterator candidate = policy->GetCandidateVer(packet);

	if(!candidate || !candidate.FileList()) return;

	pkgCache::VerFileIterator verFileIt = candidate.FileList();

	std::string archive, origin, component, architecture;

	if(verFileIt.File().Archive()) archive = verFileIt.File().Archive();
	if(verFileIt.File().Origin()) origin = verFileIt.File().Origin();
	if(verFileIt.File().Component()) component = verFileIt.File().Component();
	if(verFileIt.File().Architecture()) architecture = verFileIt.File().Architecture();

	push_back(Candidate{packet.Index(),
						packet.FullName(),
						candidate.VerStr(),
						architecture,
						archive,
						origin,
						component});
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
