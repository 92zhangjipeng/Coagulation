#ifndef CALIBRATIONPOSITION_H
#define CALIBRATIONPOSITION_H

#include <QWidget>
#include "testing.h"

namespace Ui {
class CalibrationPosition;
}

class CalibrationPosition : public QWidget
{
    Q_OBJECT

public:
    explicit CalibrationPosition(QWidget *parent = 0);
    ~CalibrationPosition();

public slots:


private slots:

private:
    Ui::CalibrationPosition *ui;
public:


};

#endif // CALIBRATIONPOSITION_H
