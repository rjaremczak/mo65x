#include "cpuwidget.h"
#include "commonformatters.h"
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

  connect(ui->reset, &QAbstractButton::clicked, this, &CpuWidget::resetRequested);
  connect(ui->nmi, &QAbstractButton::clicked, this, &CpuWidget::nmiRequested);
  connect(ui->irq, &QAbstractButton::clicked, this, &CpuWidget::irqRequested);
  connect(ui->regPC, QOverload<int>::of(&QSpinBox::valueChanged), this, &CpuWidget::programCounterChanged);
  connect(ui->regSP, QOverload<int>::of(&QSpinBox::valueChanged), this, &CpuWidget::stackPointerChanged);
  connect(ui->regA, QOverload<int>::of(&QSpinBox::valueChanged), this, &CpuWidget::registerAChanged);
  connect(ui->regX, QOverload<int>::of(&QSpinBox::valueChanged), this, &CpuWidget::registerXChanged);
  connect(ui->regY, QOverload<int>::of(&QSpinBox::valueChanged), this, &CpuWidget::registerYChanged);
  connect(ui->clearStatistics, &QAbstractButton::clicked, this, &CpuWidget::clearStatisticsRequested);
  connect(ui->skipInstruction, &QAbstractButton::clicked, this, &CpuWidget::skipInstruction);
  connect(ui->continuousExecution, &QAbstractButton::clicked, [&] { emitExecutionRequest(true); });
  connect(ui->stepExecution, &QAbstractButton::clicked, this, [&] { emitExecutionRequest(false); });
  connect(ui->stopExecution, &QAbstractButton::clicked, this, &CpuWidget::stopExecutionRequested);

  setMonospaceFont(disassemblerView);
  setMonospaceFont(ui->flags);

  ui->ioPortConfig->setValue(0b11010001);
}

CpuWidget::~CpuWidget() {
  delete ui;
}

void CpuWidget::updateOnMemoryChange(AddressRange range) {
  disassemblerView->updateMemoryView(range);
  updateSpecialCpuAddresses();
}

void CpuWidget::updateState(EmulatorState es) {
  const auto& regs = es.regs;

  setWindowTitle(tr("%1 @ %2").arg(CpuState::format(es.cpuState.execution), CpuState::format(es.cpuState.mode)));

  ui->regA->setValue(regs.a);
  ui->regX->setValue(regs.x);
  ui->regY->setValue(regs.y);
  ui->regSP->setValue(regs.sp.address());
  ui->regPC->setValue(regs.pc);

  updateSpecialCpuAddresses();

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

  ui->lastExecStats->setText(formatExecutionStatistics(es.lastExecutionStatistics));
  ui->avgExecStats->setText(formatExecutionStatistics(es.accExecutionStatistics));

  ui->regPC->setValue(regs.pc);
  disassemblerView->changeStart(regs.pc);
  updateUI(es.cpuState.execution);
}

void CpuWidget::changeProgramCounter(uint16_t addr) {
  ui->regPC->setValue(addr);
}

void CpuWidget::updateSpecialCpuAddresses() {
  ui->resetVector->setValue(memory.word(CpuAddress::ResetVector));
  ui->nmiVector->setValue(memory.word(CpuAddress::NmiVector));
  ui->irqVector->setValue(memory.word(CpuAddress::IrqVector));
  ui->ioPortData->setValue(memory[CpuAddress::IoPortData]);
  ui->ioPortConfig->setValue(memory[CpuAddress::IoPortConfig]);
}

void CpuWidget::updateUI(CpuState::Execution state) {
  const auto processing = state == CpuState::Execution::Running || state == CpuState::Execution::Stopping;
  ui->cpuFrame->setDisabled(processing);
  ui->skipInstruction->setDisabled(processing);
  ui->continuousExecution->setDisabled(processing);
  ui->stopExecution->setDisabled(!processing);
  ui->stepExecution->setDisabled(processing || state == CpuState::Execution::Halted);
  ui->nmiVector->setDisabled(processing);
  ui->resetVector->setDisabled(processing);
  ui->irqVector->setDisabled(processing);
  ui->ioPortData->setDisabled(processing);
  ui->ioPortConfig->setDisabled(processing);
  ui->clockFrequency->setDisabled(processing);
}

void CpuWidget::skipInstruction() {
  disassemblerView->nextInstruction();
  emit programCounterChanged(disassemblerView->first());
}

void CpuWidget::emitExecutionRequest(bool continuous) {
  updateUI(CpuState::Execution::Running);
  emit executionRequested(continuous, static_cast<Frequency>(ui->clockFrequency->value() * 1e6));
}
