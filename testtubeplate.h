// testtubeplate.h
#ifndef TESTTUBEPLATE_H
#define TESTTUBEPLATE_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QMap>
#include <QLinearGradient>

// 试管状态枚举
enum class TubeState {
    Empty,      // 空试管（透明水晶玻璃）
    Pending,    // 待使用（透明水晶玻璃，显示编号）
    PPP,        // 全血/血浆（淡黄色）
    PRP         // PPP/PRP血浆（浅黄色）
};

class TestTubePlate : public QWidget
{
    Q_OBJECT

public:
    explicit TestTubePlate(QWidget *parent = nullptr);

    // 设置行列数
    void setRows(int rows);
    int rows() const { return m_rows; }

    void setColumns(int columns);
    int columns() const { return m_columns; }

    // 设置圆环宽度
    void setRingWidth(int width);
    int ringWidth() const { return m_ringWidth; }

    // 设置边框边距
    void setBorderMargin(int margin);
    int borderMargin() const { return m_borderMargin; }

    // 设置试管编号
    void setTubeId(int row, int col, const QString& id);
    QString tubeId(int row, int col) const;

    // 设置试管状态
    void setTubeState(int row, int col, TubeState state);
    TubeState tubeState(int row, int col) const;

    // 批量设置状态
    void setAllTubesState(TubeState state);

    // 清除所有高亮
    void clearHighlight();

    // 获取当前高亮的位置
    QPoint currentHighlight() const { return m_highlightPos; }

    // 设置是否可点击
    void setClickable(bool clickable);
    bool isClickable() const { return m_clickable; }

    // 获取widget的最小大小
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

signals:
    void tubeClicked(int row, int col, const QString& id, TubeState state);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void calculateLayout();
    void drawTube(QPainter* painter, int row, int col, const QRect& rect, bool highlighted);
    void drawGlassTube(QPainter* painter, const QPoint& center, int outerRadius, int innerRadius,
                       TubeState state, bool highlighted);
    void drawHighlightRing(QPainter* painter, const QPoint& center, int radius);
    QPoint getTubeAt(const QPoint& pos) const;
    int calculateTubeSize() const;

private:
    int m_rows = 10;
    int m_columns = 6;
    int m_ringWidth = 10;
    int m_borderMargin = 2;
    bool m_clickable = true;  // 是否可点击

    QMap<QPair<int, int>, QString> m_tubeIds;
    QMap<QPair<int, int>, TubeState> m_tubeStates;
    QPoint m_highlightPos = QPoint(-1, -1);

    QRect m_gridRect;
    QSize m_cellSize;
    int m_startX, m_startY;
};

#endif // TESTTUBEPLATE_H
