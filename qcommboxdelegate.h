#ifndef QCOMMBOXDELEGATE_H
#define QCOMMBOXDELEGATE_H

#pragma execution_character_set("utf-8")

#include <QComboBox>
#include <qDebug>
#include <QApplication>
#include <QMouseEvent>
#include <QStyledItemDelegate>

class QCommboxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    QCommboxDelegate(QObject *parent = 0,  QStringList list = QStringList() << "", bool editable = false, QString mask = "");

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                               const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                           const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
     QStringList m_qlist;
     bool m_bEdit;
     QString m_qszMask;

signals:
     void comboxChanged(const QString & para);


public slots:
    void OnComboBoxChanged(const QString & para)
    {
        emit comboxChanged(para);
    }

 public:
    mutable QWidget *m_pEditor;

};

#endif // QCOMMBOXDELEGATE_H
