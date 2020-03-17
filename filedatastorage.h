#pragma once

#include <QFile>
#include <QFileDevice>
#include <QJsonDocument>
#include <QJsonObject>
#include <fstream>
#include <sstream>

template <class DataObjectType> class FileDataStorage {
  const std::string m_filePath;

public:
  FileDataStorage(const std::string& filePath) : m_filePath(filePath) {}

  bool read(DataObjectType& dataObject) {
    std::ifstream is(m_filePath);
    if (!is.is_open()) return false;

    std::ostringstream oss;
    oss << is.rdbuf();
    const auto buf = QByteArray::fromStdString(oss.str());
    dataObject.read(QJsonDocument::fromJson(buf).object());
    return true;
  }

  DataObjectType read() {
    DataObjectType dataObject;
    read(dataObject);
    return dataObject;
  }

  void write(const DataObjectType& dataObject) {
    std::ofstream os(m_filePath);
    if (os.is_open()) {
      QJsonObject json;
      dataObject.write(json);
      const auto buf = QJsonDocument(json).toJson();
      os.write(buf.data(), buf.size());
    } else {
      qFatal("unable to save configuration file %s", m_filePath);
    }
  }

  DataObjectType readOrInitialise() {
    DataObjectType dao;
    if (!read(dao)) write(dao);
    return dao;
  }
};
