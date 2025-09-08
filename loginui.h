#ifndef LOGINUI_H
#define LOGINUI_H
#include <QColor>
#include <QMessageBox>
#include <QMainWindow>
#include <mainwindow.h>
#include "loadequipmentpos.h"
#include "cglobal.h"


namespace Ui
{
    class LoginUi;
}

class LoginUi : public QWidget
{
    Q_OBJECT
public:
    explicit LoginUi(QWidget *parent = 0);
    ~LoginUi();
protected:

signals:

private slots:

public slots:

private:
    Ui::LoginUi *ui;

};

#endif // LOGINUI_H
