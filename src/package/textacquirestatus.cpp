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

	// TODO: можно собрать статистику
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
	_outStream << std::endl;

	if(!item.Owner->ErrorText.empty()) {
		std::stringstream errorText;

		errorText << item.Owner->ErrorText;
		// TODO: форматирование

		_errors.push_back(errorText.str());
	}

	_isAllUpd = false;
}

bool TextAcquireStatus::Pulse(pkgAcquire *owner)
{
	(void)owner;
	// TODO: здесь можно испускать сигнал об изменениях загрузки итема
	return true;
}

bool TextAcquireStatus::MediaChange(std::string media, std::string drive)
{
	(void)media;
	(void)drive;
	// TODO: смена дисков, сделать если необходимо
	return true;
}

} // namespace package
