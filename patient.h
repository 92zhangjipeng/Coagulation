#ifndef PATIENT_H
#define PATIENT_H

#include <QTableWidget>
#include <QWidget>
#include <QMenu>
#include <QAction>
#include<QMessageBox>
#include <QTreeWidgetItem>

namespace Ui
{
    class Patient;
}

class Patient : public QWidget
{
    Q_OBJECT

public:
    explicit Patient(QWidget *parent = 0);
    ~Patient();

private slots:










public slots:


signals:



private:
    Ui::Patient *ui;




};

#endif // PATIENT_H
