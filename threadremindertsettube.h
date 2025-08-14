#ifndef THREADREMINDERTSETTUBE_H
#define THREADREMINDERTSETTUBE_H

#include "pmessagebox.h"
#include "qualitycontrol.h"
#include "testing.h"
#include <QObject>
#include <unordered_map>

class ThreadReminderTsetTube : public QObject
{
    Q_OBJECT
public:
    explicit ThreadReminderTsetTube(QObject *parent = nullptr);
    ~ThreadReminderTsetTube();
public:

    void _usededtraytubeconfigstate(int index_); //脱机试管盘用完更新状态
private:
    void Indicates_wastetankisfull();
    void cleaningfluidbalanceisinsufficient();





    // 检查试杯区是否已用完
    bool checkIfTestTubeZoneAllUsed(const int index_tray);


    void showTestTubeZoneOneWarning(const int zoneNumber);

    PMessageBox* ensureMessageBoxInitialized(const int zoneNumber);

    void connectMessageBoxSignals(PMessageBox *pWidget);

    void positionMessageBoxCentered(PMessageBox *pWidget);

    void showMessageBox(PMessageBox *pWidget);




signals:
    void  reminderMainUi(quint8, QString ); //主界面右下角提示文字

public slots:
    void recvequipmentReminder(const int Warmingindex);

    void handleTrayUpdateConfirmed(int index);

    void handleTrayUpdateCanceled(int index);

private:
    PMessageBox *mpTestCupTrayITipsInfo = nullptr;          /*空试管盘提示信息*/



    std::unordered_map<int, PMessageBox*> messageBoxMap;
    PMessageBox *messageBox_= nullptr;
    PMessageBox *messageBox_II= nullptr;
    PMessageBox *messageBox_III= nullptr;
    PMessageBox *messageBox_IIII= nullptr;

};

#endif // THREADREMINDERTSETTUBE_H
