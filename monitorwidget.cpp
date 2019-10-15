#include "monitorwidget.h"
#include "ui_monitorwidget.h"
#include "disassembler.h"
#include <QFontDatabase>

MonitorWidget::MonitorWidget(QWidget *parent, const Memory& memory) :
    QDockWidget(parent),
    ui(new Ui::MonitorWidget),
    m_memory(memory),
    m_disassembler(memory)
{
    ui->setupUi(this);

    auto font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(13);
    ui->dumpListView->setFont(font);

    m_dumpListModel = new QStringListModel(this);
    ui->dumpListView->setModel(m_dumpListModel);

    QStringList list;
    for(int i=0; i<5; i++) {
        list.append(m_disassembler.disassemble());
        m_disassembler.step();
    }
    m_dumpListModel->setStringList(list);

    ui->dumpListView->setCurrentIndex(m_dumpListModel->index(1));
}

MonitorWidget::~MonitorWidget()
{
    delete ui;
}
