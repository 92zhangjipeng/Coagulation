#include "widgetdelegate.h"
#include <QComboBox>
#include <QDateTimeEdit>
#include <QListWidget>
#include <QStandardItemModel>
#include <QTextOption>

#pragma warning(disable:4100)

WidgetDelegate::WidgetDelegate(QItemDelegate *parent) : QItemDelegate(parent)
{
   this->m_comboBoxList<< "";
}

WidgetDelegate::WidgetDelegate(QStringList list, QItemDelegate *parent) :QItemDelegate(parent)
{
    this->m_comboBoxList = list;
    this->m_Css = "QComboBox{border-radius:5px; padding:2px 4px; font: 19 17pt '楷体'; border: 2px solid rgba(220,220,220);background-color:rgba(188,187,183);}"
                  "QComboBox::drop-down {subcontrol-origin: padding;subcontrol-position: top right;width: 15px; border:none;}"
                  "QComboBox::down-arrow {height:30px; width:30px; image: url(:/Picture/minus.png);}"
                  "QComboBox QAbstractItemView{background:rgba(255,255,255,1);border:1px solid rgba(150,150,150,1);border-radius:0px 0px 5px 5px;font: 24px; font-family: 楷体;outline: 0px;}"
                  "QComboBox QAbstractItemView::item{ height:36px; color:#666666; padding-left:9px; background-color:#FFFFFF;}"
                  "QComboBox QAbstractItemView::item:hover{background-color:#409CE1;color:#ffffff;}"
                  "QComboBox QAbstractItemView::item:selected{background-color:#409CE1;color:#ffffff;}"
                  "QComboBox:on { padding-top: 3px;padding-left: 3px;}"
                  "QComboBox::down-arrow:on {top: 1px;left:3px;}";
}

WidgetDelegate::~WidgetDelegate() {

}


QWidget *WidgetDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 创建自己需要的控件进行返回
    QComboBox *editor = new QComboBox(parent);
    editor->setStyleSheet(this->m_Css);
    //static_cast<QStandardItemModel*>(editor->model())->item(pos)->setTextAlignment(Qt::AlignCenter);
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
    // 将编辑器设置为矩形属性
    editor->setGeometry(option.rect);
}

// 设置模型数据
void WidgetDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox *>(editor);	// 类型转换
    // 模型（单元格）显示的数据
    model->setData(index, comboBox->currentText());
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
