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

void CpuWidget::updateState(const CpuRegisters state)
{
    ui->regA->setText(format8(state.A));
    ui->regX->setText(format8(state.X));
    ui->regY->setText(format8(state.Y));
    ui->regSP->setText(format16(0x100 + state.SP));
    ui->regPC->setText(format16(state.PC));

    ui->flagB->setEnabled(state.flagB);
    ui->flagC->setEnabled(state.flagC);
    ui->flagD->setEnabled(state.flagD);
    ui->flagI->setEnabled(state.flagI);
    ui->flagN->setEnabled(state.flagN);
    ui->flagV->setEnabled(state.flagV);
    ui->flagZ->setEnabled(state.flagZ);
}

QString CpuWidget::format8(uint8_t num) const
{
    return QString("%1").arg(num,2,16,QChar('0')).toUpper();
}

QString CpuWidget::format16(uint16_t num) const
{
    return QString("%1").arg(num,4,16,QChar('0')).toUpper();
}
