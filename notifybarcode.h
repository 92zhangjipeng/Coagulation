#ifndef NOTIFYBARCODE_H
#define NOTIFYBARCODE_H
#pragma execution_character_set("utf-8")

#include <QWidget>
namespace Ui {
class NotifyBarCode;
}

class NotifyBarCode : public QWidget
{
    Q_OBJECT

public:
    explicit NotifyBarCode(QWidget *parent = 0);
    ~NotifyBarCode();

    void changerowAndCol(const int , const int Column, const QString changeData);
protected:
   void closeEvent(QCloseEvent *event);
signals:
    void savebarcode(unsigned int row,quint8 cols,QString noityBarcode);
private slots:

    void on_toolButton_save_clicked();

    void on_toolButton_cancel_clicked();

private:
    Ui::NotifyBarCode *ui;
    unsigned int mrows;
    quint8 mcols;
};

#endif // NOTIFYBARCODE_H
