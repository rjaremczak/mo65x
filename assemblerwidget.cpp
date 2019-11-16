#include "assemblerwidget.h"
#include "assembler.h"
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

  connect(ui->loadFile, &QToolButton::clicked, this, &AssemblerWidget::loadEditorFile);
  connect(ui->saveFile, &QToolButton::clicked, this, &AssemblerWidget::saveEditorFile);
  connect(ui->saveFileAs, &QToolButton::clicked, this, &AssemblerWidget::saveEditorFileAs);
  connect(ui->assembleSourceCode, &QToolButton::clicked, this, &AssemblerWidget::assembleSourceCode);
  setMonospaceFont(ui->sourceCode);
}

AssemblerWidget::~AssemblerWidget() {
  delete ui;
}

void AssemblerWidget::loadFile(const QString& fname) {
  QFile file(fname);
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    ui->sourceCode->setPlainText(QTextStream(&file).readAll());
    fileName_ = fname;
    emit fileLoaded(fname);
  }
}

void AssemblerWidget::saveFile(const QString& fname) {
  QFile file(fname);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream(&file) << ui->sourceCode->toPlainText();
    fileName_ = fname;
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
  if (fileName_.isEmpty()) fileName_ = QFileDialog::getSaveFileName(this, "", tr("Save File"));
  if (!fileName_.isEmpty()) saveFile(fileName_);
}

void AssemblerWidget::saveEditorFileAs() {
  if (auto fname = QFileDialog::getSaveFileName(this, tr("Save File"), QFileInfo(fileName_).absolutePath()); !fname.isEmpty())
    saveFile(fname);
}

bool AssemblerWidget::process() {
  QString src = ui->sourceCode->toPlainText();
  QTextStream is(&src, QIODevice::ReadOnly);
  int lineNum = 0;
  while (!is.atEnd()) {
    const auto line = is.readLine();
    if (line.isNull()) break;
    if (auto result = assembler_.process(line); result != Assembler::Result::Ok) {
      QMessageBox::warning(this, tr("Assembly Error"),
                           QString("%1 at line %2")
                               .arg(QMetaEnum::fromType<Assembler::Result>().valueToKey(static_cast<int>(result)))
                               .arg(lineNum + 1));

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

void AssemblerWidget::assembleSourceCode() {
  assembler_.resetOrigin();
  assembler_.clearCode();
  assembler_.clearSymbols();
  assembler_.changeMode(Assembler::Mode::ScanForSymbols);
  if (!process()) return;

  assembler_.resetOrigin();
  assembler_.changeMode(Assembler::Mode::EmitCode);
  if (!process()) return;

  QMessageBox::information(this, tr("Machine Code Generated"),
                           tr("origin: %1\nsymbols: %2\nsize: %3 B")
                               .arg(QString::asprintf("$%04X", assembler_.origin()))
                               .arg(assembler_.symbols().size())
                               .arg(assembler_.code().size()));
  emit machineCodeGenerated(assembler_.origin(), assembler_.code());
}
