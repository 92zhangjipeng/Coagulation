#ifndef DELEGATE_H
#define DELEGATE_H

#include <QStyledItemDelegate>
#include <QItemDelegate>
#include <QModelIndex>
#include <QPainter>
#include <QWidget>
#include "analyticaljson.h"

#define  COMBOXSEX           3
#define  COMBOXPART          6
#define  COMBOX_8            8
#define  COMBOX_9            9
#define  COMBOX_10           10
#define  COMBOX_11           11
#define  COMBOX_12           12
class Delegate : public QItemDelegate
{
    Q_OBJECT

public:
    Delegate(char Show,QObject *parent = nullptr);
    ~Delegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
                     const QModelIndex &index) const;
     QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const;
     void setEditorData(QWidget *editor, const QModelIndex &index) const;
     void setModelData(QWidget *editor, QAbstractItemModel *model,
                                     const QModelIndex &index) const;
     AnalyticalJson m_Analytical;
     char m_loadShow;
 private:
     QVariantList m_AnalyticallistData;
     QMap <QString ,QStringList> m_show_text;
private slots:
    void AnalyticalJsonUse();



};

#endif // DELEGATE_H
