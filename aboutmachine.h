#ifndef ABOUTMACHINE_H
#define ABOUTMACHINE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCloseEvent>
#include "cglobal.h"
#include "opencvFindRBC/customtitlebar.h"

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
    void sycnVersion(const QString version);
    void sycnSerialname(QString _serialname);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onMinimizeRequested();
    void onMaximizeRequested();
    void onCloseRequested();
    void onConfirmClicked();

private:
    void initUI();
    void initStyle();

    Ui::AboutMachine *ui;
    CustomTitleBar *m_titleBar;

    QWidget *m_centerWidget;
    QVBoxLayout *m_mainLayout;
    QVBoxLayout *m_centerLayout;

    QLabel *m_companyLabel;
    QLabel *m_versionLabel;
    QLabel *m_machineLabel;
    QFrame *m_lineFrame;
    QLabel *m_connectionLabel;
    QLabel *m_moduleLabel;
    QLabel *m_hashLabel;
    QPushButton *m_confirmBtn;

    QString m_Version;
    QString m_Machine;
};

#endif // ABOUTMACHINE_H
