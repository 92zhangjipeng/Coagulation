#include "qcommboxdelegate.h"



QCommboxDelegate::QCommboxDelegate(QObject *parent, const QStringList list/* = QStringList() << ""*/, bool editable/* = false*/, QString mask/* = ""*/)
 :QStyledItemDelegate(parent)
{
    m_qlist = list;
    m_bEdit = editable;
    m_qszMask = mask;
}
QWidget *QCommboxDelegate::createEditor(QWidget *parent,
     const QStyleOptionViewItem &/* option */,
     const QModelIndex &index ) const
 {
     QComboBox *editor = new QComboBox(parent);
     editor->insertItems(0, m_qlist);
     //editor->setCurrentIndex(0);
     //editor->setCurrentItem(m_qlist.findIndex(m_qlist[0]));
     m_pEditor = editor;
     connect(m_pEditor, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(OnComboBoxChanged(const QString &)));
     return editor;
 }

void QCommboxDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{
     QString value = index.model()->data(index, Qt::EditRole).toString();
     QComboBox *comboBox = static_cast<QComboBox*>(editor);
     comboBox->setEditable(m_bEdit);
     if ( m_qszMask != "" )
     {
         QValidator* validator = new QRegExpValidator( QRegExp(m_qszMask), 0);
         comboBox->setValidator(validator);
     }
     if(m_qlist.indexOf(value) == -1) return; //确保不删除列表中不存在的数据
     comboBox->setCurrentIndex(m_qlist.indexOf(value));

}

void QCommboxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
 {
     QComboBox *comboBox = static_cast<QComboBox*>(editor);
     comboBox->setEditable(m_bEdit);
     QString value = comboBox->currentText();
     model->setData(index, value);
 }
void QCommboxDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
