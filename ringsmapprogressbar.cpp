#include "ringsmapprogressbar.h"
#include <QPainter>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

RingsMapProgressbar::RingsMapProgressbar(QWidget *parent) : QWidget(parent),
    m_rotateAngle(0),
    m_persent(0)
{

}

void RingsMapProgressbar::setPersent(double persent,QString passage)
{
    if(persent != m_persent)
    {
        m_persent = persent*100/300;
        update();
    }
    m_passage = passage;
}
void RingsMapProgressbar::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    m_rotateAngle = (360*m_persent)/100;
    int side = qMin(width(), height());
    QRectF outRect(0, 0, side, side);
    QRectF inRect(20, 20, side-40, side-40);
    QString valueStr = QString("%1%\n%2").arg(QString::number(m_persent)).arg(m_passage);
    //画底圆
    p.setPen(Qt::NoPen);
    QPixmap backMap = QPixmap(":/Picture/progress_back.png");
    p.drawPixmap(outRect, backMap, outRect);
    //画内弧
    QPixmap frontMap = QPixmap(":/Picture/progress_front.png");
    p.setBrush(QBrush(frontMap));
    p.drawPie(outRect, (90-m_rotateAngle)*16, m_rotateAngle*16);
    //画文字
    QFont f = QFont("Microsoft YaHei", 15, QFont::Bold);
    p.setFont(f);
    p.setPen(QColor("#DDDDDD"));
    p.drawText(inRect, Qt::AlignCenter, valueStr);
}

