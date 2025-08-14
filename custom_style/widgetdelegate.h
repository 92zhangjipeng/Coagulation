#ifndef WIDGETDELEGATE_H
#define WIDGETDELEGATE_H

#include <QObject>
#pragma once
#include <QItemDelegate>
#include <QPainter>
#include <QStyledItemDelegate>


class TableItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    TableItemDelegate(QObject * parent=nullptr);
    ~TableItemDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    //void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    //void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    //void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    //void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};


class TableSelTimeDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    TableSelTimeDelegate(QObject * parent=nullptr);
    ~TableSelTimeDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const override;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    //void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    //void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    //void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
private:
    QString m_dateCss;
};




class WidgetDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    WidgetDelegate(QItemDelegate *parent = nullptr);
    WidgetDelegate(QStringList list, QItemDelegate *parent = nullptr);
    ~WidgetDelegate();

    // 创建编辑器
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    // 设置编辑器数据
    virtual void setEditorData(QWidget *editor, const QModelIndex &) const override;
    // 更新编辑器集合属性
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    // 设置模型数据
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    /*****根据项目需求增添辅佐函数*****/

    // 获取索引数据
    QString getCurrentComboBoxData(int index);
    // 插入
    void insertCoBData(QString str);
    // 删除
    void removeCobData(QString str);

private:
    QStringList m_comboBoxList;
	QString m_Css;

};

#endif // WIDGETDELEGATE_H
