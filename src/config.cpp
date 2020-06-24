#include "config.h"

static constexpr auto AsmFileName = "asmFileName";

void Config::read(const nlohmann::json& json) {
  asmFileName = json[AsmFileName];
}

nlohmann::json Config::toJson() const {
  nlohmann::json json;
  json[AsmFileName] = asmFileName;
  return json;
}
