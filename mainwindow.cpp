#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  system = new System(this);

  monitorWidget = new DisassemblerWidget(this, system);
  this->addDockWidget(Qt::LeftDockWidgetArea, monitorWidget);

  memoryWidget = new MemoryWidget(this, system);
  this->addDockWidget(Qt::RightDockWidgetArea, memoryWidget);

  executionWidget = new ExecutionWidget(this, system);
  this->addDockWidget(Qt::RightDockWidgetArea, executionWidget);

  centralWidget = new CentralWidget(this);
  this->setCentralWidget(centralWidget);

  pollTimer = new QTimer(this);
  connect(pollTimer, &QTimer::timeout, system, &System::checkCpuState);
  // pollTimer->start(1000);

  connect(system, &System::cpuStateChanged, executionWidget, &ExecutionWidget::updateCpuState);
  connect(system, &System::cpuStateChanged, [&](auto registers) { monitorWidget->changeAddress(registers.pc); });
  connect(system, &System::memoryContentChanged, monitorWidget, &DisassemblerWidget::updateMemoryContent);

  connect(monitorWidget, &DisassemblerWidget::addressChanged, system, &System::changePC);

  connect(executionWidget, &ExecutionWidget::pcChanged, system, &System::changePC);

  system->checkCpuState();
}

MainWindow::~MainWindow() {
  delete ui;
}
