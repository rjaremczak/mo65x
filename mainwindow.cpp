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

  m_cpuWidget = new CpuWidget(this, m_emulator.memoryView());
  this->addDockWidget(Qt::RightDockWidgetArea, m_cpuWidget);

  m_videoWidget = new VideoWidget(this, m_emulator.memoryView());
  this->addDockWidget(Qt::LeftDockWidgetArea, m_videoWidget);

  m_assemblerWidget = new AssemblerWidget(this, m_emulator.memoryRef());
  m_memoryWidget = new MemoryWidget(this, m_emulator.memoryView());
  m_disassemblerWidget = new DisassemblerWidget(this, m_emulator.memoryView());
  m_viewWidget = new CentralWidget(this, m_assemblerWidget, m_memoryWidget, m_disassemblerWidget);
  setCentralWidget(m_viewWidget);

  m_pollTimer = new QTimer(this);
  m_pollTimer->start(40);
  connect(m_pollTimer, &QTimer::timeout, this, &MainWindow::polling);

  connect(m_cpuWidget, &CpuWidget::executionRequested, this, &MainWindow::execute);
  connect(m_cpuWidget, &CpuWidget::programCounterChanged, this, &MainWindow::changeProgramCounter);
  connect(m_cpuWidget, &CpuWidget::stackPointerChanged, this, &MainWindow::changeStackPointer);
  connect(m_cpuWidget, &CpuWidget::registerAChanged, this, &MainWindow::changeRegisterA);
  connect(m_cpuWidget, &CpuWidget::registerXChanged, this, &MainWindow::changeRegisterX);
  connect(m_cpuWidget, &CpuWidget::registerYChanged, this, &MainWindow::changeRegisterY);

  connect(m_cpuWidget, &CpuWidget::clearStatisticsRequested, this, &MainWindow::clearStatistics);
  connect(m_cpuWidget, &CpuWidget::stopExecutionRequested, this, &MainWindow::stopExecution);
  connect(m_cpuWidget, &CpuWidget::resetRequested, this, &MainWindow::triggerReset);
  connect(m_cpuWidget, &CpuWidget::nmiRequested, this, &MainWindow::triggerNmi);
  connect(m_cpuWidget, &CpuWidget::irqRequested, this, &MainWindow::triggerIrq);

  // connect(emulator, &EmulatorQt::stateChanged, m_cpuWidget, &CpuWidget::updateState);
  // connect(emulator, &EmulatorQt::stateChanged, m_disassemblerWidget, &DisassemblerWidget::updateState);
  // connect(emulator, &EmulatorQt::memoryContentChanged, m_cpuWidget, &CpuWidget::updateOnMemoryChange);
  // connect(emulator, &EmulatorQt::memoryContentChanged, m_memoryWidget, &MemoryWidget::updateOnMemoryChange);
  // connect(emulator, &EmulatorQt::memoryContentChanged, m_disassemblerWidget, &DisassemblerWidget::updateOnMemoryChange);
  // connect(emulator, &EmulatorQt::memoryContentChanged, m_videoWidget, &VideoWidget::updateOnMemoryChange);
  connect(emulator, &EmulatorQt::operationCompleted, this, &MainWindow::showMessage);

  connect(m_assemblerWidget, &AssemblerWidget::newFileCreated, [&] { changeAsmFileName(""); });
  connect(m_assemblerWidget, &AssemblerWidget::fileLoaded, this, &MainWindow::changeAsmFileName);
  connect(m_assemblerWidget, &AssemblerWidget::fileSaved, this, &MainWindow::changeAsmFileName);
  connect(m_assemblerWidget, &AssemblerWidget::operationCompleted, this, &MainWindow::showMessage);
  connect(m_assemblerWidget, &AssemblerWidget::codeWritten, this, &EmulatorQt::memoryContentChanged);
  connect(m_assemblerWidget, &AssemblerWidget::programCounterChanged, this, &MainWindow::changeProgramCounter);

  connect(m_memoryWidget, &MemoryWidget::loadFromFileRequested, this, &MainWindow::loadMemoryFromFile);
  connect(m_memoryWidget, &MemoryWidget::saveToFileRequested, this, &MainWindow::saveMemoryToFile);

  connect(m_disassemblerWidget, &DisassemblerWidget::goToStartClicked, this, &EmulatorQt::changeProgramCounter);

  if (!m_config.asmFileName.isEmpty()) m_assemblerWidget->loadFile(m_config.asmFileName);
  m_videoWidget->setFrameBufferAddress(0x200);
  propagateState(m_emulator.state());
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::changeAsmFileName(const QString& fileName) {
  m_config.asmFileName = fileName;
  m_configStorage->write(m_config);

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
}

void MainWindow::initConfigStorage() {
  auto appDir = QDir(QDir::homePath() + "/." + ProjectName);
  if (!appDir.exists()) appDir.mkpath(".");

  m_configStorage = new FileDataStorage<Config>(appDir.filePath("config.json"));
  m_config = m_configStorage->readOrCreate();
}

void MainWindow::propagateState(EmulatorState es) {

  if (m_viewWidget->isVisible(m_memoryWidget)) {
    const QSignalBlocker memoryBlocker(this->m_memoryWidget);
    m_memoryWidget->updateOnMemoryChange(AddressRange::Max);
  }

  if (m_viewWidget->isVisible(m_disassemblerWidget)) {
    const QSignalBlocker disassemblerBlocker(this->m_disassemblerWidget);
    m_disassemblerWidget->updateState(es);
    m_disassemblerWidget->updateOnMemoryChange(AddressRange::Max);
  }

  const QSignalBlocker cpuBlocker(this->m_cpuWidget);
  m_cpuWidget->updateState(es);

  const QSignalBlocker videoBlocker(this->m_videoWidget);
  m_videoWidget->updateView();
}

void MainWindow::emulatorStateChanged() {
  const auto es = m_emulator.state();
  m_cpuWidget->updateState(es);
  m_disassemblerWidget->updateState(es);
}

void MainWindow::emulatorMemoryContentChanged(AddressRange range) {
  m_cpuWidget->updateOnMemoryChange(range);
  m_memoryWidget->updateOnMemoryChange(range);
  m_disassemblerWidget->updateOnMemoryChange(range);
  m_videoWidget->updateOnMemoryChange(range);
}

void MainWindow::polling() {
  if (const auto es = m_emulator.state(); es.cpuState.running()) propagateState(es);
}

void MainWindow::clearStatistics() {
  m_emulator.clearStatistics();
  emulatorStateChanged();
}

void MainWindow::stopExecution() {
  m_emulator.stopExecution();
  emulatorStateChanged();
}

void MainWindow::triggerReset() {
  m_emulator.triggerReset();
  emulatorStateChanged();
}

void MainWindow::triggerNmi() {
  m_emulator.triggerNmi();
  emulatorStateChanged();
}

void MainWindow::triggerIrq() {
  m_emulator.triggerIrq();
  emulatorStateChanged();
}

void MainWindow::execute(bool continuous, Frequency clock) {
}

void MainWindow::changeProgramCounter(uint16_t) {
}

void MainWindow::changeStackPointer(uint16_t) {
}

void MainWindow::changeRegisterA(uint8_t) {
}

void MainWindow::changeRegisterX(uint8_t) {
}

void MainWindow::changeRegisterY(uint8_t) {
}

void MainWindow::memoryContentChanged(AddressRange range) {
  emulatorMemoryContentChanged(range);
}

void MainWindow::loadMemoryFromFile(Address start, const QString& fname) {
  // emit operationCompleted(rsize > 0 ? tr("loaded %1 B\nfrom file %2").arg(rsize).arg(fname) : "load error", rsize > 0);
}

void MainWindow::saveMemoryToFile(AddressRange, const QString& fname) {
  //  emit operationCompleted(rsize > 0 ? tr("saved %1 B\nto file %2").arg(rsize).arg(fname) : "save error", rsize > 0);
}
