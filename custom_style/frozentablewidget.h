#ifndef FROZENTABLEWIDGET_H
#define FROZENTABLEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QTableView>
#include <QScrollBar>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QComboBox>
#include <QPaintEvent>
#include <QStandardItemModel>


// 自定义代理：只读单元格
class ReadOnlyDelegate : public QStyledItemDelegate {
public:
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&,
                         const QModelIndex&) const override {
        return nullptr; // 返回nullptr使单元格只读
    }
};

// 自定义代理：组合框单元格
class ComboBoxDelegate : public QStyledItemDelegate {
public:
    explicit ComboBoxDelegate(const QStringList& items, QObject* parent = nullptr)
        : QStyledItemDelegate(parent), m_items(items) {}

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&,
                         const QModelIndex&) const override {
        QComboBox* editor = new QComboBox(parent);
        editor->setStyleSheet("QComboBox{border-radius:5px; padding:2px 4px; font: 19 17pt '楷体'; border: 2px solid rgba(220,220,220);background-color:rgba(188,187,183);}"
                              "QComboBox::drop-down {subcontrol-origin: padding;subcontrol-position: top right;width: 15px; border:none;}"
                              "QComboBox::down-arrow {height:30px; width:30px; image: url(:/Picture/minus.png);}"
                              "QComboBox QAbstractItemView{background:rgba(255,255,255,1);border:1px solid rgba(150,150,150,1);border-radius:0px 0px 5px 5px;font: 24px; font-family: 楷体;outline: 0px;}"
                              "QComboBox QAbstractItemView::item{ height:36px; color:#666666; padding-left:9px; background-color:#FFFFFF;}"
                              "QComboBox QAbstractItemView::item:hover{background-color:#409CE1;color:#ffffff;}"
                              "QComboBox QAbstractItemView::item:selected{background-color:#409CE1;color:#ffffff;}"
                              "QComboBox:on { padding-top: 3px;padding-left: 3px;}"
                              "QComboBox::down-arrow:on {top: 1px;left:3px;}");
        editor->addItems(m_items);
        return editor;
    }

    void setEditorData(QWidget* editor, const QModelIndex& index) const override {
        QComboBox* cb = static_cast<QComboBox*>(editor);
        cb->setCurrentText(index.data().toString());
    }

    void setModelData(QWidget* editor, QAbstractItemModel* model,
                     const QModelIndex& index) const override {
        QComboBox* cb = static_cast<QComboBox*>(editor);
        model->setData(index, cb->currentText());
    }

private:
    QStringList m_items;
};

class FrozenTableWidget : public QTableView  {
    Q_OBJECT
public:
    explicit FrozenTableWidget(QStandardItemModel *model, QWidget *parent = nullptr);

    void setFrozenColumnCount(int count);  // 设置冻结列数
    void applyCustomStyle();               // 应用自定义样式
    void setRowNumbersVisible(bool visible);// 控制行号显示
    void enableColumnWrap(int column, bool enable = true);

    // 冻结列代理设置
    void setFrozenColumnDelegate(int column, QAbstractItemDelegate* delegate);

    // 冻结列样式表
    void setFrozenStyleSheet(const QString& styleSheet);

protected:
    void resizeEvent(QResizeEvent *event) override;
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
    void paintEvent(QPaintEvent* event) override;
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) override;

private slots:
    void updateSectionWidth(int logicalIndex, int oldSize, int newSize); // 同步列宽
    void updateSectionHeight(int logicalIndex, int oldSize, int newSize);
    void handleDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);


private:
    void initFrozenView();        // 初始化冻结视图
    void updateFrozenTableGeometry(); // 更新冻结视图位置

    // 新增的辅助函数
    void updateRowHeights();
    void createShadowBorder();

    QTableView* frozenTableView;  // 冻结列的视图
    int frozenColumnCount = 1;    // 默认冻结1列
    bool showRowNumbers = false;  // 是否显示行号
};

#endif // FROZENTABLEWIDGET_H
