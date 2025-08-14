#ifndef CANCELTASKCONFIGURE_H
#define CANCELTASKCONFIGURE_H

#include <QDialog>
#include <QLabel>
//增加相关头文件
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QPainter>
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <QMimeData>
#include <QDrag>
#include <QTableWidget>


namespace Ui {
class CancelTaskConfigure;
}

class CancelTaskConfigure : public QDialog
{
    Q_OBJECT

public:
    explicit CancelTaskConfigure(QWidget *parent , QList<int> click_sampleid);
    ~CancelTaskConfigure();
private:
    QList<int> mclickHole;

    void initLayout();

    void SetColumnText(QTableWidget * tablewiget,int row,int col,QString text);


signals:
    void makesureCacelIdList(QList<int> );

public slots:

    void delenter(QTableWidgetItem *item);

private slots:
    void on_ok_clicked();

    void on_Cancel_clicked();

private:
    Ui::CancelTaskConfigure *ui;

    QList<int> deleteitem;

    QString CancelVScroBarCss =
            "QScrollBar:vertical {width: 18px; background: transparent; margin: 0px,0px,0px,0px; padding-top: 18px;padding-bottom: 18px;} "
            "QScrollBar::handle:vertical {width: 18px; background: rgba(0,0,0,25%);border-radius: 4px;min-height: 20;}"
            "QScrollBar::handle:vertical:hover {width: 8px;background: rgba(0,0,0,50%);border-radius: 4px; min-height: 20;}"
            "QScrollBar::add-line:vertical {height: 18px;width: 18px;border-image: url(:/Picture/minus.png);subcontrol-position: bottom;}"
             "QScrollBar::sub-line:vertical {height: 18px;width: 18px;border-image: url(:/Picture/Quality.png);subcontrol-position: top;}"
            "QScrollBar::add-line:vertical:hover {height: 18px;width: 18px;border-image: url(:/Picture/minus.png);subcontrol-position: bottom;}"
            "QScrollBar::sub-line:vertical:hover {height: 18px;width: 18px;border-image: url(:/Picture/Quality.png);subcontrol-position: top;}"
            "QScrollBar::sub-page:vertical {background: rgb(178,180,180); border-radius: 0px;}"
            "QScrollBar::add-page:vertical {background: rgb(178,180,180); border-radius: 0px;}";

    QString CancelTableWidgetCss = "QTableWidget::item:hover{background-color:rgb(70 ,130 ,180)}"
                             "QTableWidget::item:selected{background-color:rgb(139, 139, 122)}"
                              "QTableView QTableCornerButton::section{color: white; background-color: rgb(188, 187, 186); "
                              "border: 1px solid rgb(188, 187, 186);border-radius:0px; border-color: rgb(188, 187, 186);"
                              "font: bold 1pt;padding:12px 0 0 10px}"
                               "QHeaderView::section,QTableCornerButton:section{ \
                               padding:3px; margin:0px; color:rgba(188, 187, 186, 255);  border:1px solid rgba(188, 187, 186, 255); \
                               border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
                               background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
                               "QTableWidget{background-color:white;border:none;}"
                               "QHeaderView::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\
                               stop:0 rgba(188, 187, 186, 255), stop: 0.5 rgba(188, 187, 186, 255),stop: 0.6 rgba(188, 187, 186, 255), stop:1 rgba(188, 187, 186, 255)); color: white;}"
                               "QTableView QTableCornerButton::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\
                               stop:0 rgba(188, 187, 186, 255), stop: 0.5 rgba(188, 187, 186, 255),stop: 0.6 rgba(188, 187, 186, 255), stop:1 rgba(188, 187, 186, 255)); color: white;}";
};

#endif // CANCELTASKCONFIGURE_H
