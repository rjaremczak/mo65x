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

  connect(cpuWidget, &CpuWidget::executionRequested, emulator, &EmulatorQt::execute);
  connect(cpuWidget, &CpuWidget::programCounterChanged, emulator, &EmulatorQt::changeProgramCounter);
  connect(cpuWidget, &CpuWidget::stackPointerChanged, emulator, &EmulatorQt::changeStackPointer);
  connect(cpuWidget, &CpuWidget::registerAChanged, emulator, &EmulatorQt::changeAccumulator);
  connect(cpuWidget, &CpuWidget::registerXChanged, emulator, &EmulatorQt::changeRegisterX);
  connect(cpuWidget, &CpuWidget::registerYChanged, emulator, &EmulatorQt::changeRegisterY);

  connect(cpuWidget, &CpuWidget::clearStatisticsRequested, emulator, &EmulatorQt::clearStatistics, Qt::DirectConnection);
  connect(cpuWidget, &CpuWidget::stopExecutionRequested, emulator, &EmulatorQt::stopExecution, Qt::DirectConnection);
  connect(cpuWidget, &CpuWidget::resetRequested, emulator, &EmulatorQt::triggerReset, Qt::DirectConnection);
  connect(cpuWidget, &CpuWidget::nmiRequested, emulator, &EmulatorQt::triggerNmi, Qt::DirectConnection);
  connect(cpuWidget, &CpuWidget::irqRequested, emulator, &EmulatorQt::triggerIrq, Qt::DirectConnection);

  connect(emulator, &EmulatorQt::stateChanged, cpuWidget, &CpuWidget::updateState);
  connect(emulator, &EmulatorQt::stateChanged, disassemblerWidget, &DisassemblerWidget::updateState);
  connect(emulator, &EmulatorQt::memoryContentChanged, cpuWidget, &CpuWidget::updateOnChange);
  connect(emulator, &EmulatorQt::memoryContentChanged, memoryWidget, &MemoryWidget::updateOnChange);
  connect(emulator, &EmulatorQt::memoryContentChanged, disassemblerWidget, &DisassemblerWidget::updateOnChange);
  connect(emulator, &EmulatorQt::memoryContentChanged, videoWidget, &VideoWidget::updateOnChange);
  connect(emulator, &EmulatorQt::operationCompleted, this, &MainWindow::showMessage);

  connect(assemblerWidget, &AssemblerWidget::newFileCreated, [&] { changeAsmFileName(""); });
  connect(assemblerWidget, &AssemblerWidget::fileLoaded, this, &MainWindow::changeAsmFileName);
  connect(assemblerWidget, &AssemblerWidget::fileSaved, this, &MainWindow::changeAsmFileName);
  connect(assemblerWidget, &AssemblerWidget::operationCompleted, this, &MainWindow::showMessage);
  connect(assemblerWidget, &AssemblerWidget::codeWritten, emulator, &EmulatorQt::memoryContentChanged);
  connect(assemblerWidget, &AssemblerWidget::programCounterChanged, emulator, &EmulatorQt::changeProgramCounter);

  connect(memoryWidget, &MemoryWidget::loadFromFileRequested, emulator, &EmulatorQt::loadMemoryFromFile);
  connect(memoryWidget, &MemoryWidget::saveToFileRequested, emulator, &EmulatorQt::saveMemoryToFile);

  connect(disassemblerWidget, &DisassemblerWidget::goToStartClicked, emulator, &EmulatorQt::changeProgramCounter);

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
  emulator = new EmulatorQt();
  emulator->moveToThread(&emulatorThread);
  connect(&emulatorThread, &QThread::finished, emulator, &EmulatorQt::deleteLater);
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
  if (const auto es = emulator->state(); es.cpuState.running()) propagateState(es);
}
