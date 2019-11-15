#pragma once

#include "commons.h"
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
  void programCounterChanged(quint16);

public slots:
  void updateMemoryContent(quint16 first, quint16 last);
  void updateState(CpuInfo);
  void updateDisassemblerView();
  void changeProgramCounter(quint16);

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
