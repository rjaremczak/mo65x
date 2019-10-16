#ifndef EMULATORWIDGET_H
#define EMULATORWIDGET_H

#include <QDockWidget>

namespace Ui {
class EmulatorWidget;
}

class EmulatorWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit EmulatorWidget(QWidget *parent = nullptr);
    ~EmulatorWidget();

private:
    Ui::EmulatorWidget *ui;
};

#endif // EMULATORWIDGET_H
