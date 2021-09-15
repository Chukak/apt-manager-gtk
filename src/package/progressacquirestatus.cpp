#include "progressacquirestatus.h"

#include <apt-pkg/acquire-item.h>
#include <apt-pkg/strutl.h>
#include <sstream>

namespace package
{
ProgressAcquireStatus::ProgressAcquireStatus(Progress *pg) : _pg(pg)
{}

void ProgressAcquireStatus::Start()
{
	pkgAcquireStatus::Start();

	if(_pg) _pg->increment();
}

void ProgressAcquireStatus::Stop()
{
	pkgAcquireStatus::Stop();

	if(_pg) _pg->increment();
}

void ProgressAcquireStatus::IMSHit(pkgAcquire::ItemDesc &item)
{
	(void)item;
	if(_pg) _pg->increment();
}

void ProgressAcquireStatus::Fetch(pkgAcquire::ItemDesc &item)
{
	if(_pg) _pg->increment();

	if(item.Owner->Complete) return;
}

void ProgressAcquireStatus::Done(pkgAcquire::ItemDesc &item)
{
	(void)item;
	if(_pg) _pg->increment();
}

void ProgressAcquireStatus::Fail(pkgAcquire::ItemDesc &item)
{
	switch(item.Owner->Status) {
	case pkgAcquire::Item::StatDone:
	case pkgAcquire::Item::StatIdle: {
		break;
	}
	default: {
		if(!item.Owner->ErrorText.empty()) {
			std::stringstream errorText;
			errorText << "Error: (" << item.Owner->ID << "): " << item.Owner->ErrorText
					  << "\n";
			_errors.push_back(errorText.str());
		}
		break;
	}
	}
}

bool ProgressAcquireStatus::Pulse(pkgAcquire *owner)
{
	(void)owner;
	return true;
}

bool ProgressAcquireStatus::MediaChange(std::string media, std::string drive)
{
	(void)media;
	(void)drive;
	return true;
}

} // namespace package
