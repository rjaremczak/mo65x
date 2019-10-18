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
    void loadMemoryFromFile();

private:
    Ui::MemoryWidget *ui;
    System* m_emulator;
};

#endif // MEMORYWIDGET_H
