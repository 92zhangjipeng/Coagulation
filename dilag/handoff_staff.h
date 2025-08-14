#ifndef HANDOFF_STAFF_H
#define HANDOFF_STAFF_H

#include <QWidget>

namespace Ui {
class handoff_staff;
}

class handoff_staff : public QWidget
{
    Q_OBJECT

public:
    explicit handoff_staff(QWidget *parent = 0);
    ~handoff_staff();

    void setTitlename(QString titlename);

protected:
    virtual void closeEvent(QCloseEvent* event) override;

private slots:
    void on_toolButton_handofstaff_clicked();

signals:
    void closeSwitchUser(QString);

private:
    Ui::handoff_staff *ui;

    QString m_Titlename;
};

#endif // HANDOFF_STAFF_H
