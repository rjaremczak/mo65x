#ifndef MEMORYWIDGET_H
#define MEMORYWIDGET_H

#include <QDockWidget>

namespace Ui {
class MemoryWidget;
}

class MemoryWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit MemoryWidget(QWidget *parent = nullptr);
    ~MemoryWidget();

private:
    Ui::MemoryWidget *ui;
};

#endif // MEMORYWIDGET_H
