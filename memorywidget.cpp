#include "memorywidget.h"
#include "ui_memorywidget.h"
#include <QFileDialog>

MemoryWidget::MemoryWidget(QWidget *parent, System* emulator) :
    QDockWidget(parent),
    ui(new Ui::MemoryWidget),
    m_emulator(emulator)
{
    ui->setupUi(this);
    connect(ui->loadMemory, &QToolButton::clicked, this, &MemoryWidget::loadMemoryFromFile);
}

MemoryWidget::~MemoryWidget()
{
    delete ui;
}

void MemoryWidget::loadMemoryFromFile()
{
    uint16_t addr = static_cast<uint16_t>(ui->loadMemoryAddr->value());
    QFileDialog::getOpenFileContent("", [&](const QString fileName, const QByteArray& fileContent){
        if(fileName.isEmpty()) {
            qDebug("error");
        } else {
            m_emulator->loadMemory(addr, fileContent);
        }
    });
}
