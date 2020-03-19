#include "commondefs.h"
#include "config.h"
#include "emulatorstate.h"
#include "filedatastorage.h"
#include "mainwindow.h"
#include "test/assemblertest.h"
#include "test/flagstest.h"
#include "test/instructionstest.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QMetaType>
#include <QScreen>
#include <QStyleFactory>
#include <QTest>

Q_DECLARE_METATYPE(EmulatorState)
Q_DECLARE_METATYPE(Data)
Q_DECLARE_METATYPE(Address)
Q_DECLARE_METATYPE(AddressRange)
Q_DECLARE_METATYPE(FileOperationCallBack)
Q_DECLARE_METATYPE(Frequency)

int test(int argc, char** argv) {
  QApplication app(argc, argv);

  AssemblerTest assemblerTest;
  InstructionsTest opCodesTest;
  FlagsTest flagsTest;

  return QTest::qExec(&opCodesTest, argc, argv) | QTest::qExec(&assemblerTest, argc, argv) | QTest::qExec(&flagsTest, argc, argv);
}

int gtest(int argc, char** argv) {
    QApplication app(argc, argv);

    AssemblerTest assemblerTest;
    InstructionsTest opCodesTest;
    FlagsTest flagsTest;

    return QTest::qExec(&opCodesTest, argc, argv) | QTest::qExec(&assemblerTest, argc, argv) | QTest::qExec(&flagsTest, argc, argv);
}

int main(int argc, char* argv[]) {

#ifdef RUN_TESTS
    return test(argc, argv);
#else
  // register types already aliased by Qt
  qRegisterMetaType<uint8_t>("uint8_t");
  qRegisterMetaType<uint16_t>("uint16_t");
  qRegisterMetaType<Address>("Address");
  qRegisterMetaType<Frequency>("Frequency");

  // register own custom types
  qRegisterMetaType<EmulatorState>();
  qRegisterMetaType<Data>();
  qRegisterMetaType<AddressRange>();
  qRegisterMetaType<FileOperationCallBack>();

  QApplication app(argc, argv);
  QApplication::setStyle(QStyleFactory::create("Fusion"));

  app.setStyleSheet(R"(
    * {
        background-color: rgb(63,67,67);
        color: white;
        selection-color: black;
        selection-background-color: orange;
        alternate-background-color: rgb(41,41,41);
    }

    *:disabled {
        color: gray;
    }

    QAbstractButton:checked {
        color: black;
        background-color: orange;
    }

    QDockWidget {
    }

    QDockWidget::title {
    }

    QMainWindow::separator {
        /*
        background: rgb(41,41,41);
        width: 2px;
        height: 2px;
        */
    }

    QMainWindow::separator:hover {
        /* background: orange; */
    }

    QStatusBar {
        /* border-top: 1px solid rgb(41,41,41); */
    }

  )");

  MainWindow mainWindow;
  QRect scr = QGuiApplication::primaryScreen()->geometry();
  mainWindow.setMinimumWidth(static_cast<int>(scr.width() * 0.85));
  mainWindow.setMinimumHeight(static_cast<int>(scr.height() * 0.85));
  mainWindow.move((scr.width() - mainWindow.width()) / 2, (scr.height() - mainWindow.height()) / 2);
  QObject::connect(&app, &QCoreApplication::aboutToQuit, &mainWindow, &MainWindow::prepareToQuit);
  mainWindow.show();
  return app.exec();
#endif
}
