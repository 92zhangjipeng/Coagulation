#ifndef PAIRFRAMETABLEWIDGET_H
#define PAIRFRAMETABLEWIDGET_H

#include <QTableWidget>
#include <QPainter>
#include <QSet>
#include <QVector>
#include <algorithm>

// QTableWidget 子类:在 paintEvent 中绘制配对样本框。
// 替代原 overlay 方案(overlay 作为 viewport 子widget 做几何同步,会与 viewport 形成
// Move/Resize 几何循环,导致切到结果预览后主线程卡死、内存持续上涨)。
// paintEvent 是合法绘制上下文(QPainter 在 paintEvent 内使用),不触发重入重绘;
// 不涉及子 widget 几何同步,无 Move/Resize 循环。
//
// 注意:本类无信号/槽/属性,故不使用 Q_OBJECT 宏,避免对 MOC 的依赖
// (否则需 MOC 生成 moc_pairframetablewidget.cpp,未重新运行 qmake 会导致链接错误:
//  无法解析 metaObject/qt_metacast/qt_metacall/staticMetaObject)。
// ui->saveTaskWidgetList 已被 calibrate.ui 提升为本类指针,可直接调用 setPairRows。
class PairFrameTableWidget : public QTableWidget
{
public:
    explicit PairFrameTableWidget(QWidget *parent = nullptr) : QTableWidget(parent) {}

    // 设置需要画框的行号集合(外部在数据变化时调用)
    void setPairRows(const QSet<int> &rows) { m_pairRows = rows; viewport()->update(); }
    void clearPairRows() { m_pairRows.clear(); viewport()->update(); }

    void syncLayout()
    {
        doItemsLayout();
        updateGeometries();
        if (QWidget *vp = viewport()) {
            vp->update();
        }
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QTableWidget::paintEvent(event);  // 先画表格内容

        QPainter painter(viewport());
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(Qt::NoBrush);
        const QRect visibleRect = viewport()->rect();

        if (!m_pairRows.isEmpty()) {
            painter.setPen(QPen(QColor(0, 120, 215), 2, Qt::DashLine));
            const int margin = 2;
            for (int row : m_pairRows) {
                QTableWidgetItem *item = this->item(row, 0);
                if (!item)
                    continue;
                QRect rect = visualItemRect(item);
                if (rect.isNull() || !rect.isValid() || !visibleRect.intersects(rect))
                    continue;
                rect.setLeft(0);
                rect.setRight(visibleRect.width());
                painter.drawRect(rect.adjusted(margin, margin, -margin, -margin));
            }
        }

        if (!selectionModel())
            return;
        QVector<int> selectedRows;
        const QModelIndexList selected = selectionModel()->selectedRows();
        selectedRows.reserve(selected.size());
        for (const QModelIndex &index : selected) {
            if (index.isValid())
                selectedRows.append(index.row());
        }
        if (selectedRows.isEmpty())
            return;
        std::sort(selectedRows.begin(), selectedRows.end());
        painter.setPen(QPen(QColor(20, 135, 184), 2));
        int blockStart = selectedRows.first();
        int blockEnd = blockStart;
        auto drawBlock = [&](int firstRow, int lastRow) {
            const int y = rowViewportPosition(firstRow);
            int blockH = 0;
            for (int row = firstRow; row <= lastRow; ++row)
                blockH += rowHeight(row);
            if (blockH <= 0 || y + blockH <= 0 || y >= visibleRect.height())
                return;
            const QRect frame(1, y + 1, qMax(0, visibleRect.width() - 3), qMax(0, blockH - 2));
            painter.drawRoundedRect(frame, 4, 4);
        };
        for (int i = 1; i < selectedRows.size(); ++i) {
            if (selectedRows.at(i) == blockEnd + 1) {
                blockEnd = selectedRows.at(i);
                continue;
            }
            drawBlock(blockStart, blockEnd);
            blockStart = blockEnd = selectedRows.at(i);
        }
        drawBlock(blockStart, blockEnd);
    }

private:
    QSet<int> m_pairRows;
};

#endif // PAIRFRAMETABLEWIDGET_H
