#include "commonutils.h"

namespace mindpart {

std::filesystem::path home_path() {
#ifdef _WIN32
  return std::string(getenv("HOMEDRIVE")) + std::string(getenv("HOMEPATH"));
#else
  return getenv("HOME");
#endif
}

} // namespace mindpart
