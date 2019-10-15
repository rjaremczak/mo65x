#include "cpuwidget.h"
#include "ui_cpuwidget.h"

CpuWidget::CpuWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CpuWidget)
{
    ui->setupUi(this);
}

CpuWidget::~CpuWidget()
{
    delete ui;
}

static QString flagStr(bool flagStatus, const char* flagCode)
{
    return flagStatus ? flagCode : QString("<span style='color:gray'>%1</span>").arg(flagCode);
}

void CpuWidget::updateState(const CpuRegisters registers)
{
    ui->regA->setText(format8(registers.a));
    ui->regX->setText(format8(registers.x));
    ui->regY->setText(format8(registers.y));
    ui->regSP->setText(format16(0x100 + registers.sp));
    ui->regPC->setText(format16(registers.pc));

    QString str;
    str.append(flagStr(registers.flags.n, "N"));
    str.append(flagStr(registers.flags.v, "V"));
    str.append(flagStr(false, "."));
    str.append(flagStr(registers.flags.b, "B"));
    str.append(flagStr(registers.flags.d, "D"));
    str.append(flagStr(registers.flags.i, "I"));
    str.append(flagStr(registers.flags.z, "Z"));
    str.append(flagStr(registers.flags.c, "C"));
    ui->flags->setText(str);
}

QString CpuWidget::format8(uint8_t num) const
{
    return QString("%1").arg(num,2,16,QChar('0')).toUpper();
}

QString CpuWidget::format16(uint16_t num) const
{
    return QString("%1").arg(num,4,16,QChar('0')).toUpper();
}
