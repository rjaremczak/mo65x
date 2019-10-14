#include "monitorwidget.h"
#include "ui_monitorwidget.h"

MonitorWidget::MonitorWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::MonitorWidget)
{
    ui->setupUi(this);

    m_dumpListModel = new QStringListModel(this);
    ui->dumpListView->setModel(m_dumpListModel);

    QStringList list;
    list.append("0800 02 20 43 LDA 4320");
    list.append("test 1");
    m_dumpListModel->setStringList(list);

    ui->dumpListView->setCurrentIndex(m_dumpListModel->index(1));
}

MonitorWidget::~MonitorWidget()
{
    delete ui;
}
