#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  initConfigStorage();
  startSystem();

  cpuWidget_ = new CpuWidget(this, system_->memoryView());
  this->addDockWidget(Qt::RightDockWidgetArea, cpuWidget_);

  assemblerWidget_ = new AssemblerWidget;
  memoryWidget_ = new MemoryWidget;
  viewWidget_ = new CentralWidget(this, assemblerWidget_, memoryWidget_);
  setCentralWidget(viewWidget_);

  pollTimer_ = new QTimer(this);
  connect(pollTimer_, &QTimer::timeout, system_, &System::propagateCurrentState);
  // pollTimer->start(1000);

  connect(cpuWidget_, &CpuWidget::programCounterChanged, system_, &System::changeProgramCounter);
  connect(cpuWidget_, &CpuWidget::singleStepExecutionRequested, system_, &System::executeSingleStep);

  connect(system_, &System::cpuStateChanged, cpuWidget_, &CpuWidget::updateState);
  connect(system_, &System::memoryContentChanged, cpuWidget_, &CpuWidget::updateMemoryContent);

  connect(assemblerWidget_, &AssemblerWidget::fileLoaded, this, &MainWindow::changeAsmFileName);
  connect(assemblerWidget_, &AssemblerWidget::fileSaved, this, &MainWindow::changeAsmFileName);
  connect(assemblerWidget_, &AssemblerWidget::machineCodeGenerated, system_, &System::uploadToMemory);
  connect(assemblerWidget_, &AssemblerWidget::machineCodeGenerated,
          [&](auto addr, auto) { cpuWidget_->changeProgramCounter(addr); });

  system_->propagateCurrentState();

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

void MainWindow::initConfigStorage() {
  auto appDir = QDir(QDir::homePath() + "/.mo65plus");
  if (!appDir.exists()) appDir.mkpath(".");

  configStorage_ = new FileDataStorage<Config>(appDir.filePath("config.json"));
  config_ = configStorage_->readOrCreate();
}

void MainWindow::startSystem() {
  system_ = new System();
  system_->moveToThread(&systemThread_);
  connect(&systemThread_, &QThread::finished, system_, &System::deleteLater);
  systemThread_.start();
}
