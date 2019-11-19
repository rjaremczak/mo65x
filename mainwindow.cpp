#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  initConfigStorage();
  startEmulator();

  cpuWidget_ = new CpuWidget(this, emulator_->memoryView());
  this->addDockWidget(Qt::RightDockWidgetArea, cpuWidget_);

  assemblerWidget_ = new AssemblerWidget;
  memoryWidget_ = new MemoryWidget(this, emulator_->memoryView());
  viewWidget_ = new CentralWidget(this, assemblerWidget_, memoryWidget_);
  setCentralWidget(viewWidget_);

  pollTimer_ = new QTimer(this);
  connect(pollTimer_, &QTimer::timeout, emulator_, &Emulator::notifyStateChanged);
  // pollTimer->start(1000);

  connect(cpuWidget_, &CpuWidget::executeOneInstructionRequested, emulator_, &Emulator::executeOneInstruction);
  connect(cpuWidget_, &CpuWidget::startExecutionRequested, emulator_, &Emulator::startExecution);
  connect(cpuWidget_, &CpuWidget::programCounterChanged, emulator_, &Emulator::changeProgramCounter);
  connect(cpuWidget_, &CpuWidget::stackPointerChanged, emulator_, &Emulator::changeStackPointer);
  connect(cpuWidget_, &CpuWidget::registerAChanged, emulator_, &Emulator::changeAccumulator);
  connect(cpuWidget_, &CpuWidget::registerXChanged, emulator_, &Emulator::changeRegisterX);
  connect(cpuWidget_, &CpuWidget::registerYChanged, emulator_, &Emulator::changeRegisterY);

  connect(cpuWidget_, &CpuWidget::stopExecutionRequested, emulator_, &Emulator::stopExecution, Qt::DirectConnection);
  connect(cpuWidget_, &CpuWidget::clearCycleCounterRequested, emulator_, &Emulator::resetCycleCounter, Qt::DirectConnection);
  connect(cpuWidget_, &CpuWidget::resetRequested, emulator_, &Emulator::triggerReset, Qt::DirectConnection);
  connect(cpuWidget_, &CpuWidget::nmiRequested, emulator_, &Emulator::triggerNmi, Qt::DirectConnection);
  connect(cpuWidget_, &CpuWidget::irqRequested, emulator_, &Emulator::triggerIrq, Qt::DirectConnection);

  connect(emulator_, &Emulator::cpuStateChanged, cpuWidget_, &CpuWidget::updateState);
  connect(emulator_, &Emulator::memoryContentChanged, cpuWidget_, &CpuWidget::updateMemoryView);
  connect(emulator_, &Emulator::memoryContentChanged, memoryWidget_, &MemoryWidget::updateMemoryView);
  connect(emulator_, &Emulator::operationCompleted, this, &MainWindow::showResult);

  connect(assemblerWidget_, &AssemblerWidget::fileLoaded, this, &MainWindow::changeAsmFileName);
  connect(assemblerWidget_, &AssemblerWidget::fileSaved, this, &MainWindow::changeAsmFileName);
  connect(assemblerWidget_, &AssemblerWidget::machineCodeGenerated, emulator_, &Emulator::loadMemory);
  connect(assemblerWidget_, &AssemblerWidget::machineCodeGenerated, cpuWidget_, &CpuWidget::changeProgramCounter);
  connect(assemblerWidget_, &AssemblerWidget::operationCompleted, this, &MainWindow::showResult);

  connect(memoryWidget_, &MemoryWidget::loadFromFileRequested, emulator_, &Emulator::loadMemoryFromFile);
  connect(memoryWidget_, &MemoryWidget::saveToFileRequested, emulator_, &Emulator::saveMemoryToFile);

  emulator_->notifyStateChanged();

  if (!config_.asmFileName.isEmpty()) assemblerWidget_->loadFile(config_.asmFileName);
}

MainWindow::~MainWindow() {
  emulatorThread_.quit();
  emulatorThread_.wait();
  delete ui;
}

void MainWindow::changeAsmFileName(const QString& fileName) {
  config_.asmFileName = fileName;
  configStorage_->write(config_);

  QFileInfo fileInfo(fileName);
  setWindowTitle("mo65plus: " + fileInfo.fileName());
}

void MainWindow::showResult(const QString& message, bool success) {
  if (success) {
    ui->statusbar->setStyleSheet("color: darkseagreen");
    ui->statusbar->showMessage(message, 10000);
  } else {
    ui->statusbar->setStyleSheet("color: red");
    ui->statusbar->showMessage(message);
  }
}

void MainWindow::initConfigStorage() {
  auto appDir = QDir(QDir::homePath() + "/.mo65plus");
  if (!appDir.exists()) appDir.mkpath(".");

  configStorage_ = new FileDataStorage<Config>(appDir.filePath("config.json"));
  config_ = configStorage_->readOrCreate();
}

void MainWindow::startEmulator() {
  emulator_ = new Emulator();
  emulator_->moveToThread(&emulatorThread_);
  connect(&emulatorThread_, &QThread::finished, emulator_, &Emulator::deleteLater);
  emulatorThread_.start();
}
