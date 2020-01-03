#pragma once

#include <QWidget>

namespace Ui {
class CentralWidget;
}

class CentralWidget : public QWidget
{
  Q_OBJECT

public:
  explicit CentralWidget(QWidget* parent, QWidget* assemblerWidget, QWidget* memoryWidget, QWidget* disassemblerWidget);
  bool isVisible(const QWidget*) const;
  ~CentralWidget();

private:
  Ui::CentralWidget *ui;
  QWidget* const assemblerWidget;
  QWidget* const memoryWidget;
  QWidget* const disassemblerWidget;
};
