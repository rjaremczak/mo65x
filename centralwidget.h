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
  explicit CentralWidget(QWidget* parent, QWidget* assemblerWidget_, QWidget* memoryWidget_);
  ~CentralWidget();

private:
  Ui::CentralWidget *ui;
  QWidget* assemblerWidget_;
  QWidget* memoryWidget_;
};

#endif // CENTRALWIDGET_H
