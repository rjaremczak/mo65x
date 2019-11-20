#pragma once

#include "assembler.h"
#include <QWidget>

namespace Ui {
class AssemblerWidget;
}

class AssemblerWidget : public QWidget {
  Q_OBJECT

public:
  explicit AssemblerWidget(QWidget* parent, Memory& memory);
  ~AssemblerWidget();

signals:
  void fileLoaded(const QString&);
  void fileSaved(const QString&);
  void codeWritten(AddressRange);
  void operationCompleted(const QString& message, bool success = true);
  void programCounterChanged(uint16_t);

public slots:
  void loadFile(const QString& fname);
  void saveFile(const QString& fname);

private:
  Ui::AssemblerWidget* ui;
  QString fileName;
  Assembler assembler;

  Memory& memory; // considered not clean, to be refactored some time...

  bool process(Memory&);

private slots:
  void newFile();
  void loadEditorFile();
  void saveEditorFile();
  void saveEditorFileAs();
  void assembleSourceCode();
};
