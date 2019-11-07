#ifndef EXECUTIONWIDGET_H
#define EXECUTIONWIDGET_H

#include "system.h"
#include <QDockWidget>

namespace Ui {
class ExecutionWidget;
}

class ExecutionWidget : public QDockWidget
{
  Q_OBJECT

  System* system;

public:
  explicit ExecutionWidget(QWidget* parent, System* system);
  ~ExecutionWidget();

signals:
  void pcChanged(uint16_t);

public slots:
  void changePC(uint16_t);
  void executeSingleStep();
  void startContinuousExecution();
  void stopContinuousExecution();
  void updateCpuState();

private:
  Ui::ExecutionWidget *ui;
};

#endif // EXECUTIONWIDGET_H
