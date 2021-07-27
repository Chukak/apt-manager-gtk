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

CandidateList Cache::getCandidates(CandidateType type)
{
	CandidateList result;

	switch(type) {
	case CandidateType::Installed: {
		for(pkgCache::GrpIterator group = _cacheFile->GetPkgCache()->GrpBegin();
			group != _cacheFile->GetPkgCache()->GrpEnd();
			++group) {
			for(pkgCache::PkgIterator packet = group.PackageList(); !packet.end();
				packet = group.NextPkg(packet)) {
				bool ok;
				Candidate newCandidate = createCandidate(packet, ok);
				if(ok) result.push_back(newCandidate);
			}
		}
		break;
	}
	case CandidateType::Upgradable: {
		if(!_cacheFile->BuildSourceList()) {
			utils::PrintPkgError();
			break;
		}

		pkgSourceList* sourceListPkgs = _cacheFile->GetSourceList();
		{
			std::stringstream textStreamStatus;
			TextAcquireStatus updateStatus(textStreamStatus);
			ListUpdate(updateStatus, *sourceListPkgs);
		}

		_cacheFile->RemoveCaches();
		if(!_cacheFile->BuildCaches(nullptr, false) &&
		   !_cacheFile->Open(nullptr, false)) {
			utils::PrintPkgError();
			break;
		}

		pkgDepCache* packetCache = _cacheFile->GetDepCache();

		for(pkgCache::PkgIterator packet = packetCache->PkgBegin(); !packet.end();
			++packet) {
			pkgDepCache::StateCache& state = (*packetCache)[packet];
			if(state.Upgradable() && packet->CurrentVer) {
				bool ok;
				Candidate newCandidate = createCandidate(packet, ok);
				if(ok) result.push_back(newCandidate);
			}
		}
		break;
	}
	}

	return result;
}

Candidate Cache::createCandidate(pkgCache::PkgIterator packetIter, bool& ok)
{
	Candidate newCandidate;

	pkgCache::VerIterator candidate =
		_cacheFile->GetPolicy()->GetCandidateVer(packetIter);

	if(!candidate || !candidate.FileList()) {
		ok = false;
		return newCandidate;
	}

	newCandidate.Number = packetIter.Index();
	newCandidate.FullName = packetIter.FullName();

	pkgCache::VerFileIterator verFileIt = candidate.FileList();
	std::string archive, origin, component, architecture;

	if(verFileIt.File().Archive()) newCandidate.Archive = verFileIt.File().Archive();
	if(verFileIt.File().Origin()) newCandidate.Origin = verFileIt.File().Origin();
	if(verFileIt.File().Component())
		newCandidate.Component = verFileIt.File().Component();
	if(verFileIt.File().Architecture())
		newCandidate.Architecture = verFileIt.File().Architecture();

	ok = true;
	return newCandidate;
}

std::ostream& operator<<(std::ostream& stream, const CandidateList& candidateList)
{
	stream << "[\n";

	for(const Candidate& candidate : candidateList)
		stream << " {\n  "
			   << "FullName: " << candidate.FullName << "\n  "
			   << "Version: " << candidate.Version << "\n  "
			   << "Architecture: " << candidate.Architecture << "\n  "
			   << "Archive: " << candidate.Archive << "\n  "
			   << "Origin: " << candidate.Origin << "\n  "
			   << "Component: " << candidate.Component << "\n }\n";

	stream << "]" << std::endl;
	return stream;
}

} // namespace package
