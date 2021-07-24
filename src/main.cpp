#include "cache.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	package::Cache cache;
	package::CandidateList packages(cache,
									package::NewAvailablePackages /*InstalledPackages*/);

	std::cout << packages << std::endl;

	return 0;
}
