#include "utils.h"

#include <apt-pkg/init.h>
#include <apt-pkg/pkgsystem.h>
#include <apt-pkg/error.h>

namespace utils
{
bool InitPkgConfiguration()
{
	static bool confIsInitialized(false);
	if(!confIsInitialized) {
		if(!pkgInitConfig(*_config)) {
			PrintPkgError();
			return false;
		}

		if(!pkgInitSystem(*_config, _system)) {
			_error->DumpErrors(std::cerr);
			return false;
		}
	}
	return true;
}

void PrintPkgError()
{
	if(_error->PendingError()) {
		_error->DumpErrors(std::cerr);
	}
}

Configuration& GetPkgConfig()
{
	return *_config;
}

} // namespace utils
