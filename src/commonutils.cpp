#include "commonutils.h"
#include <fstream>
#include <sys/stat.h>

namespace mindpart {

std::filesystem::path home_directory() {
#ifdef _WIN32
  return std::string(getenv("HOMEDRIVE")) + std::string(getenv("HOMEPATH"));
#else
  return getenv("HOME");
#endif
}

} // namespace mindpart
