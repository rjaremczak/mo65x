#ifndef MONITORWIDGET_H
#define MONITORWIDGET_H

#include <QDockWidget>
#include <QStringListModel>
#include "disassembler.h"

namespace Ui {
class MonitorWidget;
}

class MonitorWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit MonitorWidget(QWidget *parent, const Memory& );
    ~MonitorWidget();

private:
    Ui::MonitorWidget *ui;
    QStringListModel *m_dumpListModel;

    const Memory& m_memory;
    Disassembler m_disassembler;
};

#endif // MONITORWIDGET_H
