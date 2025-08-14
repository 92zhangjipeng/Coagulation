#include "FrozenTableWidget.h"
#include <QsLog/include/QsLog.h>
#include <QGraphicsDropShadowEffect>
#include <QItemSelectionModel>
#include <QPainter>
#include <QTextEdit>

FrozenTableWidget::FrozenTableWidget(QStandardItemModel *model, QWidget *parent)
    : QTableView(parent), frozenColumnCount(0), showRowNumbers(true) {
    setModel(model);
    frozenTableView = new QTableView(this);
    initFrozenView();
    applyCustomStyle(); // 应用自定义样式 
}

// 初始化冻结视图
void FrozenTableWidget::initFrozenView() {
    // 共享模型
    frozenTableView->setModel(model());
    frozenTableView->setFocusPolicy(Qt::NoFocus);

    // 隐藏行号（垂直表头）
    frozenTableView->verticalHeader()->setVisible(false);
    verticalHeader()->setVisible(false);

    // 启用自动换行
    frozenTableView->setWordWrap(true);
    frozenTableView->setTextElideMode(Qt::ElideNone);
    frozenTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    // 隐藏冻结视图的水平滚动条
    frozenTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 冻结视图置于主视图上层
    viewport()->stackUnder(frozenTableView);

    // 同步垂直滚动条
    connect(verticalScrollBar(), &QScrollBar::valueChanged,
           frozenTableView->verticalScrollBar(), &QScrollBar::setValue);
    connect(frozenTableView->verticalScrollBar(), &QScrollBar::valueChanged,
           verticalScrollBar(), &QScrollBar::setValue);

    // 同步列宽变化
    connect(horizontalHeader(), &QHeaderView::sectionResized,
               this, &FrozenTableWidget::updateSectionWidth);
    connect(verticalHeader(), &QHeaderView::sectionResized,
                this, &FrozenTableWidget::updateSectionHeight);

    // 数据变化时更新行高
    connect(model(), &QAbstractItemModel::dataChanged,
            this, &FrozenTableWidget::handleDataChanged);
}

// 设置冻结列数
void FrozenTableWidget::setFrozenColumnCount(int count) {
    frozenColumnCount = qMax(0, count);
    // 隐藏非冻结列
    for (int col = 0; col < model()->columnCount(); ++col) {
        frozenTableView->setColumnHidden(col, col >= frozenColumnCount);
    }
    updateFrozenTableGeometry();
}

// 设置行号可见性
void FrozenTableWidget::setRowNumbersVisible(bool visible) {
    showRowNumbers = visible;
    update();
}

// 启用列自动换行
void FrozenTableWidget::enableColumnWrap(int column, bool enable) {
    if (enable) {
        // 设置列宽自适应内容
        frozenTableView->horizontalHeader()->setSectionResizeMode(column, QHeaderView::ResizeToContents);

        // 为每行创建支持换行的文本控件
        for (int row = 0; row < model()->rowCount(); ++row) {
            QModelIndex index = model()->index(row, column);
            QString text = model()->data(index).toString();

            QTextEdit *textEdit = new QTextEdit(frozenTableView);
            textEdit->setText(text);
            textEdit->setFrameShape(QFrame::NoFrame);
            textEdit->setReadOnly(true);
            textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

            textEdit->setStyleSheet("QTextEdit {"
                                   "  background: transparent;"
                                   "  border: none;"
                                   "  padding: 4px;"
                                   "}");

            frozenTableView->setIndexWidget(index, textEdit);
        }
    }
    updateRowHeights();  // 更新行高
}


// 更新行高
void FrozenTableWidget::updateRowHeights() {
    for (int row = 0; row < model()->rowCount(); ++row) {
        int maxHeight = 40;  // 最小行高

        // 计算冻结列中的最大高度
        for (int col = 0; col < frozenColumnCount; ++col) {
            if (auto widget = frozenTableView->indexWidget(model()->index(row, col))) {
                maxHeight = qMax(maxHeight, widget->sizeHint().height());
            }
        }

        // 同步主表和冻结表的行高
        setRowHeight(row, maxHeight);
        frozenTableView->setRowHeight(row, maxHeight);
    }
}

// 创建阴影边框效果
void FrozenTableWidget::createShadowBorder() {
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(frozenTableView);
    shadow->setBlurRadius(12);
    shadow->setOffset(3, 0);
    shadow->setColor(QColor(0, 0, 0, 60));
    frozenTableView->setGraphicsEffect(shadow);
}


void FrozenTableWidget::applyCustomStyle() {
    // 主表格样式（添加滚动条样式）
    setStyleSheet(R"(
        /* 主表格样式 */
        QTableView {
            gridline-color: #e0e0e0;
            background-color: white;
            alternate-background-color: #f5f5f5;
        }
        QTableView::item:selected {
            background-color: #d8e6f3;
        }
        QHeaderView::section {
            background-color:rgb(188, 187, 186);
            color: white;
            height: 25px;
            padding: 4px;
            border: 1px solid rgb(0, 0, 0);
            border-radius:0px;
            border-color: rgb(100, 100, 100);
            font-weight: bold;
        }

        /* 滚动条样式 [[7]][[11]] */
        QScrollBar:vertical {
            background: #F0F0F0;
            width: 12px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background: #C0C0C0;
            min-height: 20px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical:hover {
            background: #A8A8A8;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
            background: none;
        }
        QScrollBar:horizontal {
            background: #F0F0F0;
            height: 12px;
            margin: 0;
        }
        QScrollBar::handle:horizontal {
            background: #C0C0C0;
            min-width: 20px;
            border-radius: 6px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #A8A8A8;
        }
    )");

    // 冻结表格样式（增强边框效果）
    frozenTableView->setStyleSheet(R"(
        QTableView {
            gridline-color: #e0e0e0;
            background-color: #f8f8f8;
            alternate-background-color: #f0f0f0;
            border-right: 1px solid #3498db;  /* 增强右侧边框 */
        }
        QTableView::item:selected {
            background-color: #d8e6f3;
        }
        QHeaderView::section {
            background-color:rgb(188, 187, 186);
            color: white;
            height: 25px;
            padding: 4px;
            border: 1px solid rgb(0, 0, 0);
            border-radius:1px;
            border-color: rgb(100, 100, 100);
            font-weight: bold;
        }
    )");
}


// 更新冻结视图位置
void FrozenTableWidget::updateFrozenTableGeometry() {
    if (frozenColumnCount <= 0) {
        frozenTableView->hide();
        return;
    }
    frozenTableView->show();
    int totalWidth = 0;
    // 计算冻结列总宽度
    for (int i = 0; i < frozenColumnCount; ++i) {
        totalWidth += columnWidth(i);
    }
    // 设置冻结视图几何位置：紧贴左侧无留白
    frozenTableView->setGeometry(
        0,                          // X坐标设为0（紧贴左侧边缘）
        frameWidth(),
        totalWidth,
        viewport()->height() + frameWidth()
    );
    // 移动冻结视图的视口位置
    frozenTableView->verticalScrollBar()->setSliderPosition(
        verticalScrollBar()->value()
    );

    // 创建右侧边框效果
    QFrame *borderFrame = new QFrame(this);
    borderFrame->setGeometry(frozenTableView->geometry().adjusted(-2, -2, 2, 2));
    borderFrame->setStyleSheet(
        "QFrame {"
        "  border: 2px solid #3498db;"
        "  border-top: none;"
        "  border-left: none;"
        "  border-bottom: none;"
        "}"
    );
    borderFrame->raise();
    frozenTableView->raise();
}
// 重写窗口大小调整事件
void FrozenTableWidget::resizeEvent(QResizeEvent* event) {
    QTableView::resizeEvent(event);
    updateFrozenTableGeometry();
}

// 处理跨视图的键盘导航
QModelIndex FrozenTableWidget::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) {
    QModelIndex current = QTableView::moveCursor(cursorAction, modifiers);
    if (cursorAction == MoveLeft && current.column() < frozenColumnCount &&
        visualRect(current).topLeft().x() < frozenTableView->width()) {
        // 当焦点在冻结列且向左越界时，强制焦点停留在第一列
        return model()->index(current.row(), 0, rootIndex());
    }
    return current;
}

// 绘制行号
void FrozenTableWidget::paintEvent(QPaintEvent* event) {
    QTableView::paintEvent(event);

    if (showRowNumbers) {
        QPainter painter(viewport());
        QStyleOptionViewItem option;
        option.initFrom(this);
        option.state |= QStyle::State_Active;
        option.font = font();
        option.font.setBold(true);
        option.palette.setColor(QPalette::Text, Qt::darkGray);

        for (int row = 0; row < model()->rowCount(); ++row) {
            QRect rect = visualRect(model()->index(row, 0));
            QString number = QString::number(row + 1);

            // 绘制行号背景
            painter.fillRect(QRect(0, rect.y(), frozenTableView->width(), rect.height()),
                            QColor(240, 240, 240));

            // 绘制行号文本
            painter.drawText(QRect(5, rect.y(), frozenTableView->width() - 10, rect.height()),
                           Qt::AlignLeft | Qt::AlignVCenter, number);
        }
    }
}

void FrozenTableWidget::updateSectionWidth(int logicalIndex, int oldSize, int newSize)
{
    if (logicalIndex < frozenColumnCount) {
        frozenTableView->setColumnWidth(logicalIndex, newSize);
        updateFrozenTableGeometry();
    }
}

// 处理行高变化
void FrozenTableWidget::updateSectionHeight(int logicalIndex, int oldSize, int newSize) {
    frozenTableView->setRowHeight(logicalIndex, newSize);
}

// 处理数据变化
void FrozenTableWidget::handleDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight) {
    // 检查变化是否发生在冻结列中
    if (topLeft.column() < frozenColumnCount || bottomRight.column() < frozenColumnCount) {
        for (int col = topLeft.column(); col <= bottomRight.column(); ++col) {
            if (col < frozenColumnCount) {
                enableColumnWrap(col, true);
            }
        }
    }
}

// 重写滚动事件
void FrozenTableWidget::scrollTo(const QModelIndex &index, ScrollHint hint) {
    QTableView::scrollTo(index, hint);
    frozenTableView->scrollTo(index, hint);
}


// 冻结列代理设置
void FrozenTableWidget::setFrozenColumnDelegate(int column, QAbstractItemDelegate* delegate) {
    Q_ASSERT(frozenTableView);
    frozenTableView->setItemDelegateForColumn(column, delegate);
}

// 冻结列样式表
void FrozenTableWidget::setFrozenStyleSheet(const QString& styleSheet) {
    frozenTableView->setStyleSheet(styleSheet);
}

