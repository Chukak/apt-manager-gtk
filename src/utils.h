#ifndef UTILS_H
#define UTILS_H

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

} // namespace utils

#endif // UTILS_H
