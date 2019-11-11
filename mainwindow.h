#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "assemblerwidget.h"
#include "centralwidget.h"
#include "config.h"
#include "cpuwidget.h"
#include "filedatastorage.h"
#include "memorywidget.h"
#include "system.h"
#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

  Ui::MainWindow* ui;

public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

public slots:
  void changeAsmFileName(const QString&);

private:
  CentralWidget* viewWidget;
  AssemblerWidget* assemblerWidget;
  MemoryWidget* memoryWidget;
  CpuWidget* cpuWidget;
  QTimer* pollTimer;
  System* system;

  FileDataStorage<Config>* configStorage;
  Config config;

  void initConfigStorage();
};

#endif // MAINWINDOW_H
