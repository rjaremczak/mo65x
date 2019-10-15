#include "monitorwidget.h"
#include "ui_monitorwidget.h"
#include "disassembler.h"

MonitorWidget::MonitorWidget(QWidget *parent, const Memory& memory) :
    QDockWidget(parent),
    ui(new Ui::MonitorWidget),
    m_memory(memory),
    m_disassembler(memory)
{
    ui->setupUi(this);

    m_dumpListModel = new QStringListModel(this);
    ui->dumpListView->setModel(m_dumpListModel);

    /*
    QStringList list;
    list.append("0800 02 20 43 LDA 4320");
    list.append("test 1");
    m_dumpListModel->setStringList(list);
    */

    //m_dumpListModel->setStringList(m_disassembler.format(5));

    ui->dumpListView->setCurrentIndex(m_dumpListModel->index(1));
}

MonitorWidget::~MonitorWidget()
{
    delete ui;
}
