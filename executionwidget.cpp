#include "executionwidget.h"
#include "ui_executionwidget.h"

static QString flagStr(bool flagStatus, const char* flagCode) {
  return flagStatus ? flagCode : QString("<span style='color:gray'>%1</span>").arg(flagCode);
}

static std::map<Cpu::State, const char*> ExecutionStatusStr{{Cpu::Stopped, "Stopped"},
                                                            {Cpu::InvalidOpCode, "Invalid OpCode"},
                                                            {Cpu::Running, "Running"},
                                                            {Cpu::PendingStop, "Stop Requested"}};

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

void ExecutionWidget::updateCpuState() {
  const auto regs = system->cpu().regs;

  ui->regA->setValue(regs.a);
  ui->regX->setValue(regs.x);
  ui->regY->setValue(regs.y);
  ui->regSP->setValue(regs.sp.address());
  ui->regPC->setValue(regs.pc);

  QString str;
  str.append(flagStr(regs.p.negative, "N"));
  str.append(flagStr(regs.p.overflow, "V"));
  str.append(flagStr(false, "."));
  str.append(flagStr(false, "B"));
  str.append(flagStr(regs.p.decimal, "D"));
  str.append(flagStr(regs.p.interrupt, "I"));
  str.append(flagStr(regs.p.zero, "Z"));
  str.append(flagStr(regs.p.carry, "C"));
  ui->flags->setText(str);

  ui->cycles->setNum(system->cpu().cycles);
  setWindowTitle(QString("Execution : %1").arg(ExecutionStatusStr[system->cpu().state()]));
}
