#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  initConfigStorage();

  system = new System(this);

  cpuWidget = new CpuWidget(this, system->memoryView());
  this->addDockWidget(Qt::RightDockWidgetArea, cpuWidget);

  assemblerWidget = new AssemblerWidget;
  memoryWidget = new MemoryWidget;
  viewWidget = new CentralWidget(this, assemblerWidget, memoryWidget);
  setCentralWidget(viewWidget);

  if (!config.asmFileName.isEmpty()) assemblerWidget->loadFile(config.asmFileName);

  pollTimer = new QTimer(this);
  connect(pollTimer, &QTimer::timeout, system, &System::propagateCurrentState);
  // pollTimer->start(1000);

  connect(cpuWidget, &CpuWidget::programCounterChangeRequested, system, &System::changeProgramCounter);
  connect(cpuWidget, &CpuWidget::singleStepExecutionRequested, system, &System::executeSingleStep);

  connect(system, &System::cpuStateChanged, cpuWidget, &CpuWidget::updateState);
  connect(system, &System::memoryContentChanged, cpuWidget, &CpuWidget::updateMemoryContent);

  connect(assemblerWidget, &AssemblerWidget::fileLoaded, this, &MainWindow::changeAsmFileName);
  connect(assemblerWidget, &AssemblerWidget::fileSaved, this, &MainWindow::changeAsmFileName);

  system->propagateCurrentState();
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::changeAsmFileName(const QString& fileName) {
  config.asmFileName = fileName;
  configStorage->write(config);
  setWindowTitle("mo65plus: " + fileName);
}

void MainWindow::initConfigStorage() {
  auto appDir = QDir(QDir::homePath() + "/.mo65plus");
  if (!appDir.exists()) appDir.mkpath(".");

  configStorage = new FileDataStorage<Config>(appDir.filePath("config.json"));
  config = configStorage->readOrCreate();
}
