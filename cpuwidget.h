#pragma once

#include "addressrange.h"
#include "commondefs.h"
#include "disassemblerview.h"
#include "emulatorstate.h"
#include <QDockWidget>

namespace Ui {
class CpuWidget;
}

class CpuWidget : public QDockWidget {
  Q_OBJECT
  Q_ENUM(CpuState)

public:
  explicit CpuWidget(QWidget* parent, const Memory&);
  ~CpuWidget() override;

signals:
  void singleStepRequested();
  void continuousExecutionRequested();
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
  void updateMemory(AddressRange);
  void updateState(EmulatorState);
  void changeProgramCounter(uint16_t);

private:
  Ui::CpuWidget* ui;

  DisassemblerView* disassemblerView;
  const Memory& memory;

  int rowsInView() const;
  void updateSpecialCpuAddresses();
  void updateUI(CpuState);

private slots:
  void skipInstruction();
  void startContinuousExecution();
};
