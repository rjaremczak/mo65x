#include "memorywidget.h"
#include "ui_memorywidget.h"
#include <QFileDialog>
#include <QMessageBox>

MemoryWidget::MemoryWidget(QWidget* parent, System* system) : QDockWidget(parent), ui(new Ui::MemoryWidget), system(system) {
  ui->setupUi(this);

  connect(ui->loadMemory, &QToolButton::clicked, this, &MemoryWidget::loadMemoryFromFile);
  connect(ui->fillMemory, &QToolButton::clicked, this, &MemoryWidget::fillMemory);
}

MemoryWidget::~MemoryWidget() {
  delete ui;
}

void MemoryWidget::loadMemoryFromFile() {
  uint16_t first = static_cast<uint16_t>(ui->loadMemoryFirst->value());
  QFileDialog::getOpenFileContent("", [&](const QString fileName, const QByteArray& fileContent) {
    QString title = "Load memory";
    if (fileName.isEmpty()) {
      QMessageBox::warning(this, title, "file load error");
    } else {
      auto size = system->loadMemory(first, fileContent);
      if (size == static_cast<size_t>(fileContent.size())) {
        QMessageBox::information(this, title, QString::asprintf("Loaded %d B at address 0x%04X", fileContent.size(), first));
      } else {
        QMessageBox::warning(this, title, QString::asprintf("Loaded %d B at address 0x%04X", fileContent.size(), first));
      }
    }
  });
}

void MemoryWidget::fillMemory() {
  system->fillMemory(ui->fillMemoryFirst->wordValue(), ui->fillMemoryLast->wordValue(), ui->fillMemoryValue->byteValue());
}
