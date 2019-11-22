#include "config.h"
#include "commondefs.h"
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

  QApplication app(argc, argv);
  QApplication::setStyle(QStyleFactory::create("Fusion"));

  QFile qss(":/main.qss");
  qss.open(QFile::ReadOnly);
  app.setStyleSheet(qss.readAll());

  MainWindow mainWindow;
  QRect scr = QGuiApplication::primaryScreen()->geometry();
  mainWindow.move((scr.width() - mainWindow.width()) / 2, (scr.height() - mainWindow.height()) / 2);
  QObject::connect(&app, &QCoreApplication::aboutToQuit, &mainWindow, &MainWindow::prepareToQuit);
  mainWindow.show();
  return app.exec();
}
