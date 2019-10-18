#include "monitorwidget.h"
#include "ui_monitorwidget.h"
#include "disassembler.h"
#include <QFontDatabase>

MonitorWidget::MonitorWidget(QWidget *parent, const Memory& memory) :
    QDockWidget(parent),
    ui(new Ui::MonitorWidget),
    m_disassembler(memory)
{
    ui->setupUi(this);
    initView();

    connect(ui->progAddr, QOverload<int>::of(&QSpinBox::valueChanged), [&]{ changeAddress(static_cast<uint16_t>(ui->progAddr->value())); });
}

MonitorWidget::~MonitorWidget()
{
    delete ui;
}

void MonitorWidget::changeAddress(uint16_t pc)
{
    if(m_address != pc) {
        m_address = pc;
        updateView();
        emit addressChanged(pc);
    }
}

void MonitorWidget::updateMemoryView(size_t start, size_t size)
{
    updateView();
}

void MonitorWidget::resizeEvent(QResizeEvent *)
{
    //qDebug("resize");
}

int MonitorWidget::rowsInView() const
{
    return 30;
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
    m_disassembler.setAddr(m_address);
    QString html("<div style='white-space:pre; display:inline-block'>");
    int rows = rowsInView();
    if(rows--) {
        html.append("<div style='color:black; background-color: lightgreen'>");
        html.append(m_disassembler.disassemble()).append("</div>");
        m_disassembler.step();
        html.append("<div style='color:darkseagreen'>");
        while(rows--) {
            html.append(m_disassembler.disassemble()+"<br>");
            m_disassembler.step();
        }
        html.append("</div>");
    }
    html.append("</div>");
    ui->dumpView->setHtml(html);
}
