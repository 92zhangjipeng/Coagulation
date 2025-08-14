#include "tipcustomwidget.h"
#include "ui_tipcustomwidget.h"
#include <QPainter>

tipcustomwidget::tipcustomwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tipcustomwidget)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    setBackColor(0, 0, 0);
    setDirect(DIRECT::DIRECT_BOTTOM, 0.6);

    QFont f;
    f.setFamily("楷体");
    f.setPixelSize(14);
    setContentFont(f);
    setContent("I am a default text");

    setLeftTopMargin();

}

tipcustomwidget::~tipcustomwidget()
{
    delete ui;
}

void tipcustomwidget::setBackColor(int r, int g, int b, int a)
{
    m_backColor = QColor(r,g,b,a);
}

void tipcustomwidget::setDirect(DIRECT direct, double size)
{
    m_direct = direct;
    m_posSize = size;
}

void tipcustomwidget::setContentFont(QFont font)
{
    ui->label->setFont(font);
}

void tipcustomwidget::setContent(const QString &content, QColor color)
{
    ui->label->setText(content);
    ui->label->setStyleSheet(QString("color: rgb(%1, %2, %3)")
                             .arg(color.red())
                             .arg(color.green())
                             .arg(color.blue()));
}

void tipcustomwidget::setLeftTopMargin(int leftMargin, int topMargin)
{
    m_leftMargin = leftMargin;
    m_topMargin = topMargin;
    this->setContentsMargins(m_leftMargin + TRANSPARENT_LENGTH,
                             m_topMargin + TRANSPARENT_LENGTH,
                             m_leftMargin + TRANSPARENT_LENGTH,
                             m_topMargin + TRANSPARENT_LENGTH);
}

void tipcustomwidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);


    painter.setPen(Qt::NoPen);
    painter.setBrush(m_backColor);

    //相对于当前布局的起点坐标
    painter.drawRoundedRect(TRANSPARENT_LENGTH, TRANSPARENT_LENGTH,
                            width() - TRANSPARENT_LENGTH * 2,
                            height() - TRANSPARENT_LENGTH * 2, 4, 4);
    QPointF points[3];

    switch (m_direct) {
    case DIRECT::DIRECT_LEFT: {
        points[0] = QPointF(TRANSPARENT_LENGTH,
                            height() * m_posSize - DEF_TRIANGLE_HEIGHT);
        points[1] = QPointF(TRANSPARENT_LENGTH - DEF_TRIANGLE_HEIGHT,
                            height() * m_posSize);
        points[2] = QPointF(TRANSPARENT_LENGTH,
                            height() * m_posSize + DEF_TRIANGLE_HEIGHT);
        break;
    }

    case DIRECT::DIRECT_TOP: {
        points[0] = QPointF(width() * m_posSize - DEF_TRIANGLE_HEIGHT,
                            TRANSPARENT_LENGTH);
        points[1] = QPointF(width() * m_posSize,
                            TRANSPARENT_LENGTH - DEF_TRIANGLE_HEIGHT);
        points[2] = QPointF(width() * m_posSize + DEF_TRIANGLE_HEIGHT,
                            TRANSPARENT_LENGTH);
        break;
    }

    case DIRECT::DIRECT_RIGHT: {
        points[0] = QPointF(width() - TRANSPARENT_LENGTH,
                            height() * m_posSize - DEF_TRIANGLE_HEIGHT);
        points[1] = QPointF(width() - DEF_TRIANGLE_HEIGHT, height() * m_posSize);
        points[2] = QPointF(width() - TRANSPARENT_LENGTH,
                            height() * m_posSize + DEF_TRIANGLE_HEIGHT);
        break;
    }

    case DIRECT::DIRECT_BOTTOM: {
        points[0] = QPointF(width() * m_posSize - DEF_TRIANGLE_HEIGHT,
                            height() - TRANSPARENT_LENGTH);
        points[1] = QPointF(width() * m_posSize, height() - DEF_TRIANGLE_HEIGHT);
        points[2] = QPointF(width() * m_posSize + DEF_TRIANGLE_HEIGHT,
                            height() - TRANSPARENT_LENGTH);
        break;
    }

    default:
        break;
    }

    painter.drawPolygon(points, 3);
}






