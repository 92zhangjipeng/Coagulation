#ifndef RESULT_H
#define RESULT_H

#include <QWidget>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QFileDialog>
#include <QPageSetupDialog>
#include <QPoint>
#include <QMap>
#include <QTableWidget>
#include "QCustomPlot.h"
#include "inquire_sql_info.h" /*查询*/
#include "cglobal.h"

namespace Ui {
class Result;
}

class Result : public QWidget
{
    Q_OBJECT

public:
    explicit Result(QWidget *parent = 0);
    ~Result();
protected:

private slots:
    void Slot_PrintResult();
    void Slot_PrintSetup();
    void Slot_PrintView();
    void printPreview(QPrinter*);
    void createPix(QPixmap *pix);

signals:

    void  CurveData(QString,QCustomPlot *customPlot);
    void  CleanningCurve(QCustomPlot *customPlot);
public slots:

	
private:
    Ui::Result *ui;


};

#endif // RESULT_H
