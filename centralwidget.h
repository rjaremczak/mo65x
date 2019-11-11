#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QWidget>

namespace Ui {
class CentralWidget;
}

class CentralWidget : public QWidget
{
  Q_OBJECT

public:
  explicit CentralWidget(QWidget* parent, QWidget* assemblerWidget, QWidget* memoryWidget);
  ~CentralWidget();

private:
  Ui::CentralWidget *ui;
  QWidget* assemblerWidget;
  QWidget* memoryWidget;
};

#endif // CENTRALWIDGET_H
