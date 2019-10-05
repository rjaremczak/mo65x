#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QFile qss(":/main.qss");
    qss.open( QFile::ReadOnly);
    a.setStyleSheet(qss.readAll());

    MainWindow w;
    //QObject::connect(&a, &QCoreApplication::aboutToQuit, &w, &MainWindow::prepareToQuit);
    w.show();
    return a.exec();
}
