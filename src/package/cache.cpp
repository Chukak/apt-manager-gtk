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

	if(!_cacheFile->GetPkgCache()) {
		INFO() << "_cacheFile->GetPkgCache() == false";
		utils::PrintPkgError();
		return;
	}
	DEBUG() << "Successfully create a package cache: _cacheFile->GetPkgCache() == true";

	_isValid = true;
}

CandidateList
Cache::getCandidates(CandidateType type, bool& ok, Progress* pg, pkgAcquireStatus* status)
{
	CandidateList result;
	ok = true;

	switch(type) {
	case CandidateType::List_Of_Installed:
	case CandidateType::Delete: {
		break;
	}
	case CandidateType::Update:
	case CandidateType::Install: {
		DEBUG() << "Updating the package cache...";

		if(pg) {
			pg->setRange(0, 1);
			pg->reset();
		}

		if(!_cacheFile->BuildSourceList()) {
			INFO() << "_cacheFile->BuildSourceList() == false";
			utils::PrintPkgError();
			ok = false;
			break;
		}

		if(pg) pg->increment();

		pkgSourceList* sourceListPkgs = _cacheFile->GetSourceList();
		if(!sourceListPkgs) {
			INFO() << "_cacheFile->GetSourceList() == NULL;";
			ok = false;
			break;
		}

		{
			if(status) {
				ListUpdate(*status, *sourceListPkgs);
			} else {
				std::stringstream textStreamStatus;
				TextAcquireStatus updateStatus(textStreamStatus);
				ListUpdate(updateStatus, *sourceListPkgs);

				DEBUG() << textStreamStatus.str();
			}
		}

		_cacheFile->RemoveCaches();
		if(!_cacheFile->BuildCaches(nullptr, false) &&
		   !_cacheFile->Open(nullptr, false)) {
			INFO() << "_cacheFile->BuildCaches(...) == false; _cacheFile->Open(...) == "
					  "false";
			utils::PrintPkgError();
			ok = false;
			break;
		}

		break;
	}
	}

	createCandidatesByType(result, type, ok, pg);

	DEBUG() << "Successfully generated all the packages (count: " << result.size()
			<< ").";

	return result;
}

void Cache::createCandidatesByType(CandidateList& list,
                                   CandidateType type,
                                   bool& ok,
                                   Progress* pg)
{
	pkgCache* const packetCache = _cacheFile->GetPkgCache();
	if(!packetCache) {
		INFO() << "_cacheFile->GetPkgCache() == NULL;";
		utils::PrintPkgError();
		ok = false;
		return;
	}

	pkgDepCache* packetDepCache = _cacheFile->GetDepCache();
	if(!packetDepCache) {
		INFO() << "_cacheFile->GetDepCache() == NULL;";
		utils::PrintPkgError();
		ok = false;
		return;
	}

	switch(type) {
	case CandidateType::Update:
	case CandidateType::Install: {
		if(!_cacheFile->GetPolicy()) {
			INFO() << "_cacheFile->GetPolicy() == NULL";
			utils::PrintPkgError();
			ok = false;
			return;
		}
	}
	default: {
		break;
	}
	}

	DEBUG() << "Successfully updated the package cache.";

	if(pg) {
		pg->setRange(0, packetCache->Head().PackageCount);
		pg->reset();
	}

	for(pkgCache::PkgIterator packet = packetCache->PkgBegin(); !packet.end(); ++packet) {
		pkgDepCache::StateCache& state = (*packetDepCache)[packet];
		pkgCache::VerIterator verIter = packet.CurrentVer();

		if(pg) pg->increment();

		if((!verIter && type == CandidateType::Install) ||
		   (type == CandidateType::Update)) {
			verIter = _cacheFile->GetPolicy()->GetCandidateVer(packet);
		}

		if(!verIter) continue;

		pkgCache::PkgIterator currentPkgIter = verIter.ParentPkg();

		if(((type == CandidateType::List_Of_Installed || type == CandidateType::Delete) &&
			(currentPkgIter.CurrentVer() == verIter ||
			 (state.CandidateVer == verIter && state.Upgradable()))) ||
		   (type == CandidateType::Update && (state.Upgradable() && state.InstallVer)) ||
		   (type == CandidateType::Install && !state.InstallVer)) {
			bool okCreate = false;
			Candidate newCandidate = createCandidate(verIter, okCreate);
			if(okCreate) list.push_back(newCandidate);
		}
	}
}

bool Cache::processCandidates(const CandidateList& list, CandidateType type, Progress* pg)
{
	if(pg) pg->setRange(0, 7);

	if(!RunScripts("APT::Install::Pre-Invoke")) {
		INFO() << "RunScripts(\"APT::Install::Pre-Invoke\") == false";
		utils::PrintPkgError();
		return false;
	}

	if(pg) pg->increment();

	if(!_cacheFile->Open(nullptr, false)) {
		INFO() << "_cacheFile->Open(...) == false";
		utils::PrintPkgError();
		return false;
	}

	if(pg) pg->increment();

	pkgDepCache* depCache = _cacheFile->GetDepCache();

	if(depCache->DelCount() != 0 || depCache->InstCount() != 0) {
		INFO() << "depCache->DelCount() == 0; depCache->InstCount() == 0";
		utils::PrintPkgError();
		return false;
	}

	if(pg) pg->increment();

	pkgPolicy* policy = _cacheFile->GetPolicy();

	size_t markProcessed = 0;
	for(pkgCache::PkgIterator pkg = depCache->PkgBegin(); !pkg.end(); ++pkg) {
		CandidateList::const_iterator found =
			std::find_if(list.cbegin(),
						 list.cend(),
						 [&pkg, policy](const Candidate& candidate) {
							 return candidate.isEquals(policy, pkg);
						 });

		if(found != list.cend()) {
			switch(type) {
			case CandidateType::Update:
			case CandidateType::Install: {
				if(!depCache->MarkInstall(pkg, true, 0, false, true)) {
					INFO() << "depCache->MarkInstall(...) == false";
					utils::PrintPkgError();
				} else
					++markProcessed;

				break;
			}
			case CandidateType::Delete: {
				if(!depCache->MarkDelete(pkg, true, 0, true)) {
					INFO() << "depCache->MarkDelete(...) == false";
					utils::PrintPkgError();
				} else
					++markProcessed;
			}
			default: {
				break;
			}
			}
		}
	}
	DEBUG() << markProcessed << " packages are will processed.";

	if(pg) pg->increment();

	if(!_cacheFile->BuildSourceList()) {
		INFO() << "_cacheFile->BuildSourceList() == false";
		utils::PrintPkgError();
		return false;
	}

	if(pg) pg->increment();

	std::unique_ptr<pkgPackageManager> manager(utils::GetPkgSystem().CreatePM(depCache));
	pkgSourceList* const sourceList = _cacheFile->GetSourceList();
	pkgRecords records(*depCache);
	pkgAcquire managerAcq;

	if(!manager->GetArchives(&managerAcq, sourceList, &records)) {
		INFO() << "manager->GetArchives(...) == false";
		utils::PrintPkgError();
		return false;
	}

	if(pg) pg->increment();

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

		if(pg) {
			pg->increment();
			pg->setRange(0,
						 static_cast<int>(managerAcq.ItemsEnd() -
										  managerAcq.ItemsBegin()));
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

	if(pg) pg->setRange(0, static_cast<int>(markProcessed));

	DEBUG() << "Redirecting stdout and stderr to temporarily file.\n";
	utils::RedirectLogOutputToTemporaryFile(true);

	switch(manager->DoInstall(managerProgress)) {
	case pkgPackageManager::Completed: {
	case pkgPackageManager::Incomplete:
	case pkgPackageManager::Failed:
		if(_error->PendingError()) {
			utils::PrintPkgError();
		}

		if(pg) pg->increment();

		break;
	}
	}

	utils::RedirectLogOutputToTemporaryFile(false);
	utils::GetLog() << "\n";

	managerAcq.Shutdown();
	if(!manager->GetArchives(&managerAcq, sourceList, &records)) {
		INFO() << "manager->GetArchives(...) == false";
		utils::PrintPkgError();
		return false;
	}

	delete managerProgress;

	if(!RunScripts("APT::Install::Post-Invoke-Success")) {
		INFO() << "RunScripts(\"APT::Install::Post-Invoke-Success\") == false";
		utils::PrintPkgError();
		return false;
	}

	return true;
}

Candidate Cache::createCandidate(pkgCache::VerIterator verIter, bool& ok)
{
	ok = true;
	Candidate newCandidate;

	if(!verIter.FileList()) {
		ok = false;
		return newCandidate;
	}

	pkgCache::PkgIterator packetIter = verIter.ParentPkg();

	newCandidate.Number = packetIter.Index();
	newCandidate.FullName = packetIter.FullName();
	newCandidate.Version = verIter.VerStr();

	pkgCache::VerFileIterator verFileIt = verIter.FileList();
	std::string archive, origin, component, architecture;

	if(verFileIt.File().Archive()) newCandidate.Archive = verFileIt.File().Archive();
	if(verFileIt.File().Origin()) newCandidate.Origin = verFileIt.File().Origin();
	if(verFileIt.File().Component())
		newCandidate.Component = verFileIt.File().Component();
	if(verFileIt.File().Architecture())
		newCandidate.Architecture = verFileIt.File().Architecture();

	newCandidate.SizeKB = SizeToStr(static_cast<double>(verIter->Size));
	newCandidate.SizeKB += "B";

	return newCandidate;
}

std::ostream& operator<<(std::ostream& stream, const Candidate& candidate)
{
	stream << "{\n  "
		   << "FullName: " << candidate.FullName << "\n  "
		   << "Version: " << candidate.Version << "\n  "
		   << "Architecture: " << candidate.Architecture << "\n  "
		   << "Archive: " << candidate.Archive << "\n  "
		   << "Origin: " << candidate.Origin << "\n  "
		   << "Component: " << candidate.Component << "\n }";
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const CandidateList& candidateList)
{
	stream << "[\n";

	for(const Candidate& candidate : candidateList) {
		stream << " ";
		stream << candidate;
		stream << "\n";
	}

	stream << "]" << std::endl;
	return stream;
}

} // namespace package
