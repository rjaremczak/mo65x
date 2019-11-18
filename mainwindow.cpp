#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  initConfigStorage();
  startSystem();

  cpuWidget_ = new CpuWidget(this, emulator_->memoryView());
  this->addDockWidget(Qt::RightDockWidgetArea, cpuWidget_);

  assemblerWidget_ = new AssemblerWidget;
  memoryWidget_ = new MemoryWidget(this, emulator_->memoryView());
  viewWidget_ = new CentralWidget(this, assemblerWidget_, memoryWidget_);
  setCentralWidget(viewWidget_);

  pollTimer_ = new QTimer(this);
  connect(pollTimer_, &QTimer::timeout, emulator_, &Emulator::propagateCurrentState);
  // pollTimer->start(1000);

  connect(cpuWidget_, &CpuWidget::programCounterChanged, emulator_, &Emulator::changeProgramCounter);
  connect(cpuWidget_, &CpuWidget::executeOneInstructionRequested, emulator_, &Emulator::executeOneInstruction);

  connect(emulator_, &Emulator::cpuStateChanged, cpuWidget_, &CpuWidget::updateState);
  connect(emulator_, &Emulator::memoryContentChanged, cpuWidget_, &CpuWidget::updateMemoryView);
  connect(emulator_, &Emulator::memoryContentChanged, memoryWidget_, &MemoryWidget::updateMemoryView);
  connect(emulator_, &Emulator::operationCompleted, this, &MainWindow::showInStatusLine);

  connect(assemblerWidget_, &AssemblerWidget::fileLoaded, this, &MainWindow::changeAsmFileName);
  connect(assemblerWidget_, &AssemblerWidget::fileSaved, this, &MainWindow::changeAsmFileName);
  connect(assemblerWidget_, &AssemblerWidget::machineCodeGenerated, emulator_, &Emulator::loadMemory);
  connect(assemblerWidget_, &AssemblerWidget::machineCodeGenerated, cpuWidget_, &CpuWidget::changeProgramCounter);

  connect(memoryWidget_, &MemoryWidget::loadFromFileRequested, emulator_, &Emulator::loadMemoryFromFile);
  connect(memoryWidget_, &MemoryWidget::saveToFileRequested, emulator_, &Emulator::saveMemoryToFile);

  emulator_->propagateCurrentState();

  if (!config_.asmFileName.isEmpty()) assemblerWidget_->loadFile(config_.asmFileName);
}

MainWindow::~MainWindow() {
  systemThread_.quit();
  systemThread_.wait();
  delete ui;
}

void MainWindow::changeAsmFileName(const QString& fileName) {
  config_.asmFileName = fileName;
  configStorage_->write(config_);

  QFileInfo fileInfo(fileName);
  setWindowTitle("mo65plus: " + fileInfo.fileName());
}

void MainWindow::showMessageBox(const QString& message, bool success) {
  if (success)
    QMessageBox::information(this, "", message);
  else
    QMessageBox::warning(this, "", message);
}

void MainWindow::showInStatusLine(const QString& message, bool) {
  ui->statusbar->showMessage(message, 10000);
}

void MainWindow::initConfigStorage() {
  auto appDir = QDir(QDir::homePath() + "/.mo65plus");
  if (!appDir.exists()) appDir.mkpath(".");

  configStorage_ = new FileDataStorage<Config>(appDir.filePath("config.json"));
  config_ = configStorage_->readOrCreate();
}

void MainWindow::startSystem() {
  emulator_ = new Emulator();
  emulator_->moveToThread(&systemThread_);
  connect(&systemThread_, &QThread::finished, emulator_, &Emulator::deleteLater);
  systemThread_.start();
}
