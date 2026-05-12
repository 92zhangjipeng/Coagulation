// testtubeplate.cpp
#include "testtubeplate.h"
#include <QFontMetrics>
#include <QRadialGradient>
#include <QPainterPath>

TestTubePlate::TestTubePlate(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);

    // 默认所有试管为待使用状态
    for (int i = 0; i < m_rows; ++i) {
        for (int j = 0; j < m_columns; ++j) {
            m_tubeStates[qMakePair(i, j)] = TubeState::Pending;
        }
    }
}

void TestTubePlate::setRows(int rows)
{
    if (rows > 0 && m_rows != rows) {
        m_rows = rows;
        updateGeometry();
        update();
    }
}

void TestTubePlate::setColumns(int columns)
{
    if (columns > 0 && m_columns != columns) {
        m_columns = columns;
        updateGeometry();
        update();
    }
}

void TestTubePlate::setRingWidth(int width)
{
    if (width > 0 && m_ringWidth != width) {
        m_ringWidth = width;
        updateGeometry();
        update();
    }
}

void TestTubePlate::setBorderMargin(int margin)
{
    if (margin >= 0 && m_borderMargin != margin) {
        m_borderMargin = margin;
        updateGeometry();
        update();
    }
}

void TestTubePlate::setClickable(bool clickable)
{
    m_clickable = clickable;
    if (!clickable) {
        clearHighlight();
    }
}

void TestTubePlate::setTubeId(int row, int col, const QString& id)
{
    if (row >= 0 && row < m_rows && col >= 0 && col < m_columns) {
        m_tubeIds[qMakePair(row, col)] = id;
        update();
    }
}

QString TestTubePlate::tubeId(int row, int col) const
{
    return m_tubeIds.value(qMakePair(row, col), QString::number(row * m_columns + col + 1));
}

void TestTubePlate::setTubeState(int row, int col, TubeState state)
{
    if (row >= 0 && row < m_rows && col >= 0 && col < m_columns) {
        m_tubeStates[qMakePair(row, col)] = state;
        update();
    }
}

TubeState TestTubePlate::tubeState(int row, int col) const
{
    return m_tubeStates.value(qMakePair(row, col), TubeState::Pending);
}

void TestTubePlate::setAllTubesState(TubeState state)
{
    for (int i = 0; i < m_rows; ++i) {
        for (int j = 0; j < m_columns; ++j) {
            m_tubeStates[qMakePair(i, j)] = state;
        }
    }
    update();
}

void TestTubePlate::clearHighlight()
{
    if (m_highlightPos.x() != -1) {
        m_highlightPos = QPoint(-1, -1);
        update();
    }
}

int TestTubePlate::calculateTubeSize() const
{
    int minTubeDiameter = (m_ringWidth + 15) * 2;
    return minTubeDiameter;
}

QSize TestTubePlate::minimumSizeHint() const
{
    int tubeSize = calculateTubeSize();
    int minWidth = tubeSize * m_columns + m_borderMargin * 2;
    int minHeight = tubeSize * m_rows + m_borderMargin * 2;
    return QSize(minWidth, minHeight);
}

QSize TestTubePlate::sizeHint() const
{
    QSize minSize = minimumSizeHint();
    return QSize(minSize.width() * 1.5, minSize.height() * 1.5);
}

void TestTubePlate::calculateLayout()
{
    int margin = m_borderMargin;
    QRect availableRect = rect().adjusted(margin, margin, -margin, -margin);

    int cellWidth = availableRect.width() / m_columns;
    int cellHeight = availableRect.height() / m_rows;
    int cellSize = qMin(cellWidth, cellHeight);

    m_cellSize = QSize(cellSize, cellSize);

    int totalWidth = m_cellSize.width() * m_columns;
    int totalHeight = m_cellSize.height() * m_rows;

    m_startX = (width() - totalWidth) / 2;
    m_startY = (height() - totalHeight) / 2;
}

void TestTubePlate::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // 深色科技感背景
    QLinearGradient bgGradient(0, 0, 0, height());
    bgGradient.setColorAt(0, QColor(28, 35, 45));
    bgGradient.setColorAt(0.5, QColor(24, 30, 40));
    bgGradient.setColorAt(1, QColor(20, 25, 35));
    painter.fillRect(rect(), bgGradient);

    // 添加微妙的网格线
    painter.setPen(QPen(QColor(45, 55, 70, 60), 1));
    int gridSize = 40;
    for (int x = 0; x < width(); x += gridSize) {
        painter.drawLine(x, 0, x, height());
    }
    for (int y = 0; y < height(); y += gridSize) {
        painter.drawLine(0, y, width(), y);
    }

    // 顶部高光效果
    QLinearGradient topGlow(0, 0, 0, 50);
    topGlow.setColorAt(0, QColor(100, 130, 160, 40));
    topGlow.setColorAt(1, QColor(100, 130, 160, 0));
    painter.fillRect(0, 0, width(), 50, topGlow);

    calculateLayout();

    // 试管盘底座
    QRect plateRect(m_startX - 12, m_startY - 12,
                    m_cellSize.width() * m_columns + 24,
                    m_cellSize.height() * m_rows + 24);

    QPainterPath platePath;
    platePath.addRoundedRect(plateRect, 15, 15);

    painter.save();

    // 底座阴影
    painter.setPen(Qt::NoPen);
    QLinearGradient plateShadow(plateRect.topLeft(), plateRect.bottomRight());
    plateShadow.setColorAt(0, QColor(0, 0, 0, 80));
    plateShadow.setColorAt(1, QColor(0, 0, 0, 40));
    painter.translate(2, 2);
    painter.fillPath(platePath, plateShadow);
    painter.translate(-2, -2);

    // 底座主体
    QLinearGradient plateBg(plateRect.topLeft(), plateRect.bottomRight());
    plateBg.setColorAt(0, QColor(35, 45, 60, 200));
    plateBg.setColorAt(1, QColor(25, 35, 50, 180));
    painter.fillPath(platePath, plateBg);

    // 底座边框
    painter.setPen(QPen(QColor(60, 80, 100, 180), 1.5));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(platePath);

    // 底座内边框高光
    QPainterPath innerPath;
    innerPath.addRoundedRect(plateRect.adjusted(1, 1, -1, -1), 13, 13);
    painter.setPen(QPen(QColor(80, 110, 140, 120), 1));
    painter.drawPath(innerPath);

    painter.restore();

    // 绘制所有试管
    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_columns; ++col) {
            QRect tubeRect(m_startX + col * m_cellSize.width(),
                          m_startY + row * m_cellSize.height(),
                          m_cellSize.width(),
                          m_cellSize.height());

            bool highlighted = (m_highlightPos.x() == row && m_highlightPos.y() == col);
            drawTube(&painter, row, col, tubeRect, highlighted);
        }
    }
}

void TestTubePlate::drawTube(QPainter* painter, int row, int col, const QRect& rect, bool highlighted)
{
    QPoint center = rect.center();
    int outerRadius = qMin(rect.width(), rect.height()) / 2 - 3;
    int innerRadius = outerRadius - m_ringWidth;

    if (innerRadius < 8) innerRadius = 8;
    if (outerRadius <= innerRadius) outerRadius = innerRadius + m_ringWidth;

    TubeState state = tubeState(row, col);
    drawGlassTube(painter, center, outerRadius, innerRadius, state, highlighted);

    // 显示编号的条件：非Empty状态（Pending也显示编号）
    if (state != TubeState::Empty) {
        QString id = tubeId(row, col);
        painter->save();

        // 根据内圆直径动态计算字体大小
        int fontSize = qMax(7, innerRadius / 2);
        QFont font = painter->font();
        font.setPointSize(fontSize);
        font.setBold(true);
        painter->setFont(font);

        // 根据状态选择文字颜色
        QColor textColor;
        if (state == TubeState::Pending) {
            textColor = QColor(80, 100, 120);     // 灰蓝色
        } else if (state == TubeState::PPP) {
            textColor = QColor(80, 60, 30);      // 深棕色
        } else {  // PRP
            textColor = QColor(100, 70, 40);     // 深黄褐色
        }

        painter->setPen(textColor);

        QFontMetrics fm(font);
        QRect textRect = fm.boundingRect(id);

        // 确保文字不超出内圆范围
        int maxTextWidth = innerRadius * 1.6;
        if (textRect.width() > maxTextWidth) {
            fontSize = qMax(6, fontSize * maxTextWidth / textRect.width());
            font.setPointSize(fontSize);
            painter->setFont(font);
            fm = QFontMetrics(font);
            textRect = fm.boundingRect(id);
        }

        QPoint textPos(center.x() - textRect.width() / 2,
                       center.y() + textRect.height() / 3);
        painter->drawText(textPos, id);

        painter->restore();
    }
}

void TestTubePlate::drawGlassTube(QPainter* painter, const QPoint& center,
                                   int outerRadius, int innerRadius,
                                   TubeState state, bool highlighted)
{
    painter->save();

    // 1. 绘制试管外壁阴影
    QRadialGradient shadowGradient(center.x() + 3, center.y() + 3, outerRadius);
    shadowGradient.setColorAt(0, QColor(0, 0, 0, 50));
    shadowGradient.setColorAt(1, QColor(0, 0, 0, 0));
    painter->setPen(Qt::NoPen);
    painter->setBrush(shadowGradient);
    painter->drawEllipse(center, outerRadius, outerRadius);

    // 2. 绘制试管外壁（透明水晶玻璃）
    QRadialGradient glassGradient(center, outerRadius);

    // Empty和Pending都使用透明水晶玻璃色
    if (state == TubeState::Empty || state == TubeState::Pending) {
        // 透明水晶玻璃
        glassGradient.setColorAt(0, QColor(255, 255, 255, 200));
        glassGradient.setColorAt(0.3, QColor(245, 250, 255, 180));
        glassGradient.setColorAt(0.6, QColor(235, 245, 255, 160));
        glassGradient.setColorAt(0.8, QColor(225, 240, 255, 140));
        glassGradient.setColorAt(1, QColor(215, 235, 250, 120));
    } else if (state == TubeState::PPP) {
        // 全血/血浆：淡黄色
        glassGradient.setColorAt(0, QColor(235, 215, 160, 190));
        glassGradient.setColorAt(0.3, QColor(225, 200, 145, 200));
        glassGradient.setColorAt(0.6, QColor(210, 185, 130, 210));
        glassGradient.setColorAt(1, QColor(195, 170, 115, 220));
    } else { // PRP
        // PPP/PRP血浆：浅黄色（更淡、更透明）
        glassGradient.setColorAt(0, QColor(255, 235, 170, 180));
        glassGradient.setColorAt(0.3, QColor(250, 225, 155, 190));
        glassGradient.setColorAt(0.6, QColor(245, 215, 140, 200));
        glassGradient.setColorAt(1, QColor(240, 205, 125, 210));
    }

    painter->setBrush(glassGradient);
    painter->drawEllipse(center, outerRadius, outerRadius);

    // 3. 绘制高光效果
    QRadialGradient highlightGradient(center.x() - outerRadius/2.5,
                                       center.y() - outerRadius/2.5,
                                       outerRadius/1.8);

    if (state == TubeState::Empty || state == TubeState::Pending) {
        highlightGradient.setColorAt(0, QColor(255, 255, 255, 140));
        highlightGradient.setColorAt(0.4, QColor(255, 255, 255, 70));
        highlightGradient.setColorAt(0.7, QColor(255, 255, 255, 30));
        highlightGradient.setColorAt(1, QColor(255, 255, 255, 0));
    } else if (state == TubeState::PPP) {
        highlightGradient.setColorAt(0, QColor(255, 245, 200, 90));
        highlightGradient.setColorAt(0.5, QColor(255, 240, 180, 40));
        highlightGradient.setColorAt(1, QColor(255, 235, 160, 0));
    } else { // PRP
        highlightGradient.setColorAt(0, QColor(255, 250, 210, 100));
        highlightGradient.setColorAt(0.5, QColor(255, 245, 190, 50));
        highlightGradient.setColorAt(1, QColor(255, 240, 170, 0));
    }

    painter->setBrush(highlightGradient);
    painter->drawEllipse(center, outerRadius, outerRadius);

    // 4. 绘制内圆（内容物）
    QRadialGradient innerGradient(center, innerRadius);

    if (state == TubeState::Empty || state == TubeState::Pending) {
        innerGradient.setColorAt(0, QColor(255, 255, 255, 220));
        innerGradient.setColorAt(0.5, QColor(245, 250, 255, 180));
        innerGradient.setColorAt(1, QColor(235, 245, 255, 140));
    } else if (state == TubeState::PPP) {
        innerGradient.setColorAt(0, QColor(255, 235, 180, 240));
        innerGradient.setColorAt(0.5, QColor(250, 220, 165, 250));
        innerGradient.setColorAt(1, QColor(245, 205, 150, 255));
    } else { // PRP
        innerGradient.setColorAt(0, QColor(255, 245, 200, 235));
        innerGradient.setColorAt(0.5, QColor(255, 240, 185, 245));
        innerGradient.setColorAt(1, QColor(250, 235, 170, 255));
    }

    painter->setBrush(innerGradient);
    painter->drawEllipse(center, innerRadius, innerRadius);

    // 5. 内圆高光
    if (state == TubeState::Empty || state == TubeState::Pending) {
        QRadialGradient innerHighlight(center.x() - innerRadius/3,
                                        center.y() - innerRadius/3,
                                        innerRadius/1.5);
        innerHighlight.setColorAt(0, QColor(255, 255, 255, 120));
        innerHighlight.setColorAt(0.6, QColor(255, 255, 255, 50));
        innerHighlight.setColorAt(1, QColor(255, 255, 255, 0));
        painter->setBrush(innerHighlight);
        painter->drawEllipse(center, innerRadius, innerRadius);
    } else if (state == TubeState::PPP) {
        QRadialGradient filledHighlight(center.x() - innerRadius/3,
                                         center.y() - innerRadius/3,
                                         innerRadius/2);
        filledHighlight.setColorAt(0, QColor(255, 250, 200, 130));
        filledHighlight.setColorAt(1, QColor(255, 250, 200, 0));
        painter->setBrush(filledHighlight);
        painter->drawEllipse(center, innerRadius, innerRadius);
    } else { // PRP
        QRadialGradient prpHighlight(center.x() - innerRadius/3,
                                      center.y() - innerRadius/3,
                                      innerRadius/2);
        prpHighlight.setColorAt(0, QColor(255, 252, 220, 120));
        prpHighlight.setColorAt(1, QColor(255, 252, 220, 0));
        painter->setBrush(prpHighlight);
        painter->drawEllipse(center, innerRadius, innerRadius);
    }

    // 6. 绘制玻璃管壁厚度
    QRadialGradient edgeGradient(center, outerRadius);
    edgeGradient.setColorAt(0.85, QColor(255, 255, 255, 0));
    edgeGradient.setColorAt(0.95, QColor(255, 255, 255, 50));
    edgeGradient.setColorAt(1, QColor(220, 235, 250, 70));
    painter->setBrush(edgeGradient);
    painter->drawEllipse(center, outerRadius, outerRadius);

    // 7. 高亮光圈
    if (highlighted) {
        drawHighlightRing(painter, center, outerRadius + 2);
    }

    painter->restore();
}

void TestTubePlate::drawHighlightRing(QPainter* painter, const QPoint& center, int radius)
{
    painter->save();

    // 绘制外发光效果
    for (int i = 3; i >= 1; --i) {
        QRadialGradient glowGradient(center, radius + i * 2);
        int alpha = 80 / i;
        glowGradient.setColorAt(0, QColor(100, 200, 255, alpha));
        glowGradient.setColorAt(1, QColor(100, 200, 255, 0));

        painter->setPen(Qt::NoPen);
        painter->setBrush(glowGradient);
        painter->drawEllipse(center, radius + i * 2, radius + i * 2);
    }

    // 绘制高亮边框
    QPen highlightPen(QColor(80, 180, 255, 220), 2);
    painter->setPen(highlightPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(center, radius, radius);

    painter->restore();
}

void TestTubePlate::mousePressEvent(QMouseEvent *event)
{
    // 如果不可点击，则不处理
    if (!m_clickable) {
        QWidget::mousePressEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        QPoint tubePos = getTubeAt(event->pos());
        if (tubePos.x() >= 0 && tubePos.x() < m_rows &&
            tubePos.y() >= 0 && tubePos.y() < m_columns) {

            if (m_highlightPos != tubePos) {
                m_highlightPos = tubePos;
                update();
            }

            QString id = tubeId(tubePos.x(), tubePos.y());
            TubeState state = tubeState(tubePos.x(), tubePos.y());
            emit tubeClicked(tubePos.x(), tubePos.y(), id, state);
        } else {
            clearHighlight();
        }
    }

    QWidget::mousePressEvent(event);
}

void TestTubePlate::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    update();
}

QPoint TestTubePlate::getTubeAt(const QPoint& pos) const
{
    if (pos.x() >= m_startX && pos.x() < m_startX + m_cellSize.width() * m_columns &&
        pos.y() >= m_startY && pos.y() < m_startY + m_cellSize.height() * m_rows) {

        int col = (pos.x() - m_startX) / m_cellSize.width();
        int row = (pos.y() - m_startY) / m_cellSize.height();

        return QPoint(row, col);
    }

    return QPoint(-1, -1);
}
