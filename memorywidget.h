#ifndef MEMORYWIDGET_H
#define MEMORYWIDGET_H

#include <QDockWidget>
#include "emulator.h"

namespace Ui {
class MemoryWidget;
}

class MemoryWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit MemoryWidget(QWidget *parent, Emulator* emulator);
    ~MemoryWidget();

public slots:
    void loadMemoryFromFile();

private:
    Ui::MemoryWidget *ui;
    Emulator* m_emulator;
};

#endif // MEMORYWIDGET_H
