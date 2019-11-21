#include "cpuwidget.h"
#include "formatters.h"
#include "ui_cpuwidget.h"
#include "uitools.h"
#include <QFontDatabase>
#include <QMetaEnum>
#include <QResizeEvent>

static QString flagStr(bool flagStatus, const char* flagCode) {
  return flagStatus ? flagCode : QString("<span style='color:gray'>%1</span>").arg(flagCode);
}

CpuWidget::CpuWidget(QWidget* parent, const Memory& memory) : QDockWidget(parent), ui(new Ui::CpuWidget), memory(memory) {
  ui->setupUi(this);

  disassemblerView = new DisassemblerView(this, memory);
  QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui->dockWidgetContents->layout());
  layout->insertWidget(layout->indexOf(ui->auxFrame), disassemblerView);

  connect(ui->regPC, QOverload<int>::of(&QSpinBox::valueChanged), this, &CpuWidget::programCounterChanged);
  connect(ui->regSP, QOverload<int>::of(&QSpinBox::valueChanged), this, &CpuWidget::stackPointerChanged);
  connect(ui->regA, QOverload<int>::of(&QSpinBox::valueChanged), this, &CpuWidget::registerAChanged);
  connect(ui->regX, QOverload<int>::of(&QSpinBox::valueChanged), this, &CpuWidget::registerXChanged);
  connect(ui->regY, QOverload<int>::of(&QSpinBox::valueChanged), this, &CpuWidget::registerYChanged);
  connect(ui->reset, &QAbstractButton::clicked, this, &CpuWidget::resetRequested);
  connect(ui->nmi, &QAbstractButton::clicked, this, &CpuWidget::nmiRequested);
  connect(ui->irq, &QAbstractButton::clicked, this, &CpuWidget::irqRequested);
  connect(ui->clearCycleCounter, &QAbstractButton::clicked, this, &CpuWidget::clearStatisticsRequested);
  connect(ui->executeSingleStep, &QAbstractButton::clicked, this, &CpuWidget::startStepExecution);
  connect(ui->skipInstruction, &QAbstractButton::clicked, this, &CpuWidget::skipInstruction);
  connect(ui->startExecution, &QAbstractButton::clicked, this, &CpuWidget::startContinuousExecution);
  connect(ui->stopExecution, &QAbstractButton::clicked, this, &CpuWidget::stopExecutionRequested);

  setMonospaceFont(disassemblerView);
  setMonospaceFont(ui->flags);
}

CpuWidget::~CpuWidget() {
  delete ui;
}

void CpuWidget::updateMemoryView(AddressRange range) {
  disassemblerView->updateMemoryView(range);
}

void CpuWidget::processState(EmulatorState es) {
  const auto& regs = es.registers;

  setWindowTitle(tr("cpu : %1").arg(formatExecutionState(es.executionState)));

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

  ui->ioPortDirection->setValue(memory[IOPortConfig]);
  ui->ioPortData->setValue(memory[IOPortData]);
  ui->avgStatistics->setText(formatExecutionStatistics(es.avgExecutionStatistics));

  ui->regPC->setValue(regs.pc);
  disassemblerView->changeStart(regs.pc);

  const auto processing =
      es.executionState == CpuState::Running || es.executionState == CpuState::Halting || es.executionState == CpuState::Stopping;

  ui->cpuFrame->setDisabled(processing);
  ui->auxFrame->setDisabled(processing);
  ui->skipInstruction->setDisabled(processing);
  ui->startExecution->setDisabled(processing);
  ui->stopExecution->setDisabled(!processing);
  ui->executeSingleStep->setDisabled(processing || es.executionState == CpuState::Halted);
}

void CpuWidget::changeProgramCounter(uint16_t addr) {
  ui->regPC->setValue(addr);
}

void CpuWidget::skipInstruction() {
  disassemblerView->nextInstruction();
  emit programCounterChanged(disassemblerView->start());
}
