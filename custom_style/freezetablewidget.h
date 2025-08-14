#ifndef FREEZETABLEWIDGET_H
#define FREEZETABLEWIDGET_H

#include <QHeaderView>
#include <QObject>
#include <QPaintEvent>
#include <QTableWidget>
#include <QTableView>


class FreezeTableWidget : public QTableView
{
    Q_OBJECT

public:
        explicit  FreezeTableWidget(QAbstractItemModel * model , int freezeColCounts);
        ~FreezeTableWidget();

protected:
      void resizeEvent(QResizeEvent *event) override;
      //QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
      void scrollTo (const QModelIndex & index, ScrollHint hint = EnsureVisible) override;

private:
      QTableView *frozenTableView;
      void init();
      void updateFrozenTableGeometry();
      int m_iFreezeColCounts = 1;


private slots:
      void updateSectionWidth(int logicalIndex, int oldSize, int newSize);
      void updateSectionHeight(int logicalIndex, int oldSize, int newSize);
};

#endif // FREEZETABLEWIDGET_H
