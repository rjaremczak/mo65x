#pragma once

#include <QJsonObject>
#include <QString>

struct Config {
  QString asmFileName;

  void read(const QJsonObject& json);
  void write(QJsonObject& json) const;
};
