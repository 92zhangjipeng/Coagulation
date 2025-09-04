#ifndef ALIGNDELEGATE_H
#define ALIGNDELEGATE_H

#include <QStyledItemDelegate>
#include <QLineEdit>


class AlignDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit AlignDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const override
    {
        QWidget *editor = QStyledItemDelegate::createEditor(parent, option, index);
        QFont cellFont;
        cellFont.setPointSize(11); // 设置较大的字号
        cellFont.setBold(false);
        if (editor && editor->inherits("QLineEdit")) {
            QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
            if (lineEdit) {
                lineEdit->setAlignment(Qt::AlignCenter);
                lineEdit->setFont(cellFont);
            }
        }
        return editor;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override
    {
        QStyledItemDelegate::setEditorData(editor, index);
        if (editor && editor->inherits("QLineEdit")) {
            QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
            if (lineEdit) {
                lineEdit->setAlignment(Qt::AlignCenter);
            }
        }
    }
};
#endif // ALIGNDELEGATE_H
