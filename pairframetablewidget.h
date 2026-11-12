#ifndef PAIRFRAMETABLEWIDGET_H
#define PAIRFRAMETABLEWIDGET_H

#include <QHeaderView>
#include <QPainter>
#include <QSet>
#include <QTableView>
#include <QTableWidget>
#include <QVector>
#include <algorithm>
#include <functional>

// 结果预览表：QTableWidget 子类。
// 1) paintEvent 画配对框/选中框，避免 overlay 几何循环。
// 2) 可选冻结左侧若干列，横向滚动时钉住不动。
// 无 Q_OBJECT，避免额外 MOC 依赖。
class PairFrameTableWidget : public QTableWidget
{
public:
    explicit PairFrameTableWidget(QWidget *parent = nullptr);
    ~PairFrameTableWidget();

    void setPairRows(const QSet<int> &rows);
    void clearPairRows();
    void syncLayout();

    // 冻结左侧列数。结果预览固定前 4 列：样本号、添加时间、条形码、姓名。
    void setFrozenColumnCount(int count);
    int frozenColumnCount() const { return m_freezeCount; }
    void refreshFrozenDelegates();

    QTableView *frozenView() const;

    // 固定列右键：回调行号（叠层 indexAt），避免主表横向滚动后 itemAt 点空。
    void setFrozenContextMenuHandler(const std::function<void(int row)> &handler);

    // 全局坐标落在固定列叠层时返回行号，否则 -1。
    int frozenRowAtGlobal(const QPoint &globalPos) const;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    class FrozenSideView;

    void initFrozenView();
    void updateFrozenGeometry();
    void syncFrozenColumnWidths();
    void syncFrozenRowHeights();
    void syncFrozenVisibility();
    void syncFrozenSpans();
    void applyFrozenStyle();
    void handleFrozenContextMenu(const QPoint &viewportPos);

    QSet<int> m_pairRows;
    FrozenSideView *m_frozen = nullptr;
    int m_freezeCount = 0;
    std::function<void(int row)> m_frozenContextMenuHandler;
};

#endif // PAIRFRAMETABLEWIDGET_H
