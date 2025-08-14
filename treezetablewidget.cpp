#include "treezetablewidget.h"

#include <QScrollBar>
#include <QHeaderView>
#include <QDebug>
#include <QMenu>


EP_TableView::EP_TableView(QWidget *parent) : QTableView(parent)
{
    frozenTableView = new QTableView(this);
    frozenTableView->setFocusPolicy(Qt::NoFocus);
    frozenTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    frozenTableView->horizontalHeader()->setStretchLastSection(true);
    frozenTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    init();
    m_pmenu = new QMenu(frozenTableView);
    m_pmenu->addAction("删除");
    m_pmenu->addAction("删除");

    this->setFocus();

    connect(frozenTableView,&QTableView::customContextMenuRequested,this,&EP_TableView::slotCustomContexMenu);

    //connect the headers and scrollbars of both tableviews together
    connect(horizontalHeader(),&QHeaderView::sectionResized, this,
            &EP_TableView::updateSectionWidth);
    connect(verticalHeader(),&QHeaderView::sectionResized, this,
            &EP_TableView::updateSectionHeight);

    connect(frozenTableView->verticalScrollBar(), &QScrollBar::valueChanged,
            this->verticalScrollBar(), &QScrollBar::setValue);

}

EP_TableView::~EP_TableView()
{

}

void EP_TableView::SetModel(QAbstractItemModel *qmodel)
{
    this->setModel(qmodel);

    frozenTableView->setModel(qmodel);
    frozenTableView->setSelectionModel(selectionModel());
    for (int col = 1; col < model()->columnCount(); ++col)
        frozenTableView->setColumnHidden(col, true);

    frozenTableView->setColumnWidth(0, columnWidth(0));
}

void EP_TableView::resizeEvent(QResizeEvent *event)
{
    QTableView::resizeEvent(event);
    updateFrozenTableGeometry();
}

QModelIndex EP_TableView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    QModelIndex current = QTableView::moveCursor(cursorAction, modifiers);

    if (cursorAction == MoveLeft && current.column() > 0
            && visualRect(current).topLeft().x() < frozenTableView->columnWidth(0) ){
        const int newValue = horizontalScrollBar()->value() + visualRect(current).topLeft().x()
                - frozenTableView->columnWidth(0);
        horizontalScrollBar()->setValue(newValue);
    }
    return current;
}


void EP_TableView::init()
{
    frozenTableView->setFocusPolicy(Qt::NoFocus);
    frozenTableView->verticalHeader()->hide();
    frozenTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    frozenTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    viewport()->stackUnder(frozenTableView);

    frozenTableView->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1,\
                                                             stop:0 rgba(69,133,245,38), stop: 0.5 rgba(69,133,245,38),stop: 0.6 rgba(69,133,245,38), stop:1 rgba(69,133,245,38));color: black;border:1px solid rgba(69,133,245,38)}");     //border:1px solid rgba(153,153,153,51)
    frozenTableView->horizontalHeader()->setFixedHeight(40);

    frozenTableView->setStyleSheet("border:1px solid rgba(153,153,153,51)");


    frozenTableView->viewport()->setStyleSheet("border-radius:15px;background:transparent;");

    frozenTableView->horizontalHeader()->viewport()->setStyleSheet("border-radius:15px;background:transparent;");
    frozenTableView->verticalHeader()->viewport()->setStyleSheet("border-radius:15px;background:transparent;");


    frozenTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenTableView->show();

    frozenTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(frozenTableView, &QTableView::customContextMenuRequested, this, &EP_TableView::customContextMenuRequested);

    updateFrozenTableGeometry();

    setHorizontalScrollMode(ScrollPerPixel);
    setVerticalScrollMode(ScrollPerPixel);
    frozenTableView->setVerticalScrollMode(ScrollPerPixel);
}

void EP_TableView::updateFrozenTableGeometry()
{
    frozenTableView->setGeometry(verticalHeader()->width() + frameWidth(),
                                       frameWidth(), columnWidth(0),
                                       viewport()->height()+horizontalHeader()->height());
}

void EP_TableView::updateSectionWidth(int logicalIndex, int oldSize, int newSize)
{
    if (logicalIndex == 0){
        frozenTableView->setColumnWidth(0, newSize);
        updateFrozenTableGeometry();
    }
}

void EP_TableView::updateSectionHeight(int logicalIndex, int oldSize, int newSize)
{
    frozenTableView->setRowHeight(logicalIndex, newSize);
}

void EP_TableView::slotCustomContexMenu(const QPoint &pos)
{
    QPoint p;
    p.setX(pos.x());
    p.setY(pos.y());
    m_pmenu->exec(frozenTableView->mapToGlobal(p));
}



