#include "commonutils.h"
#include <fstream>
#include <sys/stat.h>

namespace mindpart {

bool is_file(const std::string& str) {
  return std::ifstream(str).good();
}

bool is_directory(const std::string& dir) {
  struct stat info;
  if (stat(dir.c_str(), &info) != 0) return false;
  return info.st_mode & S_IFDIR;
}

std::string home_directory() {
#ifdef _WIN32
  return std::string(getenv("HOMEDRIVE")) + std::string(getenv("HOMEPATH"));
#else
  return getenv("HOME");
#endif
}

bool create_directory(const std::string &dir)
{
  int error = 0;
#if defined(_WIN32)
  error = mkdir(dir.c_str());  // can be used on Windows
#else
  error = mkdir(sPath.c_str(), nMode);  // can be used on non-Windows
#endif
  return error == 0;
}

} // namespace mindpart
