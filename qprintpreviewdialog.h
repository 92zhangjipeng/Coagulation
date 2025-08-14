#ifndef QPRINTPREVIEWDIALOG_H
#define QPRINTPREVIEWDIALOG_H

#include <QWidget>
#include<QtPrintSupport/QPrintDialog>
#include<QtPrintSupport/QPrinter>

namespace Ui {
class QPrintPreviewDialog;
}

class QPrintPreviewDialog : public QWidget
{
    Q_OBJECT

public:
    explicit QPrintPreviewDialog(QWidget *parent = 0);
    ~QPrintPreviewDialog();

private:
    Ui::QPrintPreviewDialog *ui;
};

#endif // QPRINTPREVIEWDIALOG_H
