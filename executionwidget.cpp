#include "executionwidget.h"
#include "ui_executionwidget.h"

static QString flagStr(bool flagStatus, const char* flagCode) {
  return flagStatus ? flagCode : QString("<span style='color:gray'>%1</span>").arg(flagCode);
}

static std::map<ExecutionStatus, const char*> ExecutionStatusStr{
    {Stopped, "Stopped"}, {InvalidOpCode, "Invalid OpCode"}, {Running, "Running"}, {StopRequested, "Stop Requested"}};

ExecutionWidget::ExecutionWidget(QWidget* parent, System* system)
    : QDockWidget(parent), system(system), ui(new Ui::ExecutionWidget) {
  ui->setupUi(this);
  connect(ui->regPC, QOverload<int>::of(&QSpinBox::valueChanged), this, &ExecutionWidget::pcChanged);
  connect(ui->executeSingleStep, &QToolButton::clicked, this, &ExecutionWidget::executeSingleStep);
}

ExecutionWidget::~ExecutionWidget() {
  delete ui;
}

void ExecutionWidget::changePC(uint16_t value) {
  ui->regPC->setValue(value);
}

void ExecutionWidget::executeSingleStep() {
  system->executeSingleStep();
}

void ExecutionWidget::startContinuousExecution() {
}

void ExecutionWidget::stopContinuousExecution() {
}

void ExecutionWidget::updateCpuState(Registers registers, int cycles) {
  ui->regA->setValue(registers.a);
  ui->regX->setValue(registers.x);
  ui->regY->setValue(registers.y);
  ui->regSP->setValue(registers.sp.address());
  ui->regPC->setValue(registers.pc);

  QString str;
  str.append(flagStr(registers.p.negative, "N"));
  str.append(flagStr(registers.p.overflow, "V"));
  str.append(flagStr(false, "."));
  str.append(flagStr(false, "B"));
  str.append(flagStr(registers.p.decimal, "D"));
  str.append(flagStr(registers.p.interrupt, "I"));
  str.append(flagStr(registers.p.zero, "Z"));
  str.append(flagStr(registers.p.carry, "C"));
  ui->flags->setText(str);

  ui->cycles->setNum(cycles);
  setWindowTitle(QString("Execution : %1").arg(ExecutionStatusStr[system->executionStatus()]));
}
