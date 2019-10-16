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
    enum ViewMode { Asm, Hex, Txt };

    explicit MonitorWidget(QWidget *parent, const Memory& );
    ~MonitorWidget();

protected:
    void resizeEvent(QResizeEvent*) override;

private:
    Ui::MonitorWidget *ui;
    const Memory& m_memory;
    Disassembler m_disassembler;

    int rowsInView() const;
    void initView();
    void updateView();
};

#endif // MONITORWIDGET_H
