#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

static void initStyle() {
    QApplication::setStyle(QStyleFactory::create("Fusion"));
}

static void initPalette() {
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53,53,53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(53,53,53));
    palette.setColor(QPalette::AlternateBase, QColor(45,45,45));
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53,53,53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);

    palette.setColor(QPalette::Highlight, QColor(0xfd, 0x6a, 0x02).lighter());
    palette.setColor(QPalette::HighlightedText, Qt::black);
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    QApplication::setPalette(palette);
}


int main(int argc, char *argv[])
{
    initStyle();
    initPalette();
    QApplication a(argc, argv);
    MainWindow w;
    //QObject::connect(&a, &QCoreApplication::aboutToQuit, &w, &MainWindow::prepareToQuit);
    w.show();
    return a.exec();
}
