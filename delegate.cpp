#include "delegate.h"
#include <QComboBox>
#include <qDebug>
#include <vector>
#include <iostream>
#include <QJsonParseError>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

Delegate::Delegate(char Show,QObject *parent)
     : QItemDelegate(parent)
{
    m_loadShow = Show;
    m_AnalyticallistData.clear();
    AnalyticalJsonUse();
}

 Delegate::~Delegate()
{

}
 void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
     QStyleOptionViewItem itemOption(option);
     if (itemOption.state & QStyle::State_HasFocus)
     {
        itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
     }
     //QStyledItemDelegate::paint(painter, itemOption, index);
     QItemDelegate::paint(painter, itemOption, index);
     //QItemDelegate::paint(painter, option, index);
 }
QSize Delegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QItemDelegate::sizeHint(option, index);
}

QWidget *Delegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.isValid() && index.column() == m_loadShow)
    {
        QComboBox *editor = new QComboBox(parent);
        editor->setEditable(true);
        editor->installEventFilter(const_cast<Delegate *>(this));
        return editor;
    }
    else
    {
        return QItemDelegate::createEditor(parent, option, index);
    }
}

void Delegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.isValid() && index.column() == COMBOXSEX)
    {
         QString value = index.model()->data(index, Qt::DisplayRole).toString();
         QComboBox *combox = static_cast<QComboBox *>(editor);
         QMap<QString ,QStringList>::const_iterator it;
         for(it = m_show_text.constBegin(); it != m_show_text.constEnd();it++){
             if(it.key() == "性别")
                 combox->addItems(it.value());
         }
         combox->setCurrentText(value);
    }
    else if(index.isValid() && index.column() == COMBOXPART)
    {
        QString value = index.model()->data(index, Qt::DisplayRole).toString();
        QComboBox *combox = static_cast<QComboBox *>(editor);
        combox->setCurrentText(value);
        QMap<QString ,QStringList>::const_iterator it;
        for(it = m_show_text.constBegin(); it != m_show_text.constEnd();it++){
            if(it.key() == "科别")
                combox->addItems(it.value());
        }
    }
    else if(index.isValid() && index.column() == COMBOX_8)
    {
        QString value = index.model()->data(index, Qt::DisplayRole).toString();
        QComboBox *combox = static_cast<QComboBox *>(editor);
        combox->setCurrentText(value);
        QMap<QString ,QStringList>::const_iterator it;
        for(it = m_show_text.constBegin(); it != m_show_text.constEnd();it++){
            if(it.key() == "病区")
                combox->addItems(it.value());  
        }
    }
    else if(index.isValid() && index.column() == COMBOX_9)
    {
        QString value = index.model()->data(index, Qt::DisplayRole).toString();
        QComboBox *combox = static_cast<QComboBox *>(editor);
        combox->setCurrentText(value);
        QMap<QString ,QStringList>::const_iterator it;
        for(it = m_show_text.constBegin(); it != m_show_text.constEnd();it++){
            if(it.key() == "床号")
                combox->addItems(it.value());
        }
    }
    else if(index.isValid() && index.column() == COMBOX_10)
    {
        QString value = index.model()->data(index, Qt::DisplayRole).toString();
        QComboBox *combox = static_cast<QComboBox *>(editor);
        combox->setCurrentText(value);
        QMap<QString ,QStringList>::const_iterator it;
        for(it = m_show_text.constBegin(); it != m_show_text.constEnd();it++)
        {
            if(it.key() == "临床诊断")
            {
                combox->addItems(it.value());
            }
        }
    }
    else if(index.isValid() && index.column() == COMBOX_11)
    {
        QString value = index.model()->data(index, Qt::DisplayRole).toString();
        QComboBox *combox = static_cast<QComboBox *>(editor);
        combox->setCurrentText(value);
        QMap<QString ,QStringList>::const_iterator it;
        for(it = m_show_text.constBegin(); it != m_show_text.constEnd();it++)
        {
            if(it.key() == "送检医生")
            {
                combox->addItems(it.value());
            }
        }
    }
    else if(index.isValid() && index.column() == COMBOX_12)
    {
        QString value = index.model()->data(index, Qt::DisplayRole).toString();
        QComboBox *combox = static_cast<QComboBox *>(editor);
        combox->setCurrentText(value);
        QMap<QString ,QStringList>::const_iterator it;
        for(it = m_show_text.constBegin(); it != m_show_text.constEnd();it++)
        {
            if(it.key() == "备注")
                combox->addItems(it.value());
        }
    }
    else{
         QItemDelegate::setEditorData(editor, index);
     }
}

void Delegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                              const QModelIndex &index) const
{
    if (index.isValid() && index.column() == m_loadShow)
    {
        QComboBox *combox = static_cast<QComboBox *>(editor);
        model->setData(index, combox->currentText());
    }
    else
    {
        QItemDelegate::setModelData(editor, model, index);
    }
}

void Delegate::AnalyticalJsonUse()
{
    QString temp_str;
    m_show_text.clear();
    QStringList CategoryList;
    CategoryList.clear();
    CategoryList <<tr("科别")<<tr("性别")<<tr("病区")<<tr("床号")<<tr("临床诊断")<<tr("送检医生")<<tr("检验医生")<<tr("备注");
    int nSize = CategoryList.size();
    for(int i = 0 ;i<nSize ; i++)
    {
        m_AnalyticallistData.clear();
        QString objname = CategoryList.at(i);
        //QString path = QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg("患者信息配置.json");
        m_Analytical.AnalyticalJsonFile("通用信息",objname,m_AnalyticallistData,m_Analytical.JsonFilePath(1));
        QStringList templist;
        templist.clear();
        for(int j = 0; j<m_AnalyticallistData.size() ;j++)
        {
             temp_str =m_AnalyticallistData.at(j).toString();
             templist.append(temp_str);
             m_show_text.insert(objname,templist);
        }
    }
}

