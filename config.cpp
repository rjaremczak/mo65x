#include "config.h"

static constexpr auto AsmFileName = "asmFileName";

void Config::read(const QJsonObject& json) {
  asmFileName = json[AsmFileName].toString();
}

void Config::write(QJsonObject& json) const {
  json[AsmFileName] = asmFileName;
}
