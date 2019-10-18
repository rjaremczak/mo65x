#ifndef MONITORWIDGET_H
#define MONITORWIDGET_H

#include <QDockWidget>
#include <QStringListModel>
#include "disassembler.h"
#include "system.h"

namespace Ui {
class MonitorWidget;
}

class MonitorWidget : public QDockWidget
{
    Q_OBJECT

public:
    enum ViewMode { Asm, Hex, Txt };

    explicit MonitorWidget(QWidget *parent, const Memory&);
    ~MonitorWidget() override;

signals:
    void addressChanged(uint16_t);

public slots:
    void changeAddress(uint16_t);
    void updateMemoryView(size_t start, size_t size);

protected:
    void resizeEvent(QResizeEvent*) override;

private:
    Ui::MonitorWidget *ui;

    Disassembler m_disassembler;
    uint16_t m_address;

    int rowsInView() const;
    void initView();
    void updateView();
};

#endif // MONITORWIDGET_H
