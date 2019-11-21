#include "mainwindow.h"
#include "formatters.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  initConfigStorage();
  startEmulator();

  cpuWidget = new CpuWidget(this, emulator->memoryView());
  this->addDockWidget(Qt::RightDockWidgetArea, cpuWidget);

  assemblerWidget = new AssemblerWidget(this, emulator->memoryRef());
  memoryWidget = new MemoryWidget(this, emulator->memoryView());
  disassemblerWidget = new DisassemblerWidget(this, emulator->memoryView());
  viewWidget = new CentralWidget(this, assemblerWidget, memoryWidget, disassemblerWidget);
  setCentralWidget(viewWidget);

  pollTimer = new QTimer(this);
  // pollTimer->start(1000);

  connect(cpuWidget, &CpuWidget::startStepExecution, [&] { emulator->startExecution(false); });
  connect(cpuWidget, &CpuWidget::startContinuousExecution, [&] { emulator->startExecution(true); });
  connect(cpuWidget, &CpuWidget::programCounterChanged, emulator, &Emulator::changeProgramCounter);
  connect(cpuWidget, &CpuWidget::stackPointerChanged, emulator, &Emulator::changeStackPointer);
  connect(cpuWidget, &CpuWidget::registerAChanged, emulator, &Emulator::changeAccumulator);
  connect(cpuWidget, &CpuWidget::registerXChanged, emulator, &Emulator::changeRegisterX);
  connect(cpuWidget, &CpuWidget::registerYChanged, emulator, &Emulator::changeRegisterY);
  connect(cpuWidget, &CpuWidget::clearStatisticsRequested, emulator, &Emulator::clearStatistics);

  connect(cpuWidget, &CpuWidget::stopExecutionRequested, emulator, &Emulator::stopExecution, Qt::DirectConnection);
  connect(cpuWidget, &CpuWidget::resetRequested, emulator, &Emulator::triggerReset, Qt::DirectConnection);
  connect(cpuWidget, &CpuWidget::nmiRequested, emulator, &Emulator::triggerNmi, Qt::DirectConnection);
  connect(cpuWidget, &CpuWidget::irqRequested, emulator, &Emulator::triggerIrq, Qt::DirectConnection);

  connect(emulator, &Emulator::stateChanged, cpuWidget, &CpuWidget::processState);
  connect(emulator, &Emulator::stateChanged, this, &MainWindow::processEmulatorState);
  connect(emulator, &Emulator::memoryContentChanged, cpuWidget, &CpuWidget::updateMemoryView);
  connect(emulator, &Emulator::memoryContentChanged, memoryWidget, &MemoryWidget::updateMemoryView);
  connect(emulator, &Emulator::operationCompleted, this, &MainWindow::showMessage);
  connect(emulator, &Emulator::memoryContentChanged, disassemblerWidget->view, &DisassemblerView::updateMemoryView);
  connect(emulator, &Emulator::stateChanged, disassemblerWidget,
          [&](EmulatorState es) { disassemblerWidget->view->changeSelected(es.registers.pc); });

  connect(assemblerWidget, &AssemblerWidget::fileLoaded, this, &MainWindow::changeAsmFileName);
  connect(assemblerWidget, &AssemblerWidget::fileSaved, this, &MainWindow::changeAsmFileName);
  connect(assemblerWidget, &AssemblerWidget::operationCompleted, this, &MainWindow::showMessage);
  connect(assemblerWidget, &AssemblerWidget::codeWritten, emulator, &Emulator::memoryContentChanged);
  connect(assemblerWidget, &AssemblerWidget::programCounterChanged, emulator, &Emulator::changeProgramCounter);

  connect(memoryWidget, &MemoryWidget::loadFromFileRequested, emulator, &Emulator::loadMemoryFromFile);
  connect(memoryWidget, &MemoryWidget::saveToFileRequested, emulator, &Emulator::saveMemoryToFile);

  connect(disassemblerWidget, &DisassemblerWidget::goToStartClicked, emulator, &Emulator::changeProgramCounter);

  emulator->stateChanged(emulator->currentState());

  if (!config.asmFileName.isEmpty()) assemblerWidget->loadFile(config.asmFileName);
}

MainWindow::~MainWindow() {
  emulatorThread.quit();
  emulatorThread.wait();
  delete ui;
}

void MainWindow::changeAsmFileName(const QString& fileName) {
  config.asmFileName = fileName;
  configStorage->write(config);

  QFileInfo fileInfo(fileName);
  setWindowTitle("mo65plus: " + fileInfo.fileName());
}

void MainWindow::showMessage(const QString& message, bool success) {
  if (success) {
    ui->statusbar->setStyleSheet("color: white");
  } else {
    ui->statusbar->setStyleSheet("color: orange");
  }
  ui->statusbar->showMessage(message);
}

void MainWindow::processEmulatorState(EmulatorState emulatorState) {
  QString msg(tr(formatExecutionState(emulatorState.executionState)));
  if (auto es = emulatorState.lastExecutionStatistics; es.valid()) msg.append(" : ").append(formatExecutionStatistics(es));
  showMessage(msg, emulatorState.executionState != CpuState::Halted);
}

void MainWindow::initConfigStorage() {
  auto appDir = QDir(QDir::homePath() + "/.mo65plus");
  if (!appDir.exists()) appDir.mkpath(".");

  configStorage = new FileDataStorage<Config>(appDir.filePath("config.json"));
  config = configStorage->readOrCreate();
}

void MainWindow::startEmulator() {
  emulator = new Emulator();
  emulator->moveToThread(&emulatorThread);
  connect(&emulatorThread, &QThread::finished, emulator, &Emulator::deleteLater);
  emulatorThread.start();
}
