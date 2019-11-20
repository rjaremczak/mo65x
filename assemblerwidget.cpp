#include "assemblerwidget.h"
#include "assembler.h"
#include "formatters.h"
#include "ui_assemblerwidget.h"
#include "uitools.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QMetaEnum>
#include <QMetaType>
#include <QTextBlock>
#include <QTextStream>

AssemblerWidget::AssemblerWidget(QWidget* parent) : QWidget(parent), ui(new Ui::AssemblerWidget) {
  ui->setupUi(this);
  connect(ui->newFile, &QAbstractButton::clicked, this, &AssemblerWidget::newFile);
  connect(ui->loadFile, &QAbstractButton::clicked, this, &AssemblerWidget::loadEditorFile);
  connect(ui->saveFile, &QAbstractButton::clicked, this, &AssemblerWidget::saveEditorFile);
  connect(ui->saveFileAs, &QAbstractButton::clicked, this, &AssemblerWidget::saveEditorFileAs);
  connect(ui->assembleSourceCode, &QAbstractButton::clicked, this, &AssemblerWidget::assembleSourceCode);
  connect(ui->goToOrigin, &QAbstractButton::clicked, [&] { emit programCounterChanged(assembler.origin()); });

  setMonospaceFont(ui->sourceCode);
}

AssemblerWidget::~AssemblerWidget() {
  delete ui;
}

void AssemblerWidget::loadFile(const QString& fname) {
  QFile file(fname);
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    ui->sourceCode->setPlainText(QTextStream(&file).readAll());
    fileName = fname;
    emit fileLoaded(fname);
  }
}

void AssemblerWidget::saveFile(const QString& fname) {
  QFile file(fname);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream(&file) << ui->sourceCode->toPlainText();
    fileName = fname;
    emit fileSaved(fname);
  }
}

void AssemblerWidget::loadEditorFile() {
  QFileDialog::getOpenFileContent("", [&](const QString fileName, const QByteArray& fileContent) {
    QString title = tr("Load File");
    if (fileName.isEmpty()) {
      QMessageBox::warning(this, title, tr("loading error"));
    } else {
      ui->sourceCode->setPlainText(fileContent);
      emit fileLoaded(fileName);
    }
  });
}

void AssemblerWidget::saveEditorFile() {
  if (fileName.isEmpty()) fileName = QFileDialog::getSaveFileName(this, "", tr("Save File"));
  if (!fileName.isEmpty()) saveFile(fileName);
}

void AssemblerWidget::saveEditorFileAs() {
  if (auto fname = QFileDialog::getSaveFileName(this, tr("Save File"), QFileInfo(fileName).absolutePath()); !fname.isEmpty())
    saveFile(fname);
}

bool AssemblerWidget::process() {
  QString src = ui->sourceCode->toPlainText();
  QTextStream is(&src, QIODevice::ReadOnly);
  int lineNum = 0;
  while (!is.atEnd()) {
    const auto line = is.readLine();
    if (line.isNull()) break;
    if (auto result = assembler.process(line); result != AssemblerResult::Ok) {
      emit operationCompleted(tr("%1 at line %2").arg(assemblerResultStr(result)).arg(lineNum + 1), true);
      auto block = ui->sourceCode->document()->findBlockByLineNumber(lineNum);
      auto cursor = ui->sourceCode->textCursor();
      cursor.setPosition(block.position() + block.length() - 1, QTextCursor::MoveAnchor);
      cursor.setPosition(block.position(), QTextCursor::KeepAnchor);
      ui->sourceCode->setTextCursor(cursor);
      ui->sourceCode->setFocus();
      return false;
    }
    lineNum++;
  }
  return true;
}

void AssemblerWidget::newFile() {
  fileName.clear();
  ui->sourceCode->clear();
}

void AssemblerWidget::assembleSourceCode() {
  assembler.resetOrigin();
  assembler.clearCode();
  assembler.clearSymbols();
  assembler.changeMode(Assembler::Mode::ScanForSymbols);
  if (process()) {
    assembler.resetOrigin();
    assembler.changeMode(Assembler::Mode::EmitCode);
    if (process()) {
      emit machineCodeGenerated(assembler.origin(), assembler.code());
      emit operationCompleted(tr("assembled, origin: $%1, size: %2 B, symbols: %3")
                                  .arg(formatHexWord(assembler.origin()))
                                  .arg(assembler.symbols().size())
                                  .arg(assembler.code().size()));
      return;
    }
  }
  emit operationCompleted("assembly failed", false);
}
