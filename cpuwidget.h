#pragma once

#include "cpuinfo.h"
#include "disassembler.h"
#include <QDockWidget>

namespace Ui {
class CpuWidget;
}

class CpuWidget : public QDockWidget {
  Q_OBJECT

  Ui::CpuWidget* ui;

public:
  explicit CpuWidget(QWidget* parent, const Memory&);
  ~CpuWidget() override;

signals:
  void singleStepExecutionRequested();
  void continuousExecutionRequested();
  void stopExecutionRequested();
  void programCounterChangeRequested(uint16_t);

public slots:
  void updateMemoryContent(uint16_t start, uint16_t last);
  void updateState(CpuInfo);
  void updateDisassemblerView();

protected:
  void resizeEvent(QResizeEvent*) override;

private:
  const Memory& memory_;
  Disassembler disassembler_;
  uint16_t disassemblerFirstAddress_;
  uint16_t disassemblerLastAddress_;

  int rowsInView() const;

private slots:
  void skipInstruction();
};
