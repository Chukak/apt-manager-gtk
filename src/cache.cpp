#include "cache.h"
#include "utils.h"

#include "textacquirestatus.h"

#include <apt-pkg/error.h>
#include <apt-pkg/policy.h>
#include <apt-pkg/sourcelist.h>
#include <apt-pkg/update.h>
#include <apt-pkg/cachefile.h>
#include <apt-pkg/depcache.h>
#include <apt-pkg/packagemanager.h>
#include <apt-pkg/install-progress.h>
#include <apt-pkg/pkgrecords.h>
#include <apt-pkg/acquire-item.h>
#include <apt-pkg/fileutl.h>
#include <apt-pkg/strutl.h>
#include <apt-pkg/pkgsystem.h>
#include <algorithm>

#include <sstream>

namespace package
{
bool Candidate::operator==(const Candidate& rhs)
{
	return Number == rhs.Number && FullName == rhs.FullName && Version == rhs.Version &&
		   Architecture == rhs.Architecture && Archive == rhs.Archive &&
		   Origin == rhs.Origin && Component == rhs.Component;
}

bool Candidate::isEquals(pkgPolicy* policy, pkgCache::PkgIterator iter) const
{
	if(!iter || !policy || iter.end()) return false;

	if(iter.FullName() != FullName || iter.Index() != Number) return false;

	pkgCache::VerIterator verIter = policy->GetCandidateVer(iter);
	if(!verIter || !verIter.FileList()) return false;

	if(!verIter.FileList().File().Archive() ||
	   (verIter.FileList().File().Archive() != Archive))
		return false;

	if(!verIter.FileList().File().Origin() ||
	   (verIter.FileList().File().Origin() != Origin))
		return false;

	if(!verIter.FileList().File().Component() ||
	   (verIter.FileList().File().Component() != Component))
		return false;

	if(!verIter.FileList().File().Architecture() ||
	   (verIter.FileList().File().Architecture() != Architecture))
		return false;

	return true;
}

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

bool Cache::installCandidates(const CandidateList& list)
{
	if(!RunScripts("APT::Install::Pre-Invoke")) {
		utils::PrintPkgError();
		return false;
	}

	if(!_cacheFile->Open(nullptr, false)) {
		utils::PrintPkgError();
		return false;
	}

	pkgDepCache* depCache = _cacheFile->GetDepCache();

	if(depCache->DelCount() != 0 || depCache->InstCount() != 0) {
		utils::PrintPkgError();
		return false;
	}

	pkgPolicy* policy = _cacheFile->GetPolicy();

	for(pkgCache::PkgIterator pkg = depCache->PkgBegin(); !pkg.end(); ++pkg) {
		CandidateList::const_iterator found =
			std::find_if(list.cbegin(),
						 list.cend(),
						 [&pkg, policy](const Candidate& candidate) {
							 return candidate.isEquals(policy, pkg);
						 });

		if(found != list.cend()) {
			depCache->MarkInstall(pkg, true, 0, false, true);
		}
	}
	if(!_cacheFile->BuildSourceList()) {
		utils::PrintPkgError();
		return false;
	}

	std::unique_ptr<pkgPackageManager> manager(_system->CreatePM(depCache));
	pkgSourceList* const sourceList = _cacheFile->GetSourceList();
	pkgRecords records(*depCache);
	pkgAcquire managerAcq;

	if(!manager->GetArchives(&managerAcq, sourceList, &records)) {
		utils::PrintPkgError();
		return false;
	}

	bool acquireRunSuccess(true);
	{
		switch(managerAcq.Run()) {
		case pkgAcquire::Failed: {
			utils::PrintPkgError();
			return false;
		default:
			break;
		}
		}

		for(pkgAcquire::ItemIterator iter = managerAcq.ItemsBegin();
			iter != managerAcq.ItemsEnd();
			++iter) {
			if((*iter)->Status == pkgAcquire::Item::StatDone &&
			   (*iter)->Complete == true) {
				continue;
			}

			::URI uri((*iter)->DescURI());
			uri.User.clear();
			uri.Password.clear();

			_error->Error("Failed to fetch: %s %s",
						  std::string(uri).c_str(),
						  (*iter)->ErrorText.c_str());
		}
	}

	if(!acquireRunSuccess) {
		utils::PrintPkgError();
		return false;
	}

	if(!manager->FixMissing()) {
		_error->Error("Unable to correct missing packages.");
		utils::PrintPkgError();
		return false;
	}

	APT::Progress::PackageManager* managerProgress =
		APT::Progress::PackageManagerProgressFactory();

	switch(manager->DoInstall(managerProgress)) {
	case pkgPackageManager::Completed: {
		break;
	case pkgPackageManager::Incomplete:
	case pkgPackageManager::Failed:
		utils::PrintPkgError();
		break;
	}
	}

	managerAcq.Shutdown();
	if(!manager->GetArchives(&managerAcq, sourceList, &records)) {
		utils::PrintPkgError();
		return false;
	}

	delete managerProgress;

	if(!RunScripts("APT::Install::Post-Invoke-Success")) {
		utils::PrintPkgError();
		return false;
	}

	return true;
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
