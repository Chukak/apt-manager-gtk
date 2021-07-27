#include "cache.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	package::Cache cache;

	std::cout << cache.getCandidates(package::Upgradable) << std::endl;

	return 0;
}
