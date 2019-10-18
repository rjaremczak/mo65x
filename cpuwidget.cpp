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

void CpuWidget::updateState(const CpuState state)
{
    ui->regA->setText(format8(state.a));
    ui->regX->setText(format8(state.x));
    ui->regY->setText(format8(state.y));
    ui->regSP->setText(format16(0x100 + state.sp));
    ui->regPC->setText(format16(state.pc));

    QString str;
    str.append(flagStr(state.flag.n, "N"));
    str.append(flagStr(state.flag.v, "V"));
    str.append(flagStr(false, "."));
    str.append(flagStr(state.flag.b, "B"));
    str.append(flagStr(state.flag.d, "D"));
    str.append(flagStr(state.flag.i, "I"));
    str.append(flagStr(state.flag.z, "Z"));
    str.append(flagStr(state.flag.c, "C"));
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
