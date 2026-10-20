#include "holegridwidget.h"

#include <QPainter>
#include <QMouseEvent>

namespace {
constexpr int kGap = 2;
constexpr int kMinHole = 16;
}

HoleGridWidget::HoleGridWidget(QWidget *parent)
    : QWidget(parent)
    , m_maxRow(0)
    , m_maxCol(0)
    , m_selected(-1)
    , m_hover(-1)
    , m_lastClicked(-1)
    , m_fit(CenterBlock)
    , m_preferredHole(28)
    , m_squareAspect(false)
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
}

void HoleGridWidget::setHoles(const QVector<HoleCell> &holes, int columns, FitMode fit, int preferredHolePx)
{
    m_holes = holes;
    m_fit = fit;
    m_preferredHole = preferredHolePx;
    m_maxRow = 0;
    m_maxCol = 0;

    if (columns < 0) {
        int minRow = 0;
        int minCol = 0;
        bool first = true;
        for (int i = 0; i < m_holes.size(); ++i) {
            if (m_holes[i].text.isEmpty()) {
                m_holes[i].text = QString::number(m_holes[i].number);
            }
            if (first) {
                minRow = m_holes[i].row;
                minCol = m_holes[i].col;
                first = false;
            }
            minRow = qMin(minRow, m_holes[i].row);
            minCol = qMin(minCol, m_holes[i].col);
            m_maxRow = qMax(m_maxRow, m_holes[i].row);
            m_maxCol = qMax(m_maxCol, m_holes[i].col);
        }
        for (int i = 0; i < m_holes.size(); ++i) {
            m_holes[i].row -= minRow;
            m_holes[i].col -= minCol;
        }
        m_maxRow = qMax(0, m_maxRow - minRow);
        m_maxCol = qMax(0, m_maxCol - minCol);
    } else {
        if (columns < 1) {
            columns = qMax(1, m_holes.size());
        }
        for (int i = 0; i < m_holes.size(); ++i) {
            m_holes[i].row = i / columns;
            m_holes[i].col = i % columns;
            if (m_holes[i].text.isEmpty()) {
                m_holes[i].text = QString::number(m_holes[i].number);
            }
        }
        m_maxCol = qMax(0, columns - 1);
        m_maxRow = m_holes.isEmpty() ? 0 : ((m_holes.size() - 1) / columns);
    }
    updateGeometry();
    update();
}

void HoleGridWidget::setSelectedNumber(int number)
{
    if (m_selected == number) {
        return;
    }
    m_selected = number;
    update();
}

QSize HoleGridWidget::sizeHint() const
{
    const int cols = qMax(1, columnCount());
    const int rows = qMax(1, rowCount());
    const int hole = (m_preferredHole > 0) ? m_preferredHole : 40;
    const int cell = hole + kGap;
    return QSize(cols * cell + 8, rows * cell + 8);
}

QSize HoleGridWidget::minimumSizeHint() const
{
    const int cols = qMax(1, columnCount());
    const int rows = qMax(1, rowCount());
    const int cell = kMinHole + kGap;
    return QSize(cols * cell + 8, rows * cell + 8);
}

bool HoleGridWidget::hasHeightForWidth() const
{
    return m_squareAspect && columnCount() > 1 && rowCount() > 1;
}

int HoleGridWidget::heightForWidth(int w) const
{
    if (!hasHeightForWidth()) {
        return QWidget::heightForWidth(w);
    }
    const int cols = qMax(1, columnCount());
    const int rows = qMax(1, rowCount());
    return qMax(minimumSizeHint().height(), (w * rows + cols / 2) / cols);
}

void HoleGridWidget::setSquareAspect(bool on)
{
    if (m_squareAspect == on) {
        return;
    }
    m_squareAspect = on;
    updateGeometry();
    update();
}

QRect HoleGridWidget::holeRect(const HoleCell &cell) const
{
    const int cols = columnCount();
    const int rows = rowCount();
    if (cols <= 0 || rows <= 0 || width() < 8 || height() < 8) {
        return QRect();
    }

    if (m_fit == CenterBlock) {
        const int pad = 8;
        const int gap = 6;
        const int availW = qMax(1, width() - pad * 2);
        const int availH = qMax(1, height() - pad * 2);
        int hole = qMin((availW - (cols - 1) * gap) / cols,
                        (availH - (rows - 1) * gap) / rows);
        hole = qBound(kMinHole, hole, 56);
        const int gridW = cols * hole + (cols - 1) * gap;
        const int gridH = rows * hole + (rows - 1) * gap;
        const int ox = (width() - gridW) / 2;
        const int oy = (height() - gridH) / 2;
        return QRect(ox + cell.col * (hole + gap),
                     oy + cell.row * (hole + gap),
                     hole,
                     hole);
    }

    const int pad = 4;
    const int availW = qMax(1, width() - pad * 2);
    const int availH = qMax(1, height() - pad * 2);
    const int cellW = qMax(1, availW / cols);
    const int cellH = qMax(1, availH / rows);
    const int gap = qBound(6, qMin(cellW, cellH) / 5, 20);
    int hole = qMin(cellW, cellH) - gap;
    hole = qMax(kMinHole, hole);
    if (m_preferredHole > 0) {
        hole = qMin(hole, m_preferredHole);
    }
    if (cols <= 2 && cellH > cellW + 8) {
        hole = qMin(hole, qMax(kMinHole, cellW * 2 / 3));
    }
    const int x = pad + cell.col * cellW + (cellW - hole) / 2;
    const int y = pad + cell.row * cellH + (cellH - hole) / 2;
    return QRect(x, y, hole, hole);
}

int HoleGridWidget::holeAt(const QPoint &pos) const
{
    for (int i = 0; i < m_holes.size(); ++i) {
        const QRect r = holeRect(m_holes.at(i));
        if (r.contains(pos)) {
            return m_holes.at(i).number;
        }
    }
    return -1;
}

void HoleGridWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(rect(), QColor(255, 255, 255));

    for (int i = 0; i < m_holes.size(); ++i) {
        const HoleCell &cell = m_holes.at(i);
        const QRect r = holeRect(cell);
        if (!r.isValid()) {
            continue;
        }
        const bool selected = (cell.number == m_selected);
        const bool hover = (cell.number == m_hover);

        QColor fill(239, 247, 255);
        if (hover) {
            fill = QColor(217, 234, 251);
        }
        if (selected) {
            fill = QColor(255, 236, 236);
        }

        p.setBrush(fill);
        p.setPen(QPen(selected ? QColor(198, 40, 40) : QColor(90, 155, 194),
                      selected ? 2 : 1));
        p.drawEllipse(r);

        if (selected) {
            const QPoint c = r.center();
            const int arm = qMax(3, r.width() / 4);
            p.setPen(QPen(QColor(198, 40, 40), 1));
            p.drawLine(c.x() - arm, c.y(), c.x() + arm, c.y());
            p.drawLine(c.x(), c.y() - arm, c.x(), c.y() + arm);
        }

        QFont f = font();
        f.setPixelSize(qBound(8, r.height() / 3, 16));
        p.setFont(f);
        p.setPen(selected ? QColor(10, 63, 90) : QColor(26, 90, 122));
        const QString label = cell.text.isEmpty() ? QString::number(cell.number) : cell.text;
        p.drawText(r, Qt::AlignCenter, label);
    }
}

void HoleGridWidget::mousePressEvent(QMouseEvent *event)
{
    const int number = holeAt(event->pos());
    if (number < 0) {
        QWidget::mousePressEvent(event);
        return;
    }
    m_lastClicked = number;
    m_selected = number;
    update();
    emit holeClicked();
}

void HoleGridWidget::mouseMoveEvent(QMouseEvent *event)
{
    const int number = holeAt(event->pos());
    if (m_hover != number) {
        m_hover = number;
        update();
    }
}

void HoleGridWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if (m_hover != -1) {
        m_hover = -1;
        update();
    }
}
