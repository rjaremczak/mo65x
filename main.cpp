#include "mainwindow.h"

#include "config.h"
#include "filedatastorage.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QScreen>
#include <QStyleFactory>

static QString loadStringFromFile(const QString& name) {
  QFile file(name);
  file.open(QFile::ReadOnly);
  return QLatin1String(file.readAll());
}

static QDir initAppDirectory() {
  auto appDir = QDir(QDir::homePath() + "/.mo65plus");
  if (!appDir.exists()) appDir.mkpath(".");
  return appDir;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QFile qss(":/main.qss");
    qss.open( QFile::ReadOnly);
    a.setStyleSheet(qss.readAll());

    MainWindow w;
    QRect scr = QGuiApplication::primaryScreen()->geometry();
    w.move((scr.width() - w.width()) / 2, (scr.height() - w.height()) / 2);
    w.show();
    return a.exec();
}
