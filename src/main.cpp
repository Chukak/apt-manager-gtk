#include "cache.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	package::Cache cache;
	package::CandidateList candidates = cache.getCandidates(package::Upgradable);
	std::cout << candidates << std::endl;

	return 0;
}
