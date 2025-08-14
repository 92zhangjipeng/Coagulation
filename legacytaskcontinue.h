#ifndef LEGACYTASKCONTINUE_H
#define LEGACYTASKCONTINUE_H

#include <QTableWidget>
#include <QWidget>
#include "testing.h"

namespace Ui {
class LegacyTaskContinue;
}

class LegacyTaskContinue : public QWidget
{
    Q_OBJECT

public:
    explicit LegacyTaskContinue(QWidget *parent = 0);
    ~LegacyTaskContinue();
private:
    void Initialize_the_interface(QTableWidget *Table);
private slots:
    void on_toolButton_continue_clicked();
signals:

public slots:
    void SlotExcptionSampleNum(const int);
private:
    Ui::LegacyTaskContinue *ui;
};

#endif // LEGACYTASKCONTINUE_H
