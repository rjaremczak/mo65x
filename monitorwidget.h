#ifndef MONITORWIDGET_H
#define MONITORWIDGET_H

#include <QDockWidget>
#include <QStringListModel>
#include "disassembler.h"
#include "emulator.h"

namespace Ui {
class MonitorWidget;
}

class MonitorWidget : public QDockWidget
{
    Q_OBJECT

public:
    enum ViewMode { Asm, Hex, Txt };

    explicit MonitorWidget(QWidget *parent, Emulator*);
    ~MonitorWidget();

protected:
    void resizeEvent(QResizeEvent*) override;

private:
    Ui::MonitorWidget *ui;
    Emulator* m_emulator;

    Disassembler m_disassembler;

    int rowsInView() const;
    void initView();
    void updateView();
};

#endif // MONITORWIDGET_H
