#ifndef COORDINATEPPOSIT_H
#define COORDINATEPPOSIT_H

#include <QWidget>

namespace Ui {
class CoordinatepPosit;
}

class CoordinatepPosit : public QWidget
{
    Q_OBJECT

public:
    explicit CoordinatepPosit(QWidget *parent = 0);
    ~CoordinatepPosit();

private:
    Ui::CoordinatepPosit *ui;
};

#endif // COORDINATEPPOSIT_H
