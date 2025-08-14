#ifndef FUCNEQUIPMENTMAIN_H
#define FUCNEQUIPMENTMAIN_H

#include <QWidget>

namespace Ui {
class FucnEquipmentMain;
}

class FucnEquipmentMain : public QWidget
{
    Q_OBJECT

public:
    explicit FucnEquipmentMain(QWidget *parent = 0);
    ~FucnEquipmentMain();

private:
    Ui::FucnEquipmentMain *ui;
};

#endif // FUCNEQUIPMENTMAIN_H
