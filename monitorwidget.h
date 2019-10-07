#ifndef MONITORWIDGET_H
#define MONITORWIDGET_H

#include <QDockWidget>

namespace Ui {
class MonitorWidget;
}

class MonitorWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit MonitorWidget(QWidget *parent = nullptr);
    ~MonitorWidget();

private:
    Ui::MonitorWidget *ui;
};

#endif // MONITORWIDGET_H
