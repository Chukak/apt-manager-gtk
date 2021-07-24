#ifndef UTILS_H
#define UTILS_H

#include <apt-pkg/configuration.h>

namespace utils
{
/**
 * @brief InitPkgConfiguration
 * Initializes the apt-pkg _config object.
 * @return Result of initialization.
 */
bool InitPkgConfiguration();
/**
 * @brief PrintPkgError
 * Prints the last error.
 */
void PrintPkgError();
/**
 * @brief GetPkgConfig
 * @return The configuration object.
 */
Configuration& GetPkgConfig();

} // namespace utils

#endif // UTILS_H
