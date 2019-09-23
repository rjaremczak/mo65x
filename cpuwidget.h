#ifndef CPUWIDGET_H
#define CPUWIDGET_H

#include <QDockWidget>

#include "cpustate.h"

namespace Ui {
class CpuWidget;
}

class CpuWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CpuWidget(QWidget *parent = nullptr);
    ~CpuWidget();

public slots:
    void updateState(const CpuState);

private:
    Ui::CpuWidget *ui;

    QString format8(uint8_t) const;
    QString format16(uint16_t) const;
};

#endif // CPUWIDGET_H
