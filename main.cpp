#include "config.h"
#include "filedatastorage.h"
#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QScreen>
#include <QStyleFactory>

int main(int argc, char* argv[]) {
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
