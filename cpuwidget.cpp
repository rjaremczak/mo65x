#include "cpuwidget.h"
#include "ui_cpuwidget.h"
#include <QResizeEvent>

static QString flagStr(bool flagStatus, const char* flagCode) {
  return flagStatus ? flagCode : QString("<span style='color:gray'>%1</span>").arg(flagCode);
}

static std::map<ExecutionState, const char*> ExecutionStateStr{{ExecutionState::Stopped, "Stopped"},
                                                               {ExecutionState::InvalidOpCode, "Invalid OpCode"},
                                                               {ExecutionState::Running, "Running"},
                                                               {ExecutionState::PendingStop, "Stop Requested"}};

CpuWidget::CpuWidget(QWidget* parent, const Memory& memory)
    : QDockWidget(parent), ui(new Ui::CpuWidget), memory(memory), disassembler(memory) {
  ui->setupUi(this);
  connect(ui->regPC, QOverload<int>::of(&QSpinBox::valueChanged), this, &CpuWidget::programCounterChangeRequested);
  connect(ui->executeSingleStep, &QToolButton::clicked, this, &CpuWidget::singleStepExecutionRequested);
  connect(ui->skipInstruction, &QToolButton::clicked, this, &CpuWidget::skipInstruction);
}

CpuWidget::~CpuWidget() {
  delete ui;
}

void CpuWidget::updateMemoryContent(uint16_t first, uint16_t last) {
  if (disassemblerFirstAddress_ == std::clamp(disassemblerFirstAddress_, first, last) ||
      disassemblerLastAddress_ == std::clamp(disassemblerLastAddress_, first, last)) {
    updateDisassemblerView();
  }
}

void CpuWidget::updateState(CpuInfo info) {
  const auto& regs = info.regs;

  setWindowTitle(QString("CPU : %1").arg(ExecutionStateStr[info.state]));

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

  ui->cycles->setNum(info.cycles);
  ui->ioPortDirection->setValue(memory[IOPortConfig]);
  ui->ioPortData->setValue(memory[IOPortData]);

  ui->regPC->setValue(regs.pc);
  if (disassemblerFirstAddress_ != regs.pc) {
    disassemblerFirstAddress_ = regs.pc;
    updateDisassemblerView();
  }
}

void CpuWidget::resizeEvent(QResizeEvent* event) {
  if (event->size().height() != event->oldSize().height()) { updateDisassemblerView(); }
}

int CpuWidget::rowsInView() const {
  return 2 + ui->disassemblerView->height() / ui->disassemblerView->fontMetrics().height();
}

void CpuWidget::updateDisassemblerView() {
  disassembler.setOrigin(disassemblerFirstAddress_);
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
  disassemblerLastAddress_ = disassembler.currentAddress();
}

void CpuWidget::skipInstruction() {
  disassembler.setOrigin(disassemblerFirstAddress_);
  disassembler.nextInstruction();
  emit programCounterChangeRequested(disassembler.currentAddress());
}
