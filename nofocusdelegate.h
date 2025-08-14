#ifndef NOFOCUSDELEGATE_H
#define NOFOCUSDELEGATE_H

#include <QObject>
#include <QWidget>
#include<QPainter>
#include<QModelIndex>
#include<QStyleOptionViewItem>
#include<QStyledItemDelegate>

class NoFocusDelegate
{
public:
    NoFocusDelegate();
    void paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex &index) const;
};

#endif // NOFOCUSDELEGATE_H
