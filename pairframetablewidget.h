#ifndef PAIRFRAMETABLEWIDGET_H
#define PAIRFRAMETABLEWIDGET_H

#include <QTableWidget>
#include <QPainter>
#include <QSet>

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
        if (m_pairRows.isEmpty()) return;

        QPainter painter(viewport());
        painter.setRenderHint(QPainter::Antialiasing);
        QPen pen(QColor(0, 120, 215), 2, Qt::DashLine);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);

        const int margin = 2;
        const QRect visibleRect = viewport()->rect();
        for (int row : m_pairRows) {
            QTableWidgetItem *item = this->item(row, 0);  // 第0列=样本号
            if (!item) continue;
            QRect rect = visualItemRect(item);
            if (rect.isNull() || !rect.isValid() || !visibleRect.intersects(rect)) continue;
            rect.setLeft(0);
            rect.setRight(visibleRect.width());
            painter.drawRect(rect.adjusted(margin, margin, -margin, -margin));
        }
    }

private:
    QSet<int> m_pairRows;
};

#endif // PAIRFRAMETABLEWIDGET_H
