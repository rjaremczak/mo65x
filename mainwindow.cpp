#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  system_ = new System(this);

  memoryWidget_ = new MemoryWidget(this, system_);
  this->addDockWidget(Qt::RightDockWidgetArea, memoryWidget_);

  monitorWidget_ = new MonitorWidget(this, system_);
  this->addDockWidget(Qt::RightDockWidgetArea, monitorWidget_);

  centralWidget_ = new CentralWidget(this);
  this->setCentralWidget(centralWidget_);

  pollTimer_ = new QTimer(this);
  connect(pollTimer_, &QTimer::timeout, system_, &System::checkCpuState);
  pollTimer_->start(1000);

  connect(system_, &System::cpuStateChanged, monitorWidget_, &MonitorWidget::updateCpuState);
  connect(system_, &System::memoryContentChanged, monitorWidget_, &MonitorWidget::updateMemoryContent);

  connect(monitorWidget_, &MonitorWidget::addressChanged, system_, &System::changePC);

  system_->checkCpuState();
}

MainWindow::~MainWindow() {
  delete ui;
}
