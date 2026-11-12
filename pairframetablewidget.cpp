#include "pairframetablewidget.h"

#include <QContextMenuEvent>
#include <QCoreApplication>
#include <QItemSelection>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QScrollBar>

class PairFrameTableWidget::FrozenSideView : public QTableView
{
public:
    explicit FrozenSideView(PairFrameTableWidget *owner)
        : QTableView(owner)
        , m_owner(owner)
    {
    }

    void attachContextMenuHook()
    {
        setContextMenuPolicy(Qt::DefaultContextMenu);
        if (viewport()) {
            viewport()->setContextMenuPolicy(Qt::DefaultContextMenu);
            viewport()->installEventFilter(this);
        }
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (m_owner && watched == viewport() && event) {
            if (event->type() == QEvent::ContextMenu) {
                auto *ce = static_cast<QContextMenuEvent *>(event);
                m_owner->handleFrozenContextMenu(viewport()->mapFromGlobal(ce->globalPos()));
                return true;
            }
            if (event->type() == QEvent::MouseButtonPress) {
                auto *me = static_cast<QMouseEvent *>(event);
                if (me->button() == Qt::RightButton) {
                    // 先走视图选中，再出菜单（不等 CustomContextMenu 信号）。
                    QTableView::mousePressEvent(me);
                    m_owner->handleFrozenContextMenu(me->pos());
                    return true;
                }
            }
        }
        return QTableView::eventFilter(watched, event);
    }

    void contextMenuEvent(QContextMenuEvent *event) override
    {
        if (m_owner && event) {
            m_owner->handleFrozenContextMenu(viewport()
                ? viewport()->mapFromGlobal(event->globalPos())
                : event->pos());
            event->accept();
            return;
        }
        QTableView::contextMenuEvent(event);
    }

    void paintEvent(QPaintEvent *event) override
    {
        QTableView::paintEvent(event);
        if (!m_owner || !viewport())
            return;

        QPainter painter(viewport());
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(Qt::NoBrush);
        const QRect visibleRect = viewport()->rect();

        auto drawRowEdge = [&](int row, const QPen &pen) {
            if (row < 0 || row >= m_owner->rowCount())
                return;
            const int y = rowViewportPosition(row);
            const int h = rowHeight(row);
            if (h <= 0 || y + h <= 0 || y >= visibleRect.height())
                return;
            painter.setPen(pen);
            const QRect frame(1, y + 1, qMax(0, visibleRect.width() - 3), qMax(0, h - 2));
            painter.drawRoundedRect(frame, 4, 4);
        };

        if (!m_owner->m_pairRows.isEmpty()) {
            const QPen pairPen(QColor(0, 120, 215), 2, Qt::DashLine);
            for (int row : m_owner->m_pairRows)
                drawRowEdge(row, pairPen);
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
        const QPen selectPen(QColor(20, 135, 184), 2);
        int blockStart = selectedRows.first();
        int blockEnd = blockStart;
        auto drawBlock = [&](int firstRow, int lastRow) {
            const int y = rowViewportPosition(firstRow);
            int blockH = 0;
            for (int row = firstRow; row <= lastRow; ++row)
                blockH += rowHeight(row);
            if (blockH <= 0 || y + blockH <= 0 || y >= visibleRect.height())
                return;
            painter.setPen(selectPen);
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
    PairFrameTableWidget *m_owner = nullptr;
};

PairFrameTableWidget::PairFrameTableWidget(QWidget *parent)
    : QTableWidget(parent)
{
}

PairFrameTableWidget::~PairFrameTableWidget()
{
    m_frozen = nullptr;
}

void PairFrameTableWidget::setPairRows(const QSet<int> &rows)
{
    m_pairRows = rows;
    if (viewport())
        viewport()->update();
    if (m_frozen && m_frozen->viewport())
        m_frozen->viewport()->update();
}

void PairFrameTableWidget::clearPairRows()
{
    m_pairRows.clear();
    if (viewport())
        viewport()->update();
    if (m_frozen && m_frozen->viewport())
        m_frozen->viewport()->update();
}

void PairFrameTableWidget::syncLayout()
{
    doItemsLayout();
    updateGeometries();
    syncFrozenSpans();
    syncFrozenRowHeights();
    updateFrozenGeometry();
    if (viewport())
        viewport()->update();
    if (m_frozen && m_frozen->viewport())
        m_frozen->viewport()->update();
}

void PairFrameTableWidget::setFrozenColumnCount(int count)
{
    if (count < 0)
        count = 0;
    if (count > columnCount())
        count = columnCount();
    m_freezeCount = count;
    if (m_freezeCount <= 0) {
        if (m_frozen)
            m_frozen->hide();
        return;
    }
    initFrozenView();
    applyFrozenStyle();
    if (m_frozen)
        m_frozen->setEditTriggers(editTriggers());
    syncFrozenVisibility();
    syncFrozenColumnWidths();
    syncFrozenRowHeights();
    syncFrozenSpans();
    refreshFrozenDelegates();
    updateFrozenGeometry();
    m_frozen->show();
    m_frozen->raise();
}

void PairFrameTableWidget::syncFrozenSpans()
{
    if (!m_frozen || m_freezeCount <= 0)
        return;
    // setSpan 存在各自 View 上，叠层不会自动继承主表合并。
    m_frozen->clearSpans();
    const int rows = rowCount();
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < m_freezeCount; ++col) {
            const int rs = rowSpan(row, col);
            const int cs = columnSpan(row, col);
            if (rs > 1 || cs > 1)
                m_frozen->setSpan(row, col, rs, cs);
        }
    }
}

void PairFrameTableWidget::applyFrozenStyle()
{
    if (!m_frozen)
        return;

    m_frozen->setPalette(palette());
    m_frozen->setFont(font());
    m_frozen->setShowGrid(showGrid());
    m_frozen->setAlternatingRowColors(alternatingRowColors());
    m_frozen->setWordWrap(wordWrap());

    // 页面样式常写在父控件上且只匹配 QTableWidget；叠层是 QTableView，需改写后套上。
    QString sheet = styleSheet();
    if (sheet.isEmpty() || !sheet.contains(QStringLiteral("QTableWidget"))) {
        for (QWidget *w = parentWidget(); w; w = w->parentWidget()) {
            const QString parentSheet = w->styleSheet();
            if (parentSheet.contains(QStringLiteral("QTableWidget"))) {
                sheet = parentSheet;
                break;
            }
        }
    }
    sheet.replace(QStringLiteral("QTableWidget"), QStringLiteral("QTableView"));
    // 叠层只要右边分隔线，覆盖父页给整表加的边框。
    sheet += QStringLiteral(
        "QTableView{"
        "  border: none;"
        "  border-right: 1px solid #C5D7E4;"
        "  border-radius: 0px;"
        "}");
    m_frozen->setStyleSheet(sheet);

    QString headerSheet = horizontalHeader()->styleSheet();
    if (headerSheet.isEmpty()) {
        // 表头样式多半也在父页 QHeaderView::section 里，上面整表 sheet 已带；再空则跟主表头走。
        headerSheet = sheet;
    }
    m_frozen->horizontalHeader()->setStyleSheet(headerSheet);
    m_frozen->horizontalHeader()->setFont(horizontalHeader()->font());
    m_frozen->horizontalHeader()->setFixedHeight(horizontalHeader()->height());
}

void PairFrameTableWidget::refreshFrozenDelegates()
{
    if (!m_frozen || m_freezeCount <= 0)
        return;
    for (int col = 0; col < m_freezeCount; ++col)
        m_frozen->setItemDelegateForColumn(col, itemDelegateForColumn(col));
}

QTableView *PairFrameTableWidget::frozenView() const
{
    return m_frozen;
}

void PairFrameTableWidget::setFrozenContextMenuHandler(const std::function<void(int row)> &handler)
{
    m_frozenContextMenuHandler = handler;
}

int PairFrameTableWidget::frozenRowAtGlobal(const QPoint &globalPos) const
{
    if (!m_frozen || !m_frozen->isVisible() || !m_frozen->viewport())
        return -1;
    const QPoint topLeft = m_frozen->mapToGlobal(QPoint(0, 0));
    if (!QRect(topLeft, m_frozen->size()).contains(globalPos))
        return -1;
    const QModelIndex idx = m_frozen->indexAt(m_frozen->viewport()->mapFromGlobal(globalPos));
    return idx.isValid() ? idx.row() : -1;
}

void PairFrameTableWidget::handleFrozenContextMenu(const QPoint &viewportPos)
{
    if (!m_frozen)
        return;
    const QModelIndex idx = m_frozen->indexAt(viewportPos);
    if (!idx.isValid())
        return;

    if (m_frozenContextMenuHandler) {
        m_frozenContextMenuHandler(idx.row());
        return;
    }

    // 无外部回调时：合成主表右键，坐标落在该行可视区域（不依赖横滚后的列位置）。
    const int row = idx.row();
    const int y = rowViewportPosition(row) + qMax(1, rowHeight(row) / 2);
    const QPoint mainViewportPos(qMax(0, columnViewportPosition(0)) + 2, y);
    const QPoint widgetPos = viewport() ? viewport()->mapTo(this, mainViewportPos) : mainViewportPos;
    const QPoint globalPos = viewport() ? viewport()->mapToGlobal(mainViewportPos)
                                        : mapToGlobal(widgetPos);
    QContextMenuEvent ev(QContextMenuEvent::Mouse, widgetPos, globalPos);
    QCoreApplication::sendEvent(this, &ev);
}

void PairFrameTableWidget::initFrozenView()
{
    if (m_frozen)
        return;

    m_frozen = new FrozenSideView(this);
    m_frozen->setModel(model());
    m_frozen->setFocusPolicy(Qt::ClickFocus);
    m_frozen->setSelectionModel(selectionModel());
    m_frozen->setSelectionMode(selectionMode());
    m_frozen->setSelectionBehavior(selectionBehavior());
    // 与主表一致；结果查询主表为 NoEditTriggers，固定列不可编辑。
    m_frozen->setEditTriggers(editTriggers());
    m_frozen->verticalHeader()->hide();
    m_frozen->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_frozen->horizontalHeader()->setFixedHeight(horizontalHeader()->height());
    m_frozen->horizontalHeader()->setHighlightSections(false);
    m_frozen->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_frozen->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_frozen->setHorizontalScrollMode(ScrollPerPixel);
    m_frozen->setVerticalScrollMode(ScrollPerPixel);
    m_frozen->setShowGrid(showGrid());
    m_frozen->setAlternatingRowColors(alternatingRowColors());
    m_frozen->setWordWrap(wordWrap());
    m_frozen->setFrameShape(QFrame::NoFrame);
    applyFrozenStyle();

    viewport()->stackUnder(m_frozen);

    // 右键由 FrozenSideView 直接拦截（QTableView 的 CustomContextMenu 信号常丢）。
    m_frozen->attachContextMenuHook();

    connect(horizontalHeader(), &QHeaderView::sectionResized, this,
            [this](int logicalIndex, int, int newSize) {
                if (!m_frozen || logicalIndex < 0 || logicalIndex >= m_freezeCount)
                    return;
                m_frozen->setColumnWidth(logicalIndex, newSize);
                updateFrozenGeometry();
            });
    connect(verticalHeader(), &QHeaderView::sectionResized, this,
            [this](int logicalIndex, int, int newSize) {
                if (!m_frozen)
                    return;
                m_frozen->setRowHeight(logicalIndex, newSize);
            });
    connect(m_frozen->verticalScrollBar(), &QAbstractSlider::valueChanged,
            verticalScrollBar(), &QAbstractSlider::setValue);
    connect(verticalScrollBar(), &QAbstractSlider::valueChanged,
            m_frozen->verticalScrollBar(), &QAbstractSlider::setValue);
    connect(verticalScrollBar(), &QAbstractSlider::valueChanged, this, [this](int) {
        if (m_frozen && m_frozen->viewport())
            m_frozen->viewport()->update();
    });
    connect(verticalHeader(), &QHeaderView::geometriesChanged, this, [this]() {
        updateFrozenGeometry();
    });
    connect(horizontalHeader(), &QHeaderView::geometriesChanged, this, [this]() {
        updateFrozenGeometry();
    });

    if (selectionModel()) {
        connect(selectionModel(), &QItemSelectionModel::selectionChanged, this,
                [this](const QItemSelection &, const QItemSelection &) {
                    if (m_frozen && m_frozen->viewport())
                        m_frozen->viewport()->update();
                });
    }

    setHorizontalScrollMode(ScrollPerPixel);
    setVerticalScrollMode(ScrollPerPixel);
}

void PairFrameTableWidget::syncFrozenVisibility()
{
    if (!m_frozen)
        return;
    const int cols = columnCount();
    for (int col = 0; col < cols; ++col)
        m_frozen->setColumnHidden(col, col >= m_freezeCount);
}

void PairFrameTableWidget::syncFrozenColumnWidths()
{
    if (!m_frozen)
        return;
    for (int col = 0; col < m_freezeCount; ++col)
        m_frozen->setColumnWidth(col, columnWidth(col));
}

void PairFrameTableWidget::syncFrozenRowHeights()
{
    if (!m_frozen)
        return;
    const int rows = rowCount();
    for (int row = 0; row < rows; ++row)
        m_frozen->setRowHeight(row, rowHeight(row));
}

void PairFrameTableWidget::updateFrozenGeometry()
{
    if (!m_frozen || m_freezeCount <= 0) {
        if (m_frozen)
            m_frozen->hide();
        return;
    }

    int width = 0;
    for (int col = 0; col < m_freezeCount; ++col) {
        if (!isColumnHidden(col))
            width += columnWidth(col);
    }
    if (width <= 0) {
        m_frozen->hide();
        return;
    }

    const int x = frameWidth() + (verticalHeader()->isVisible() ? verticalHeader()->width() : 0);
    const int y = frameWidth();
    const int h = horizontalHeader()->height() + viewport()->height();
    m_frozen->setGeometry(x, y, width, h);
    m_frozen->horizontalHeader()->setFixedHeight(horizontalHeader()->height());
    m_frozen->show();
    m_frozen->raise();
}

void PairFrameTableWidget::resizeEvent(QResizeEvent *event)
{
    QTableWidget::resizeEvent(event);
    updateFrozenGeometry();
}

void PairFrameTableWidget::paintEvent(QPaintEvent *event)
{
    QTableWidget::paintEvent(event);

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
