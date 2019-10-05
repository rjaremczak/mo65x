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

void CpuWidget::updateState(const CpuRegisters registers)
{
    ui->regA->setText(format8(registers.a));
    ui->regX->setText(format8(registers.x));
    ui->regY->setText(format8(registers.y));
    ui->regSP->setText(format16(0x100 + registers.sp));
    ui->regPC->setText(format16(registers.pc));

    ui->flagB->setEnabled(registers.flags.b);
    ui->flagC->setEnabled(registers.flags.c);
    ui->flagD->setEnabled(registers.flags.d);
    ui->flagI->setEnabled(registers.flags.i);
    ui->flagN->setEnabled(registers.flags.n);
    ui->flagV->setEnabled(registers.flags.v);
    ui->flagZ->setEnabled(registers.flags.z);
}

QString CpuWidget::format8(uint8_t num) const
{
    return QString("%1").arg(num,2,16,QChar('0')).toUpper();
}

QString CpuWidget::format16(uint16_t num) const
{
    return QString("%1").arg(num,4,16,QChar('0')).toUpper();
}
