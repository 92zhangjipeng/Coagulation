#ifndef HOLEGRIDWIDGET_H
#define HOLEGRIDWIDGET_H

#include <QWidget>
#include <QVector>
#include <QString>

struct HoleCell {
    int row;
    int col;
    int number;
    QString text;
};

class HoleGridWidget : public QWidget
{
    Q_OBJECT
public:
    enum FitMode {
        CenterBlock = 0,
        FillEven
    };

    explicit HoleGridWidget(QWidget *parent = nullptr);

    void setHoles(const QVector<HoleCell> &holes, int columns, FitMode fit, int preferredHolePx);
    void setSelectedNumber(int number);
    int lastClickedNumber() const { return m_lastClicked; }
    int selectedNumber() const { return m_selected; }

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int w) const override;
    void setSquareAspect(bool on);

signals:
    void holeClicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QRect holeRect(const HoleCell &cell) const;
    int holeAt(const QPoint &pos) const;
    int columnCount() const { return m_maxCol + 1; }
    int rowCount() const { return m_maxRow + 1; }

    QVector<HoleCell> m_holes;
    int m_maxRow;
    int m_maxCol;
    int m_selected;
    int m_hover;
    int m_lastClicked;
    FitMode m_fit;
    int m_preferredHole;
    bool m_squareAspect;
};

#endif
