#include "executionwidget.h"
#include "ui_executionwidget.h"

static QString flagStr(bool flagStatus, const char* flagCode) {
  return flagStatus ? flagCode : QString("<span style='color:gray'>%1</span>").arg(flagCode);
}

ExecutionWidget::ExecutionWidget(QWidget* parent, System* system)
    : QDockWidget(parent), system(system), ui(new Ui::ExecutionWidget) {
  ui->setupUi(this);
  connect(ui->regPC, QOverload<int>::of(&QSpinBox::valueChanged), this, &ExecutionWidget::pcChanged);
}

ExecutionWidget::~ExecutionWidget() {
  delete ui;
}

void ExecutionWidget::changePC(uint16_t value) {
  ui->regPC->setValue(value);
}

void ExecutionWidget::executeSingleStep() {
}

void ExecutionWidget::startContinuousExecution() {
}

void ExecutionWidget::stopContinuousExecution() {
}

void ExecutionWidget::updateCpuState(CpuRegisters regs, CpuFlags flags) {
  ui->regA->setValue(regs.a);
  ui->regX->setValue(regs.x);
  ui->regY->setValue(regs.y);
  ui->regSP->setValue(regs.sp);
  ui->regPC->setValue(regs.pc);

  QString str;
  str.append(flagStr(flags.n, "N"));
  str.append(flagStr(flags.v, "V"));
  str.append(flagStr(false, "."));
  str.append(flagStr(false, "B"));
  str.append(flagStr(flags.d, "D"));
  str.append(flagStr(flags.i, "I"));
  str.append(flagStr(flags.z, "Z"));
  str.append(flagStr(flags.c, "C"));
  ui->flags->setText(str);
}
