#include "textacquirestatus.h"

#include <sstream>
#include <apt-pkg/acquire-item.h>
#include <apt-pkg/strutl.h>

namespace package
{
TextAcquireStatus::TextAcquireStatus(std::ostream &outStream) : _outStream(outStream)
{}

void TextAcquireStatus::Start()
{
	pkgAcquireStatus::Start();

	_isAnyUpd = false;
	_isAllUpd = true;
}

void TextAcquireStatus::Stop()
{
	pkgAcquireStatus::Stop();
}

void TextAcquireStatus::IMSHit(pkgAcquire::ItemDesc &item)
{
	_outStream << "Hit (" << item.Owner->ID << "): " << item.Description << std::endl;
	_isAnyUpd = true;
}

void TextAcquireStatus::Fetch(pkgAcquire::ItemDesc &item)
{
	if(item.Owner->Complete) return;

	_outStream << "Get (" << item.Owner->ID << "): " << item.Description;
	if(item.Owner->FileSize != 0)
		_outStream << " [" << SizeToStr(static_cast<double>(item.Owner->FileSize))
				   << "B]";
	_outStream << std::endl;
}

void TextAcquireStatus::Done(pkgAcquire::ItemDesc &item)
{
	(void)item;
	_downloadComplete = true;
}

void TextAcquireStatus::Fail(pkgAcquire::ItemDesc &item)
{
	switch(item.Owner->Status) {
	case pkgAcquire::Item::StatDone:
	case pkgAcquire::Item::StatIdle: {
		_outStream << "Ignore (" << item.Owner->ID << "): " << item.Description;
		break;
	}
	default: {
		_outStream << "Error: (" << item.Owner->ID << "): " << item.Description;
		break;
	}
	}

	if(!item.Owner->ErrorText.empty()) {
		std::stringstream errorText;

		errorText << item.Owner->ErrorText;
		_errors.push_back(errorText.str());

		_outStream << item.Owner->ErrorText;
	}

	_outStream << std::endl;
	_isAllUpd = false;
}

bool TextAcquireStatus::Pulse(pkgAcquire *owner)
{
	(void)owner;
	return true;
}

bool TextAcquireStatus::MediaChange(std::string media, std::string drive)
{
	(void)media;
	(void)drive;
	return true;
}

} // namespace package
