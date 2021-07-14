#include "cache.h"

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	package::Cache cache;
	package::CandidateList packages(cache);

	std::cout << packages << std::endl;

	return 0;
}
