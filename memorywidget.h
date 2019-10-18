#ifndef MEMORYWIDGET_H
#define MEMORYWIDGET_H

#include <QDockWidget>
#include "system.h"

namespace Ui {
class MemoryWidget;
}

class MemoryWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit MemoryWidget(QWidget *parent, System* emulator);
    ~MemoryWidget();

public slots:

private slots:
    void loadMemoryFromFile();
    void fillMemory();

private:
    Ui::MemoryWidget *ui;
    System* m_system;
};

#endif // MEMORYWIDGET_H
