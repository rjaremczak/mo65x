#ifndef MONITORWIDGET_H
#define MONITORWIDGET_H

#include <QDockWidget>
#include <QStringListModel>

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
    QStringListModel *m_dumpListModel;
};

#endif // MONITORWIDGET_H
