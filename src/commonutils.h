#include <string>

namespace mindpart {

bool is_file(const std::string&);
bool is_directory(const std::string&);

bool create_directory(const std::string&);

std::string home_directory();

} // namespace mindpart
