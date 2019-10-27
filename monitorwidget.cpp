#include "monitorwidget.h"
#include "disassembler.h"
#include "ui_monitorwidget.h"
#include <QFontDatabase>
#include <QResizeEvent>

MonitorWidget::MonitorWidget(QWidget* parent, System* system)
    : QDockWidget(parent), ui(new Ui::MonitorWidget), system_(system), disassembler_(system->memory()) {
  ui->setupUi(this);
  initView();

  connect(ui->regPC, QOverload<int>::of(&QSpinBox::valueChanged),
          [&] { changeAddress(static_cast<uint16_t>(ui->regPC->value())); });
}

MonitorWidget::~MonitorWidget() {
  delete ui;
}

void MonitorWidget::changeAddress(uint16_t pc) {
  if (firstAddress_ != pc) {
    firstAddress_ = pc;
    updateMemoryView();
    emit addressChanged(pc);
  }
}

void MonitorWidget::updateMemoryContent(uint16_t first, uint16_t last) {
  if (firstAddress_ == std::clamp(firstAddress_, first, last) || lastAddress_ == std::clamp(lastAddress_, first, last)) {
    updateMemoryView();
  }
}

static QString flagStr(bool flagStatus, const char* flagCode) {
  return flagStatus ? flagCode : QString("<span style='color:gray'>%1</span>").arg(flagCode);
}

void MonitorWidget::updateCpuState(CpuRegisters regs, CpuFlags flags) {
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

  if (regs.pc != firstAddress_) {
    firstAddress_ = regs.pc;
    updateMemoryView();
  }
}

void MonitorWidget::resizeEvent(QResizeEvent* event) {
  if (event->size().height() != event->oldSize().height()) {
    updateMemoryView();
  }
}

int MonitorWidget::rowsInView() const {
  return 2 + ui->dumpView->height() / ui->dumpView->fontMetrics().height();
}

void MonitorWidget::initView() {
  ui->viewModeGroup->setId(ui->viewModeAsm, Asm);
  ui->viewModeGroup->setId(ui->viewModeHex, Hex);
  ui->viewModeGroup->setId(ui->viewModeTxt, Txt);
  ui->viewModeAsm->setChecked(true);

  auto font = QFontDatabase::systemFont(QFontDatabase::FixedFont);

#ifdef __APPLE__
  font.setPointSize(13);
#endif

  ui->dumpView->setFont(font);
}

void MonitorWidget::disassemblerView() {
  disassembler_.setAddress(firstAddress_);
  QString html("<div style='white-space:pre; display:inline-block'>");
  int rows = rowsInView();
  if (rows--) {
    html.append("<div style='color:black; background-color: lightgreen'>");
    html.append(disassembler_.disassemble()).append("</div>");
    disassembler_.step();

    html.append("<div style='color:darkseagreen'>");
    while (rows--) {
      html.append(disassembler_.disassemble() + "<br>");
      disassembler_.step();
    }

    html.append("</div>");
  }
  html.append("</div>");
  ui->dumpView->setHtml(html);
  lastAddress_ = disassembler_.address();
}

void MonitorWidget::updateMemoryView() {
  disassemblerView();
}
