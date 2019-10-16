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
    initView();
    updateView();
}

MonitorWidget::~MonitorWidget()
{
    delete ui;
}

void MonitorWidget::resizeEvent(QResizeEvent *)
{
    //qDebug("resize");
}

int MonitorWidget::rowsInView() const
{
    return 15;
}

void MonitorWidget::initView()
{
    ui->viewModeGroup->setId(ui->viewModeAsm, Asm);
    ui->viewModeGroup->setId(ui->viewModeHex, Hex);
    ui->viewModeGroup->setId(ui->viewModeTxt, Txt);
    ui->viewModeAsm->setChecked(true);

    auto font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    //font.setPointSize(13);
    ui->dumpListView->setFont(font);
    ui->dumpListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->dumpListView->setSelectionMode(QAbstractItemView::NoSelection);

    m_dumpListModel = new QStringListModel(this);
    ui->dumpListView->setModel(m_dumpListModel);
}

void MonitorWidget::updateView()
{
    QStringList list;
    int rows = rowsInView();
    while(rows--) {
        list.append(m_disassembler.disassemble());
        m_disassembler.step();
    }
    m_dumpListModel->setStringList(list);
    ui->dumpListView->setCurrentIndex(m_dumpListModel->index(1));
}
