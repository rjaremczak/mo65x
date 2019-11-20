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

CpuWidget::CpuWidget(QWidget* parent, const Memory& memory)
    : QDockWidget(parent), ui(new Ui::CpuWidget), memory(memory), disassembler(memory) {
  ui->setupUi(this);

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

  setMonospaceFont(ui->disassemblerView);
  setMonospaceFont(ui->flags);
}

CpuWidget::~CpuWidget() {
  delete ui;
}

void CpuWidget::updateMemoryView(AddressRange range) {
  if (disassemblerRange.overlapsWith(range)) updateDisassemblerView();
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
  if (disassemblerRange.first != regs.pc) {
    disassemblerRange.first = regs.pc;
    updateDisassemblerView();
  }

  const auto processing =
      es.executionState == CpuState::Running || es.executionState == CpuState::Halting || es.executionState == CpuState::Stopping;

  ui->cpuFrame->setDisabled(processing);
  ui->auxFrame->setDisabled(processing);
  ui->skipInstruction->setDisabled(processing);
  ui->startExecution->setDisabled(processing);
  ui->stopExecution->setDisabled(!processing);
  ui->executeSingleStep->setDisabled(processing || es.executionState == CpuState::Halted);
}

void CpuWidget::resizeEvent(QResizeEvent* event) {
  if (event->size().height() != event->oldSize().height()) { updateDisassemblerView(); }
}

int CpuWidget::rowsInView() const {
  return 6 + ui->disassemblerView->height() / ui->disassemblerView->fontMetrics().height();
}

void CpuWidget::updateDisassemblerView() {
  disassembler.setOrigin(disassemblerRange.first);
  QString html("<div style='white-space:pre; display:inline-block'>");
  int rows = rowsInView();
  if (rows--) {
    html.append("<div style='color:black; background-color: lightgreen'>");
    html.append(disassembler.disassemble()).append("</div>");
    disassembler.nextInstruction();

    html.append("<div style='color:darkseagreen'>");
    while (rows--) {
      html.append(disassembler.disassemble() + "<br>");
      disassembler.nextInstruction();
    }

    html.append("</div>");
  }
  html.append("</div>");
  ui->disassemblerView->setHtml(html);
  disassemblerRange.last = disassembler.currentAddress();
}

void CpuWidget::changeProgramCounter(uint16_t addr) {
  ui->regPC->setValue(addr);
}

void CpuWidget::skipInstruction() {
  disassembler.setOrigin(disassemblerRange.first);
  disassembler.nextInstruction();
  emit programCounterChanged(disassembler.currentAddress());
}
