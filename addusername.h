#ifndef ADDUSERNAME_H
#define ADDUSERNAME_H

#include <QCloseEvent>
#include <QWidget>
#include  <qDebug>
#include "testing.h"
namespace Ui {
class AddUserName;
}

class AddUserName : public QWidget
{
    Q_OBJECT

public:
    explicit AddUserName(char UserSet,QWidget *parent = 0);
    ~AddUserName();
     void closeEvent(QCloseEvent *event);
     void InitUi();
private slots:
    void on_pushButton_Add_clicked();
    void on_pushButton_Cancel_clicked();
    void on_OK_clicked();
    void on_Cancel_clicked();

public slots:
	void ModifyUsername_Slot(QString);

signals:
    void AddUser(const QString,const QString, bool);
private:
    Ui::AddUserName *ui;
    char m_show;
    QString m_NotifyUsername;
};

#endif // ADDUSERNAME_H
