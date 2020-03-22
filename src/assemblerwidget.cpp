#include "assemblerwidget.h"
#include "assembler.h"
#include "commonformatters.h"
#include "ui_assemblerwidget.h"
#include "uitools.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QMetaEnum>
#include <QMetaType>
#include <QTextBlock>
#include <QTextStream>

AssemblerWidget::AssemblerWidget(QWidget* parent, Memory& memory)
    : QWidget(parent), ui(new Ui::AssemblerWidget), m_assembler(memory, m_symbols) {
  ui->setupUi(this);
  connect(ui->newFile, &QAbstractButton::clicked, this, &AssemblerWidget::newFile);
  connect(ui->loadFile, &QAbstractButton::clicked, this, &AssemblerWidget::loadEditorFile);
  connect(ui->saveFile, &QAbstractButton::clicked, this, &AssemblerWidget::saveEditorFile);
  connect(ui->saveFileAs, &QAbstractButton::clicked, this, &AssemblerWidget::saveEditorFileAs);
  connect(ui->assembleSourceCode, &QAbstractButton::clicked, this, &AssemblerWidget::assembleSourceCode);
  connect(ui->goToOrigin, &QAbstractButton::clicked, [&] { emit programCounterChanged(m_assembler.affectedAddressRange().first); });

  setMonospaceFont(ui->sourceCode);
}

AssemblerWidget::~AssemblerWidget() {
  delete ui;
}

void AssemblerWidget::loadFile(const QString& fname) {
  QFile file(fname);
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    ui->sourceCode->setPlainText(QTextStream(&file).readAll());
    m_fileName = fname;
    emit fileLoaded(fname);
  }
}

void AssemblerWidget::saveFile(const QString& fname) {
  QFile file(fname);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream(&file) << ui->sourceCode->toPlainText();
    m_fileName = fname;
    emit fileSaved(fname);
  }
}

void AssemblerWidget::loadEditorFile() {
  QFileDialog::getOpenFileContent("", [&](const QString fname, const QByteArray& fileContent) {
    QString title = tr("Load File");
    if (fname.isEmpty()) {
      QMessageBox::warning(this, title, tr("loading error"));
    } else {
      ui->sourceCode->setPlainText(fileContent);
      m_fileName = fname;
      emit fileLoaded(fname);
    }
  });
}

void AssemblerWidget::saveEditorFile() {
  if (m_fileName.isEmpty()) m_fileName = QFileDialog::getSaveFileName(this, "", tr("Save File"));
  if (!m_fileName.isEmpty()) saveFile(m_fileName);
}

void AssemblerWidget::saveEditorFileAs() {
  if (auto fname = QFileDialog::getSaveFileName(this, tr("Save File"), QFileInfo(m_fileName).absolutePath()); !fname.isEmpty())
    saveFile(fname);
}

std::optional<QString> AssemblerWidget::process() {
  QString src = ui->sourceCode->toPlainText();
  QTextStream is(&src, QIODevice::ReadOnly);
  int lineNum = 0;
  while (!is.atEnd()) {
    const auto line = is.readLine();
    if (line.isNull()) break;
    if (auto result = m_assembler.processLine(line.toStdString()); result != AssemblyResult::Ok) {
      auto block = ui->sourceCode->document()->findBlockByLineNumber(lineNum);
      auto cursor = ui->sourceCode->textCursor();
      cursor.setPosition(block.position() + block.length() - 1, QTextCursor::MoveAnchor);
      cursor.setPosition(block.position(), QTextCursor::KeepAnchor);
      ui->sourceCode->setTextCursor(cursor);
      ui->sourceCode->setFocus();
      return tr("%1 at line %2").arg(formatAssemblyResult(result)).arg(lineNum + 1);
    }
    lineNum++;
  }
  return std::nullopt;
}

void AssemblerWidget::newFile() {
  m_fileName.clear();
  ui->sourceCode->clear();
  emit newFileCreated();
}

void AssemblerWidget::assembleSourceCode() {
  m_assembler.init();
  m_assembler.changeMode(Assembler::ProcessingMode::ScanForSymbols);
  if (auto ph0 = process()) {
    emit operationCompleted(*ph0, false);
    return;
  }

  m_assembler.initPreserveSymbols();
  m_assembler.changeMode(Assembler::ProcessingMode::EmitCode);
  if (auto ph1 = process()) {
    emit operationCompleted(*ph1, false);
    return;
  }

  emit codeWritten(m_assembler.affectedAddressRange());
  emit programCounterChanged(m_assembler.affectedAddressRange().first);
  emit operationCompleted(tr("%1 B written in range $%2-$%3, symbols: %4")
                              .arg(m_assembler.bytesWritten())
                              .arg(formatHexWord(m_assembler.affectedAddressRange().first))
                              .arg(formatHexWord(m_assembler.affectedAddressRange().last))
                              .arg(m_symbols.size()));
}
