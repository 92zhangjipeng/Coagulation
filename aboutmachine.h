#ifndef ABOUTMACHINE_H
#define ABOUTMACHINE_H

#include <QWidget>
#include "cglobal.h"

#define OUR_COMPANY_NAME "全自动血小板聚集分析系统"


namespace Ui {
class AboutMachine;
}

class AboutMachine : public QWidget
{
    Q_OBJECT

public:
    explicit AboutMachine(QWidget *parent = 0);
    ~AboutMachine();

    void sycnMd5Value(const QString& exePath);

    void sycnEquipment(quint8 types);

    void sycnVersion(const QString );

    void sycnSerialname(QString _serialname);

private slots:


private:
    Ui::AboutMachine *ui;
    QString m_Version;
    QString m_Machine;
    QString m_info;

    void initStyleAbout();
};

#endif // ABOUTMACHINE_H
