#pragma once

#include <QFile>
#include <QFileDevice>
#include <QJsonDocument>
#include <QJsonObject>

template <class DataObjectType> class FileDataStorage {
  const QString m_filePath;

public:
  FileDataStorage(const QString& filePath) : m_filePath(filePath) {}

  void read(DataObjectType& dataObject) {
    QFile file(m_filePath);
    if (file.open(QIODevice::ReadOnly)) {
      dataObject.read(QJsonDocument::fromJson(file.readAll()).object());
    } else {
      qFatal("unable to read configuration file %s", qPrintable(m_filePath));
    }
  }

  DataObjectType read() {
    DataObjectType dataObject;
    read(dataObject);
    return dataObject;
  }

  void write(const DataObjectType& dataObject) {
    QFile file(m_filePath);
    if (file.open(QIODevice::WriteOnly)) {
      QJsonObject json;
      dataObject.write(json);
      file.write(QJsonDocument(json).toJson());
    } else {
      qFatal("unable to save configuration file %s", qPrintable(m_filePath));
    }
  }

  DataObjectType readOrCreate(const DataObjectType& defaultDataObject) {
    if (QFile(m_filePath).exists()) {
      return read();
    } else {
      write(defaultDataObject);
      return defaultDataObject;
    }
  }

  DataObjectType readOrCreate() {
    if (QFile(m_filePath).exists()) {
      return read();
    } else {
      DataObjectType dataObject;
      write(dataObject);
      return dataObject;
    }
  }
};
