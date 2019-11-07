#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  system = new System(this);

  disassemblerWidget = new DisassemblerWidget(this, system);
  this->addDockWidget(Qt::LeftDockWidgetArea, disassemblerWidget);

  memoryWidget = new MemoryWidget(this, system);
  this->addDockWidget(Qt::RightDockWidgetArea, memoryWidget);

  executionWidget = new ExecutionWidget(this, system);
  this->addDockWidget(Qt::RightDockWidgetArea, executionWidget);

  centralWidget = new CentralWidget(this);
  this->setCentralWidget(centralWidget);

  pollTimer = new QTimer(this);
  connect(pollTimer, &QTimer::timeout, system, &System::cpuStateChanged);
  // pollTimer->start(1000);

  connect(system, &System::cpuStateChanged, executionWidget, &ExecutionWidget::updateCpuState);
  connect(system, &System::cpuStateChanged, [&] { disassemblerWidget->changeAddress(system->cpu().regs.pc); });
  connect(system, &System::memoryContentChanged, disassemblerWidget, &DisassemblerWidget::updateMemoryContent);

  connect(disassemblerWidget, &DisassemblerWidget::addressChanged, system, &System::changeExecutionAddress);

  connect(executionWidget, &ExecutionWidget::pcChanged, system, &System::changeExecutionAddress);

  emit system->cpuStateChanged();
}

MainWindow::~MainWindow() {
  delete ui;
}
