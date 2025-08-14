#ifndef CUSTOMCURVESHOW_H
#define CUSTOMCURVESHOW_H

#include <QWidget>

namespace Ui {
class CustomCurveShow;
}

class CustomCurveShow : public QWidget
{
    Q_OBJECT

public:
    explicit CustomCurveShow(QWidget *parent = 0);
    ~CustomCurveShow();

private:
    Ui::CustomCurveShow *ui;
};

#endif // CUSTOMCURVESHOW_H
