#include "cache.h"
#include "utils.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	package::Cache cache;
	package::CandidateList candidates = cache.getCandidates(package::Upgradable);
	utils::GetLog() << candidates << std::endl;

	return 0;
}
