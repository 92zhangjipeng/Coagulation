#include "testcupdraw.h"
#include "ui_testcupdraw.h"
#include <QDesktopWidget>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

TestCupDraw::TestCupDraw(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestCupDraw)
{
    ui->setupUi(this);

    this->setMouseTracking(true);

    QDesktopWidget* pDesktopWidget = QApplication::desktop();

    QRect screenRect = pDesktopWidget->screenGeometry();

    int screenWidth = screenRect.width();

    //int screenHeight = screenRect.height();

    int need_show_width = screenWidth - 120;

    m_BarList<<ui->bar_test_1<<ui->bar_test_2<<ui->bar_test_3<<ui->bar_test_4<<ui->bar_test_5<<ui->bar_test_6
             <<ui->bar_test_7<<ui->bar_test_8<<ui->bar_test_9<<ui->bar_test_10<<ui->bar_test_11<<ui->bar_test_12;

    for(int i = 0 ; i<m_BarList.size(); i++)
    {
        QRect  rect(need_show_width/12*i,0,120,120);

        m_BarList.at(i)->setGeometry(rect);

        QString passage_str = QString("通道%1").arg(i+1);

        m_BarList.at(i)->setPersent(0,passage_str);
    }
}
TestCupDraw::~TestCupDraw()
{
    delete ui;
}
void TestCupDraw::Read_Progress(int total,int passage)
{
    QString passage_str = QString("通道%1").arg(total +1);
    m_BarList.at(total)->setPersent(passage,passage_str);
    return;
}

void TestCupDraw::paintEvent(QPaintEvent*)
{
    m_paint = new QPainter;
    m_paint->begin(this);
    delete m_paint;
}
void TestCupDraw::mousePressEvent(QMouseEvent *)
{

    return;
}
