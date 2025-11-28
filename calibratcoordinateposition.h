#ifndef CALIBRATCOORDINATEPOSITION_H
#define CALIBRATCOORDINATEPOSITION_H

#include <QWidget>

namespace Ui {
class CalibratCoordinatePosition;
}

class CalibratCoordinatePosition : public QWidget
{
    Q_OBJECT

public:
    explicit CalibratCoordinatePosition(QWidget *parent = 0);
    ~CalibratCoordinatePosition();

private:
    Ui::CalibratCoordinatePosition *ui;
};

#endif // CALIBRATCOORDINATEPOSITION_H
