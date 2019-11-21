#include "config.h"
#include "defs.h"
#include "emulatorstate.h"
#include "filedatastorage.h"
#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QMetaType>
#include <QScreen>
#include <QStyleFactory>

Q_DECLARE_METATYPE(EmulatorState)
Q_DECLARE_METATYPE(Data)
Q_DECLARE_METATYPE(Address)
Q_DECLARE_METATYPE(AddressRange)
Q_DECLARE_METATYPE(FileOperationCallBack)

int main(int argc, char* argv[]) {

  // register types already aliased by Qt
  qRegisterMetaType<uint8_t>("uint8_t");
  qRegisterMetaType<uint16_t>("uint16_t");
  qRegisterMetaType<Address>("Address");

  // register own custom types
  qRegisterMetaType<EmulatorState>();
  qRegisterMetaType<Data>();
  qRegisterMetaType<AddressRange>();
  qRegisterMetaType<FileOperationCallBack>();

  QApplication a(argc, argv);
  QApplication::setStyle(QStyleFactory::create("Fusion"));

  QFile qss(":/main.qss");
  qss.open(QFile::ReadOnly);
  a.setStyleSheet(qss.readAll());

  MainWindow w;
  QRect scr = QGuiApplication::primaryScreen()->geometry();
  w.move((scr.width() - w.width()) / 2, (scr.height() - w.height()) / 2);
  w.show();
  return a.exec();
}
