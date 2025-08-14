
#include <QTableView>

#ifndef EP_TABLEVIEW_H
#define EP_TABLEVIEW_H

#include <QWidget>
#include <QTableView>
class  QMenu;

class EP_TableView : public QTableView
{
    Q_OBJECT
public:
    explicit EP_TableView(QWidget *parent = nullptr);
    ~EP_TableView();

    void SetModel(QAbstractItemModel *model);


protected:
      void resizeEvent(QResizeEvent *event) override;
      QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;

private:
      QTableView *frozenTableView = nullptr;
      QMenu *m_pmenu = NULL;
      void init();
      void updateFrozenTableGeometry();

private slots:
      void updateSectionWidth(int logicalIndex, int oldSize, int newSize);
      void updateSectionHeight(int logicalIndex, int oldSize, int newSize);
      void slotCustomContexMenu(const QPoint &pos);

};

#endif // EP_TABLEVIEW_H



