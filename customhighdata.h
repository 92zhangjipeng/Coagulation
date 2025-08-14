#ifndef CUSTOMHIGHDATA_H
#define CUSTOMHIGHDATA_H

#include <QTimerEvent>
#include <QWidget>
#include "cglobal.h"

namespace Ui {
class CustomHighData;
}

class CustomHighData : public QWidget
{
    Q_OBJECT

public:
    explicit CustomHighData(QString Reminder,int index,int Types,unsigned short Value,QWidget *parent = 0);
    ~CustomHighData();
protected:
    virtual void closeEvent(QCloseEvent* event) override;
    void timerEvent(QTimerEvent *event) override;
private slots:
    void on_toolButton_ok_clicked();
	void onTimeOutOK();
signals:
    void AddSupplies(int, unsigned short);
    void closeDelWidget();
private:
    Ui::CustomHighData *ui;
    int  Supplies_type ;
    int Supplies_AddNum ;
    unsigned short Supplies_state;
    QTimer *tim = nullptr;
    int mcloseTimerId;
    
};

#endif // CUSTOMHIGHDATA_H




