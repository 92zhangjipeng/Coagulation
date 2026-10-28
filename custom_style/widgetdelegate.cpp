#include "widgetdelegate.h"
#include <QComboBox>
#include <QDateTimeEdit>
#include <QListWidget>
#include <QStandardItemModel>
#include <QStyle>
#include <QTextOption>

#pragma warning(disable:4100)

WidgetDelegate::WidgetDelegate(QItemDelegate *parent) : QItemDelegate(parent)
{
   this->m_comboBoxList<< "";
}

WidgetDelegate::WidgetDelegate(QStringList list, QItemDelegate *parent) :QItemDelegate(parent)
{
    this->m_comboBoxList = list;
    this->m_Css = "QComboBox {"
                  " font-family: 'Microsoft YaHei'; font-size: 13px; color: #16384A;"
                  " background: #FFFFFF; border: 1px solid #8FBFD6; border-radius: 4px;"
                  " padding: 0px 8px; }"
                  "QComboBox:hover { border: 1px solid #1A6F99; }"
                  "QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: center right;"
                  " width: 22px; border: none; background: transparent; }"
                  "QComboBox::down-arrow { width: 0px; height: 0px; image: none;"
                  " border-left: 4px solid transparent; border-right: 4px solid transparent;"
                  " border-top: 5px solid #1A6F99; }"
                  "QComboBox QAbstractItemView {"
                  " background: #FFFFFF; color: #16384A; border: 1px solid #8FBFD6;"
                  " outline: 0; font-family: 'Microsoft YaHei'; font-size: 13px; }"
                  "QComboBox QAbstractItemView::item { min-height: 28px; padding: 4px 10px; color: #16384A; }"
                  "QComboBox QAbstractItemView::item:hover { background: #E7F3FA; color: #16384A; }"
                  "QComboBox QAbstractItemView::item:selected { background: #D4EAF5; color: #16384A; }"
                  "QComboBox QScrollBar:vertical { width: 8px; margin: 0px; background: #EEF3F7; border: none; }"
                  "QComboBox QScrollBar::handle:vertical { background: #8AA4BE; min-height: 18px; border-radius: 3px; }"
                  "QComboBox QScrollBar::add-line:vertical, QComboBox QScrollBar::sub-line:vertical {"
                  " height: 0px; width: 0px; border: none; }"
                  "QComboBox QScrollBar::add-page:vertical, QComboBox QScrollBar::sub-page:vertical { background: transparent; }";
}

WidgetDelegate::~WidgetDelegate() {

}


QWidget *WidgetDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 创建自己需要的控件进行返回
    QComboBox *editor = new QComboBox(parent);
    editor->setStyleSheet(this->m_Css);
    editor->setMaxVisibleItems(8);
    editor->setFont(QFont(QStringLiteral("Microsoft YaHei"), 9));
    if (editor->view()) {
        editor->view()->setMinimumWidth(qMax(option.rect.width(), 120));
        editor->view()->setStyleSheet(QStringLiteral(
            "QAbstractItemView { background: #FFFFFF; color: #16384A; border: none; outline: 0;"
            " font-family: 'Microsoft YaHei'; font-size: 13px; }"
            "QAbstractItemView::item { min-height: 28px; padding: 4px 10px; }"
            "QAbstractItemView::item:selected { background: #D4EAF5; color: #16384A; }"
            "QScrollBar:vertical { width: 8px; margin: 0px; background: #EEF3F7; border: none; }"
            "QScrollBar::handle:vertical { background: #8AA4BE; min-height: 18px; border-radius: 3px; }"
            "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; width: 0px; }"));
    }
    return editor;
}

// 设置编辑器数据
void WidgetDelegate::setEditorData(QWidget *editor, const QModelIndex &/*index*/) const
{
    // 将参数editor转换为对应创建的控件，再进行数据初始设置就行
    QComboBox *cob = static_cast<QComboBox *>(editor);
    cob->addItems(m_comboBoxList);
    QStandardItemModel* model =  static_cast<QStandardItemModel*>(cob->model());
    if(model){
        for(int i = 0; i < model->rowCount(); i ++)
        {
            if(model->item(i))
            {
                model->item(i)->setTextAlignment(Qt::AlignCenter);
            }
        }
    }

}

// 更新编辑器集合属性
void WidgetDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    const int editorH = qMin(30, qMax(24, option.rect.height() - 8));
    const int y = option.rect.y() + (option.rect.height() - editorH) / 2;
    editor->setGeometry(option.rect.x() + 4, y, qMax(40, option.rect.width() - 8), editorH);
}

// 设置模型数据
void WidgetDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox *>(editor);	// 类型转换
    // 模型（单元格）显示的数据
    model->setData(index, comboBox->currentText());
}

void WidgetDelegate::drawDisplay(QPainter *painter, const QStyleOptionViewItem &option,
                                 const QRect &rect, const QString &text) const
{
    painter->save();
    painter->setFont(option.font);
    const QPalette::ColorGroup group = (option.state & QStyle::State_Enabled)
            ? QPalette::Normal : QPalette::Disabled;
    if (option.state & QStyle::State_Selected)
        painter->setPen(option.palette.color(group, QPalette::HighlightedText));
    else
        painter->setPen(option.palette.color(group, QPalette::Text));
    const QRect textRect = rect.adjusted(4, 2, -4, -2);
    const int flags = Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap;
    painter->drawText(textRect, flags, text);
    painter->restore();
}

// 获取索引处的数据返回
QString WidgetDelegate::getCurrentComboBoxData(int index) {
    return this->m_comboBoxList.at(index);
}

// 插入数据
void WidgetDelegate::insertCoBData(QString str) {
    this->m_comboBoxList.append(str);
}

// 移除数据
void WidgetDelegate::removeCobData(QString str)
{
    for (int i = 0; i < this->m_comboBoxList.size(); i++)
    {
        if (str == this->m_comboBoxList[i])
        {
            this->m_comboBoxList.removeAt(i);
            return;
        }
    }
}


///////////////////////////不可编辑代理///////////////////////////////////////
TableItemDelegate::TableItemDelegate(QObject *parent) :QStyledItemDelegate(parent)
{

}
TableItemDelegate::~TableItemDelegate()
{

}

QWidget* TableItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return nullptr;
}
//void TableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
//{
////    QTextOption op;
////    op.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
////    QFont font;
////    font.setFamily("楷体");
////    font.setPixelSize(14);
////    painter->setFont(font);
////    //判断当前 item 是否选中
////    if (option.state & QStyle::State_Selected)
////    {
////        //当前 item 被选中，高亮
//////        painter->fillRect(option.rect, option.palette.highlight());
////        //自定义选中背景色
////        painter->fillRect(option.rect, QBrush(Qt::gray));
////    }
////    painter->drawText(option.rect, index.data(Qt::DisplayRole).toString(), op);
//}


////////////////////选择时间代理/////////////////////////////////
TableSelTimeDelegate::TableSelTimeDelegate(QObject *parent) :QStyledItemDelegate(parent)
{
    this->m_dateCss = QString("QCalendarWidget{background-color:#FFFFFF;border: 1px solid #%1;}"
                "QCalendarWidget QAbstractItemView:enabled{color:#000000;"
                                                    "background-color:#ffffff;"
                                                     "selection-color: white;"
                                                     "selection-background-color:#%1;}"
               "QCalendarWidget QSpinBox#qt_calendar_yearedit{background:#ffffff;height:35px;width:100px;"
                                                     "selection-background-color:#%1;}"
              "QCalendarWidget QToolButton{background-color:#FFFFFF;height:35px; width:100px;color:#000000;}"
              "QCalendarWidget QToolButton:hover{border: 1px solid #%1;}"
              "QCalendarWidget QToolButton::menu-indicator#qt_calendar_monthbutton{subcontrol-position: right center;subcontrol-origin: padding;}"
              "QCalendarWidget QToolButton QMenu{height:300px;background-color:#FFFFFF;width:100px;border:2px solid #%1;}"
              "QCalendarWidget QToolButton QMenu::item:selected{color:#FFFFFF;background:#%1;}").arg("CD9E94");

}
TableSelTimeDelegate::~TableSelTimeDelegate()
{

}

QWidget* TableSelTimeDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QDateTimeEdit *timeeditor = new QDateTimeEdit(parent);
    timeeditor->setDateTime(QDateTime::currentDateTime());
    timeeditor->setMinimumDate(QDate::currentDate().addDays(-365));  // -365天
    timeeditor->setMaximumDate(QDate::currentDate().addDays(365));  // +365天
    timeeditor->setCalendarPopup(true);
    timeeditor->setStyleSheet(this->m_dateCss);
    return timeeditor;

    //return QStyledItemDelegate::createEditor(parent, option, index);

}
void TableSelTimeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    // 将编辑器设置为矩形属性
    editor->setGeometry(option.rect);
}
void TableSelTimeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDateTimeEdit *ptimeedit = static_cast<QDateTimeEdit *>(editor);	// 类型转换
    // 模型（单元格）显示的数据
    model->setData(index, ptimeedit->text());
}
