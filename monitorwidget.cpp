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

#ifdef __APPLE__
    font.setPointSize(13);
#endif

    ui->dumpView->setFont(font);
}

void MonitorWidget::updateView()
{
    QString html("<div style='white-space:pre; display:inline-block'>");
    int rows = rowsInView();
    if(rows--) {
        html.append("<span style='color:lightgreen'>");
        html.append(m_disassembler.disassemble()).append("</span><br>");
        m_disassembler.step();
        html.append("<span style='color:darkseagreen'>");
        while(rows--) {
            html.append(m_disassembler.disassemble()+"<br>");
            m_disassembler.step();
        }
        html.append("</span>");
    }
    html.append("</div>");
    ui->dumpView->setHtml(html);
}
