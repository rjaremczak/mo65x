#include "mainwindow.h"
#include "commonformatters.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QMessageBox>

static const QString ProjectName = "mo65x";
static const QString ProjectVersion = "0.94";

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  initConfigStorage();
  startEmulator();

  cpuWidget = new CpuWidget(this, emulator->memoryView());
  this->addDockWidget(Qt::RightDockWidgetArea, cpuWidget);

  videoWidget = new VideoWidget(this, emulator->memoryView());
  this->addDockWidget(Qt::LeftDockWidgetArea, videoWidget);

  assemblerWidget = new AssemblerWidget(this, emulator->memoryRef());
  memoryWidget = new MemoryWidget(this, emulator->memoryView());
  disassemblerWidget = new DisassemblerWidget(this, emulator->memoryView());
  viewWidget = new CentralWidget(this, assemblerWidget, memoryWidget, disassemblerWidget);
  setCentralWidget(viewWidget);

  pollTimer = new QTimer(this);
  pollTimer->start(40);
  connect(pollTimer, &QTimer::timeout, this, &MainWindow::polling);

  connect(cpuWidget, &CpuWidget::executionRequested, emulator, &Emulator::execute);
  connect(cpuWidget, &CpuWidget::programCounterChanged, emulator, &Emulator::changeProgramCounter);
  connect(cpuWidget, &CpuWidget::stackPointerChanged, emulator, &Emulator::changeStackPointer);
  connect(cpuWidget, &CpuWidget::registerAChanged, emulator, &Emulator::changeAccumulator);
  connect(cpuWidget, &CpuWidget::registerXChanged, emulator, &Emulator::changeRegisterX);
  connect(cpuWidget, &CpuWidget::registerYChanged, emulator, &Emulator::changeRegisterY);

  connect(cpuWidget, &CpuWidget::clearStatisticsRequested, emulator, &Emulator::clearStatistics, Qt::DirectConnection);
  connect(cpuWidget, &CpuWidget::stopExecutionRequested, emulator, &Emulator::stopExecution, Qt::DirectConnection);
  connect(cpuWidget, &CpuWidget::resetRequested, emulator, &Emulator::triggerReset, Qt::DirectConnection);
  connect(cpuWidget, &CpuWidget::nmiRequested, emulator, &Emulator::triggerNmi, Qt::DirectConnection);
  connect(cpuWidget, &CpuWidget::irqRequested, emulator, &Emulator::triggerIrq, Qt::DirectConnection);

  connect(emulator, &Emulator::stateChanged, cpuWidget, &CpuWidget::updateState);
  connect(emulator, &Emulator::stateChanged, disassemblerWidget, &DisassemblerWidget::updateState);
  connect(emulator, &Emulator::memoryContentChanged, cpuWidget, &CpuWidget::updateOnChange);
  connect(emulator, &Emulator::memoryContentChanged, memoryWidget, &MemoryWidget::updateOnChange);
  connect(emulator, &Emulator::memoryContentChanged, disassemblerWidget, &DisassemblerWidget::updateOnChange);
  connect(emulator, &Emulator::memoryContentChanged, videoWidget, &VideoWidget::updateOnChange);
  connect(emulator, &Emulator::operationCompleted, this, &MainWindow::showMessage);

  connect(assemblerWidget, &AssemblerWidget::newFileCreated, [&] { changeAsmFileName(""); });
  connect(assemblerWidget, &AssemblerWidget::fileLoaded, this, &MainWindow::changeAsmFileName);
  connect(assemblerWidget, &AssemblerWidget::fileSaved, this, &MainWindow::changeAsmFileName);
  connect(assemblerWidget, &AssemblerWidget::operationCompleted, this, &MainWindow::showMessage);
  connect(assemblerWidget, &AssemblerWidget::codeWritten, emulator, &Emulator::memoryContentChanged);
  connect(assemblerWidget, &AssemblerWidget::programCounterChanged, emulator, &Emulator::changeProgramCounter);

  connect(memoryWidget, &MemoryWidget::loadFromFileRequested, emulator, &Emulator::loadMemoryFromFile);
  connect(memoryWidget, &MemoryWidget::saveToFileRequested, emulator, &Emulator::saveMemoryToFile);

  connect(disassemblerWidget, &DisassemblerWidget::goToStartClicked, emulator, &Emulator::changeProgramCounter);

  if (!config.asmFileName.isEmpty()) assemblerWidget->loadFile(config.asmFileName);
  videoWidget->setFrameBufferAddress(0x200);
  propagateState(emulator->state());
}

MainWindow::~MainWindow() {
  emulator->stopExecution();
  emulatorThread.quit();
  emulatorThread.wait();
  delete ui;
}

void MainWindow::changeAsmFileName(const QString& fileName) {
  config.asmFileName = fileName;
  configStorage->write(config);

  QFileInfo fileInfo(fileName);
  setWindowTitle(ProjectName + " " + ProjectVersion + " © mindpart.com : " + fileInfo.fileName());
}

void MainWindow::showMessage(const QString& message, bool success) {
  if (success) {
    ui->statusbar->setStyleSheet("color: white");
  } else {
    ui->statusbar->setStyleSheet("color: orange");
  }
  ui->statusbar->showMessage(message);
}

void MainWindow::prepareToQuit() {
  qDebug("quitting...");
  emulator->stopExecution();
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
  emulatorThread.setObjectName("emulator");
}

void MainWindow::propagateState(EmulatorState es) {

  if (viewWidget->isVisible(memoryWidget)) {
    const QSignalBlocker memoryBlocker(this->memoryWidget);
    memoryWidget->updateOnChange(AddressRange::Max);
  }

  if (viewWidget->isVisible(disassemblerWidget)) {
    const QSignalBlocker disassemblerBlocker(this->disassemblerWidget);
    disassemblerWidget->updateState(es);
    disassemblerWidget->updateOnChange(AddressRange::Max);
  }

  const QSignalBlocker cpuBlocker(this->cpuWidget);
  cpuWidget->updateState(es);

  const QSignalBlocker videoBlocker(this->videoWidget);
  videoWidget->updateView();
}

void MainWindow::polling() {
  if (const auto es = emulator->state(); es.running()) propagateState(es);
}
