#include "monitorwidget.h"
#include "disassembler.h"
#include "ui_monitorwidget.h"
#include <QFontDatabase>
#include <QResizeEvent>

MonitorWidget::MonitorWidget(QWidget *parent, const Memory &memory)
    : QDockWidget(parent), ui(new Ui::MonitorWidget), m_disassembler(memory) {
  ui->setupUi(this);
  initView();

  connect(ui->regPC, QOverload<int>::of(&QSpinBox::valueChanged),
          [&] { changeAddress(static_cast<uint16_t>(ui->regPC->value())); });
}

MonitorWidget::~MonitorWidget() {
  delete ui;
}

void MonitorWidget::changeAddress(uint16_t pc) {
  if (m_firstAddress != pc) {
    m_firstAddress = pc;
    updateMemoryView();
    emit addressChanged(pc);
  }
}

void MonitorWidget::updateMemoryContent(uint16_t first, uint16_t last) {
  if (m_firstAddress == std::clamp(m_firstAddress, first, last) || m_lastAddress == std::clamp(m_lastAddress, first, last)) {
    updateMemoryView();
  }
}

static QString flagStr(bool flagStatus, const char *flagCode) {
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
  str.append(flagStr(flags.b, "B"));
  str.append(flagStr(flags.d, "D"));
  str.append(flagStr(flags.i, "I"));
  str.append(flagStr(flags.z, "Z"));
  str.append(flagStr(flags.c, "C"));
  ui->flags->setText(str);

  if (regs.pc != m_firstAddress) {
    m_firstAddress = regs.pc;
    updateMemoryView();
  }
}

void MonitorWidget::resizeEvent(QResizeEvent *event) {
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
  m_disassembler.setAddress(m_firstAddress);
  QString html("<div style='white-space:pre; display:inline-block'>");
  int rows = rowsInView();
  if (rows--) {
    html.append("<div style='color:black; background-color: lightgreen'>");
    html.append(m_disassembler.disassemble()).append("</div>");
    m_disassembler.step();

    html.append("<div style='color:darkseagreen'>");
    while (rows--) {
      html.append(m_disassembler.disassemble() + "<br>");
      m_disassembler.step();
    }

    html.append("</div>");
  }
  html.append("</div>");
  ui->dumpView->setHtml(html);
  m_lastAddress = m_disassembler.address();
}

void MonitorWidget::updateMemoryView() {
  disassemblerView();
}
