#include "freezetablewidget.h"

#include <QScrollBar>
#include <QHeaderView>

FreezeTableWidget::FreezeTableWidget(QAbstractItemModel *model /*= nullptr*/, int freezeColCounts)
{
    setModel(model);
    frozenTableView = new QTableView(this);
    m_iFreezeColCounts = freezeColCounts;

    init();

    //connect the headers and scrollbars of both tableviews together
    connect(horizontalHeader(),&QHeaderView::sectionResized, this,
            &FreezeTableWidget::updateSectionWidth);
    connect(verticalHeader(),&QHeaderView::sectionResized, this,
            &FreezeTableWidget::updateSectionHeight);

    connect(frozenTableView->verticalScrollBar(), &QAbstractSlider::valueChanged,
            verticalScrollBar(), &QAbstractSlider::setValue);
    connect(verticalScrollBar(), &QAbstractSlider::valueChanged,
            frozenTableView->verticalScrollBar(), &QAbstractSlider::setValue);
}
FreezeTableWidget::~FreezeTableWidget()
{
    delete frozenTableView;
    frozenTableView = nullptr;
}

void FreezeTableWidget::init()
{
    frozenTableView->setModel(model());
    frozenTableView->setFocusPolicy(Qt::NoFocus);
    frozenTableView->verticalHeader()->hide();
    frozenTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    //frozenTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    viewport()->stackUnder(frozenTableView);
    //! [init part1]

    //! [init part2]
    frozenTableView->setStyleSheet("QTableView { border: none;"
                                   "background-color: #8EDE21;"
                                   "selection-background-color: #999}"); //for demo purposes
    //设置和父table同步的光标选择单元格(必要)
    frozenTableView->setSelectionModel(selectionModel());

    
    for (int col = m_iFreezeColCounts; col < model()->columnCount(); ++col)
        frozenTableView->setColumnHidden(col, true);

    for(int i = 0; i <m_iFreezeColCounts; i++)
    {
        frozenTableView->setColumnWidth(i, columnWidth(0));
    }

    frozenTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenTableView->show();

    setHorizontalScrollMode(ScrollPerPixel);
    setVerticalScrollMode(ScrollPerPixel);
    frozenTableView->setVerticalScrollMode(ScrollPerPixel);
}

void FreezeTableWidget::updateSectionWidth(int logicalIndex, int /* oldSize */, int newSize)
{
    if (logicalIndex == m_iFreezeColCounts-1){
        int width = 0;
        for(int i = 0; i< m_iFreezeColCounts-1; i++)
        {
            width += columnWidth(i);
        }

        for(int i = 0; i< m_iFreezeColCounts; i++)
        {
            frozenTableView->setColumnWidth(i, (newSize+width)/m_iFreezeColCounts);
        }


        updateFrozenTableGeometry();
    }
}

void FreezeTableWidget::updateSectionHeight(int logicalIndex, int /* oldSize */, int newSize)
{
    frozenTableView->setRowHeight(logicalIndex, newSize);
}
//! [sections]


//! [resize]
void FreezeTableWidget::resizeEvent(QResizeEvent * event)
{
    QTableView::resizeEvent(event);
    updateFrozenTableGeometry();
}
//! [resize]


//! [navigate]
//QModelIndex FreezeTableWidget::moveCursor(CursorAction cursorAction,
//                                          Qt::KeyboardModifiers modifiers)
//{
//      QModelIndex current = QTableView::moveCursor(cursorAction, modifiers);

//      if (cursorAction == MoveLeft && current.column() > 0
//              && visualRect(current).topLeft().x() < columnWidth(0) ){
//            const int newValue = horizontalScrollBar()->value() + visualRect(current).topLeft().x()
//                                 - frozenTableView->columnWidth(0);
//            horizontalScrollBar()->setValue(newValue);
//      }

//      return current;
//}
//! [navigate]

void FreezeTableWidget::scrollTo (const QModelIndex & index, ScrollHint hint){
    //if (index.column() > 0)
    QTableView::scrollTo(index, hint);
}

//! [geometry]
void FreezeTableWidget::updateFrozenTableGeometry()
{
    int width = 0;
    for(int i = 0; i< m_iFreezeColCounts; i++)
    {
        width += columnWidth(i);
    }
    frozenTableView->setGeometry(verticalHeader()->width() + frameWidth(),
                                 frameWidth(),width ,
                                 viewport()->height()+horizontalHeader()->height());
}


