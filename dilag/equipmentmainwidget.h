#ifndef EQUIPMENTMAINWIDGET_H
#define EQUIPMENTMAINWIDGET_H

#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace Ui {
class EquipmentMainWidget;
}

class EquipmentMainWidget : public QDialog
{
    Q_OBJECT

public:
    explicit EquipmentMainWidget(QWidget *parent = 0);
    ~EquipmentMainWidget();

    void LoadPushBtnImagAndText(QPushButton* pFucnBtn, QString SetTextStr, QString LoadPathImage);

private:

private:
    Ui::EquipmentMainWidget *ui;
};

#endif // EQUIPMENTMAINWIDGET_H
