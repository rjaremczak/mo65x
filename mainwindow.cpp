#include "mainwindow.h"
#include "formatters.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QMessageBox>

static const QString ProjectName = "mo65plus";

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
  pollTimer->start(1000);
  connect(pollTimer, &QTimer::timeout, this, &MainWindow::polling);
  connect(this, &MainWindow::statePolled, cpuWidget, &CpuWidget::updatePolledData);
  connect(this, &MainWindow::statePolled, memoryWidget, &MemoryWidget::updatePolledData);
  connect(this, &MainWindow::statePolled, disassemblerWidget, &DisassemblerWidget::updatePolledData);

  connect(cpuWidget, &CpuWidget::singleStepRequested, emulator, &Emulator::executeSingleStep, Qt::DirectConnection);
  connect(cpuWidget, &CpuWidget::continuousExecutionRequested, emulator, &Emulator::startContinuousExecution);
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

  connect(emulator, &Emulator::stateChanged, cpuWidget, &CpuWidget::updateState);
  connect(emulator, &Emulator::memoryContentChanged, cpuWidget, &CpuWidget::updateMemory);
  connect(emulator, &Emulator::memoryContentChanged, memoryWidget, &MemoryWidget::updateMemory);
  connect(emulator, &Emulator::operationCompleted, this, &MainWindow::showMessage);
  connect(emulator, &Emulator::memoryContentChanged, disassemblerWidget, &DisassemblerWidget::updateMemory);
  connect(emulator, &Emulator::stateChanged, disassemblerWidget, &DisassemblerWidget::updateState);

  connect(assemblerWidget, &AssemblerWidget::fileLoaded, this, &MainWindow::changeAsmFileName);
  connect(assemblerWidget, &AssemblerWidget::fileSaved, this, &MainWindow::changeAsmFileName);
  connect(assemblerWidget, &AssemblerWidget::operationCompleted, this, &MainWindow::showMessage);
  connect(assemblerWidget, &AssemblerWidget::codeWritten, emulator, &Emulator::memoryContentChanged);
  connect(assemblerWidget, &AssemblerWidget::programCounterChanged, emulator, &Emulator::changeProgramCounter);

  connect(memoryWidget, &MemoryWidget::loadFromFileRequested, emulator, &Emulator::loadMemoryFromFile);
  connect(memoryWidget, &MemoryWidget::saveToFileRequested, emulator, &Emulator::saveMemoryToFile);

  connect(disassemblerWidget, &DisassemblerWidget::goToStartClicked, emulator, &Emulator::changeProgramCounter);

  if (!config.asmFileName.isEmpty()) assemblerWidget->loadFile(config.asmFileName);

  emit statePolled(emulator->state());
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
  setWindowTitle(ProjectName + ": " + fileInfo.fileName());
}

void MainWindow::showMessage(const QString& message, bool success) {
  if (success) {
    ui->statusbar->setStyleSheet("color: white");
  } else {
    ui->statusbar->setStyleSheet("color: orange");
  }
  ui->statusbar->showMessage(message);
}

void MainWindow::initConfigStorage() {
  auto appDir = QDir(QDir::homePath() + "/." + ProjectName);
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

void MainWindow::polling() {
  if (const auto es = emulator->state(); es.running()) { emit statePolled(es); }
}
