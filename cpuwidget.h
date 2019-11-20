#pragma once

#include "addressrange.h"
#include "defs.h"
#include "disassembler.h"
#include "emulatorstate.h"
#include <QDockWidget>

namespace Ui {
class CpuWidget;
}

class CpuWidget : public QDockWidget {
  Q_OBJECT
  Q_ENUM(ExecutionState)

public:
  explicit CpuWidget(QWidget* parent, const Memory&);
  ~CpuWidget() override;

signals:
  void startStepExecution();
  void startContinuousExecution();
  void stopExecutionRequested();
  void clearStatisticsRequested();
  void resetRequested();
  void nmiRequested();
  void irqRequested();
  void programCounterChanged(uint16_t);
  void stackPointerChanged(uint16_t);
  void registerAChanged(uint8_t);
  void registerXChanged(uint8_t);
  void registerYChanged(uint8_t);

public slots:
  void updateMemoryView(AddressRange);
  void processState(EmulatorState);
  void updateDisassemblerView();
  void changeProgramCounter(uint16_t);

protected:
  void resizeEvent(QResizeEvent*) override;

private:
  Ui::CpuWidget* ui;

  const Memory& memory;
  Disassembler disassembler;
  AddressRange disassemblerRange;

  int rowsInView() const;

private slots:
  void skipInstruction();
};
