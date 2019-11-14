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
#include <QThread>
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
  CentralWidget* viewWidget_;
  AssemblerWidget* assemblerWidget_;
  MemoryWidget* memoryWidget_;
  CpuWidget* cpuWidget_;
  QTimer* pollTimer_;
  System* system_;
  FileDataStorage<Config>* configStorage_;
  Config config_;

  QThread systemThread_;

  void initConfigStorage();
  void startSystem();
};

#endif // MAINWINDOW_H
