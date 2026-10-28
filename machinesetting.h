#pragma execution_character_set("utf-8")
#ifndef MACHINESETTING_H
#define MACHINESETTING_H

#include <QWidget>
#include <QCloseEvent>
#include <QShowEvent>
#include <QHideEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QColor>
#include <QVariant>
#include <QStringList>
#include <QTreeWidgetItem>
#include <QTableWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QCheckBox>
#include <QButtonGroup>
#include <QAbstractButton>
#include <QShortcut>
#include <QTimer>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QMap>
#include <QElapsedTimer>
#include "QsLog/include/QsLog.h"
#include <QList>
#include <QVector>
#include <QJsonObject>
#include <QJsonDocument>
#include <memory>
#include <array>
#include <functional>

#include "json.h"
#include "delegate.h"
#include "analyticaljson.h"
#include "addusername.h"
#include "ini_file.h"
#include "cglobal.h"
#include "dilag/custompppvalue.h"
#include "opencvFindRBC/customtitlebar.h"
#include "replacethetesttubetray.h"
#include "channelvaluepanel.h"

// 与 tabWidgetSetconfigure 下标一一对应，避免宏撞号
enum MachineSettingTab {
    TabUserManagement = 0,
    TabHospitalInfo   = 1,
    TabChannelState   = 2,
    TabReference      = 3,
    TabChannelValue   = 4,
    TabQualityControl = 5,
    TabLis            = 6,
    TabMaintenance    = 7,
    TabPerformance    = 8
};

struct btnLable
{
    QLabel *plableinfo = nullptr;
    QSpinBox *pdownmovemm = nullptr;
    int index_ = 0;
};

namespace Ui {
class MachineSetting;
}

class MachineSetting : public QWidget
{
    Q_OBJECT

public:
    explicit MachineSetting(QWidget *parent = 0);
    ~MachineSetting();

    void sycnequipmentKind(quint8 indexEquipment);
    void _initpara();
    void InitMachineui();
    void init_MchineCommon();
    void initial_ReferenceValue();
    void init_TabHospitalInfo();
    void savePrintFormFromUi();
    void initializeHospitalDepartmentInformation();
    void InitTabTwoDate(int tabnum, QString str);
    void AnalyticalJsonTwo(QStringList);
    void SetColumnText(QTableWidget *tablewiget, int row, int col, QString text);

    void updatepara(const bool isTesting);
    void placeSettingsWindow();
    void disablechn(QVector<quint8> indexchnList);
    void completedPETest();
    void HandleoutputResultData(const QString &id, const quint8 &channel,
                                const std::array<double, 3> &ratios);
    void _finishmovetestdownhigh(int index_);
    void onGripperAirMotionFinished();
    void openKeyboard();
    void runInThread();

protected:
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *ev) override;
    void changeEvent(QEvent *event) override;

signals:
    void LoginEngineerMode(const bool);
    void temperatureChanged(int index, double value);
    void getchanneldata(const quint8, int);
    void sycnViewCurvePara(int curnum, bool bcheckavge);
    void pauseConnectModule(bool bpause);
    void WriteArryCommand(const QByteArray arry, QString kindCommad);
    void testdownheight(QByteArrayList, int);
    void controlallchn(bool bopened);
    void OpenChannelMotor(const int, const bool);
    void SetParatoInstrument(const QByteArrayList, QString);
    void Synchronizeupdates();
    void ModifyUsername(QString);
    void config_modul_temp(quint8 indexmodul, double modultemp);

public slots:
    void SetUserName(const QString user, const QString, bool vip);
    void SlotSetChannelValueUpdate(const int, const int);
    void sycnupdatesqltable(QTableWidgetItem *item);
    void slotsendReminder(QString reminderstr, const int indexActive);
    void slotmoduleSpeedData(quint8 indexModule, int chnIData, int chnIIData,
                             int chnIIIData, int chnIVData);
    void slotmoduleLedData(quint8 indexModule, int chnIData, int chnIIData,
                           int chnIIIData, int chnIVData);
    void handlebackHandssuck(bool issuck, int airval);

private slots:
    void CheckSelect(QTreeWidgetItem *, int);
    void operNotifyModuleTEMP(int moduleIndex, double value);
    void on_pushButton_ADD_clicked();
    void on_pushButton_DELETE_clicked();
    void on_pushButton_CHANGE_clicked();
    void on_toolButton_Delete_clicked();
    void on_toolButton_viewSQL_clicked();
    void on_comboBox_Category_currentIndexChanged(int index);
    void on_lineEdit_Name_textChanged(const QString);
    void on_lineEdit_Abbreviation_textChanged(const QString);
    void handleButtonClick(int row, int col);
    void handleButtonClicked(QAbstractButton *button);
    void onMaintenanceButtonClicked();
    void on_toolButton_Cancel_clicked();
    void on_lineEdit_Tab0_hospital_name_editingFinished();
    void show_Preence_sets();
    void on_toolButton_ADD_clicked();
    void on_toolButton_Import_clicked();
    void on_toolButton_export_clicked();
    void on_pushButton_backsetting_clicked();
    void on_pushButton_SelTextOutPath_clicked();
    void on_pushButton_Adjustcoordinates_clicked();
    void on_pushButtonsplitAirs_clicked();
    void on_pushButtonopenSuck_clicked();
    void on_pushButtonsavedimming_clicked();
    void onCloseRequested();
    void onMinimizeRequested();
    void onMaximizeRequested();

private:
    void initCustomTitleBar();
    void initSheet();
    void refreshInfoLibraryDeleteButton();
    void reloadHospitalInfoTables();
    void initChannelValueDisplayUi();
    void setupChannelDimmingMaintenanceTab();
    void ensureChannelDimmingTabVisible();
    void refreshChannelValuePageLayout();
    void initshowPPPinit();
    void intsignalsMable();
    void initSettinggeneralParameters();
    void initloginpassword();
    void connectEnterReturnShortcuts();
    void disconnectEnterReturnShortcuts();
    void Engineerinterfacelayout(bool enterEngineerMode);
    void engineerLogin(bool makesure);
    void openParameterDialog();

    void initDimmingTab(QTableWidget *pdimmingTable, const quint8 indexequipment);
    void insertDataItem(QTableWidget *pdimmingTable, int row, int col,
                        const QString &text, bool readOnly);
    void tableinsertBtn(QTableWidget *pdimmingTable, int row, int col,
                        const QString &customQss);
    void itemChangeValue(const int row, const int cols, const int val);
    void handleButtonAction(const int moduleIndex, int col);
    void configWriteDimming2Board();

    void ImportParameters(const quint8 equipmentIndex);
    void initdisplaymoduleChn(const quint8 equipmentIndex);
    void Instrument_configuration_channel_display(const quint8 equipmentIndex);
    void ChannelSettingStatus();
    void persistChannelStatesFromUi(bool ChnState[12]);
    int  channelIndexFromName(const QString &objectName) const;

    void sendmainbordBasicPara();
    void HandsParaWriteBoard();
    void BloodPinParaWriteBoadr();
    void syncBloodPinUiToIni();
    void loadBloodPinExtendedUiFromIni();
    void reloadMaintenanceParamsFromIni();
    QByteArray outParaOx1aWrite();
    QByteArray outParaOx1cWrite();

    void ReagentConfigParaSetOrGet(const bool bsetConfig);
    void configWrite2BoardReagentNeedlePara();
    void updateReagentCapacityAndLimit();
    void writeConsumablesToInstrument();
    void initBoardReagentNeedlePara();
    void notifyReagentPinParaToBoard();

    using SettingAction = std::function<void()>;
    void connectSetting(QWidget *widget, SettingAction action);
    void configBloodpinparaSignals();
    void _configReagPinparaSignals();
    void _configHandsinitChn(quint8 equipmentIndex_);
    void _initBloodpinpara();
    void _initReagPinpara();
    void innitHands(quint8 equipmentIndex_);
    void setupGripperVacuumSimpleUi();
    void beginGripperAirDetect();
    void cancelGripperAirDetect();
    void finishGripperAirDetect();
    void feedGripperAirSample(bool issuck, int airval);
    void completeGripperOriginEmptyStep(int emptyVal);
    void scheduleGripperOriginEmptyPollFallback();
    void requestGripperOriginEmptyReading();
    void sendGripperDumpSequence();
    void sendGripperDumpRetry();
    void sendGripperHomeAfterDump();
    void sendGripperOriginSequence();
    void sendGripperReturnOriginAfterDetect();
    void sendGripperChannelGrabSequence();
    void sendGripperDumpZRetreat();
    void sendGripperCancelHoming();
    void askGripperAirApply();
    void startGripperAirPoll(bool hold);
    void stopGripperAirPoll();
    void sendGripperVacuumProbe(bool suckOn);
    void updateGripperAirStepHint(int step);
    void queueGripperAirMotion(const QByteArrayList &cmds, int motionKind);
    void persistBloodPinIniFromUi();
    void persistReagentNeedleIniFromUi();
    void persistHandsIniFromUi();
    void _hidecontrol(quint8 index, QLabel *plabel, QSpinBox *pspinbox,
                      int _ks600, int _ks800);
    void _creatstupoint(QLabel *plable, QSpinBox *pgetdownmm, int index_,
                        QMap<QLabel *, btnLable *> &data_);
    void styleClickDownLabel(QLabel *label, bool busy = false);
    void installParameterHints();
    void relayoutEquipmentPages();
    void applyClinicalSettingsChrome();
    void frameNeedleConfigPages();
    void relayoutCurveModePage();
    void refreshSettingsPageTitle(QTreeWidgetItem *item);
    void _initconfigmodulefucn();

    void loadPerformanceEvaluation(QWidget *parentWidget);

    bool hasModifyPermission(const QString clickuser) const;
    void showPermissionDeniedWarning() const;
    void createModifyDialog(const QString &username);
    void centerDialog(QWidget *widget) const;
    void LabelTextColors(QLabel *ptextReminder, QColor color, QString reminderStr);

    template<typename T, typename ValueType>
    std::array<ValueType, 5> getUiValues(const std::array<T *, 5> &spinBoxes)
    {
        std::array<ValueType, 5> values;
        for (int i = 0; i < 5; ++i) {
            if (spinBoxes[i]) {
                values[i] = static_cast<ValueType>(spinBoxes[i]->value());
            } else {
                values[i] = ValueType{};
                QLOG_WARN() << "Invalid spinbox pointer at index:" << i;
            }
        }
        return values;
    }

private:
    Ui::MachineSetting *ui;
    CustomTitleBar *m_titleBar = nullptr;
    QButtonGroup *mResultMode = nullptr;
    std::shared_ptr<customPPPValue> m_pppValueWidget;
    std::unique_ptr<ReplaceTheTestTubeTray> m_Performanceverification;

    QList<QTreeWidgetItem *> m_rootList;
    QTreeWidgetItem *m_Modelitem = nullptr;
    QTreeWidgetItem *m_childItem_0_1 = nullptr;
    QTreeWidgetItem *m_childItem_0_2 = nullptr;
    QTreeWidgetItem *m_childItem_0_3 = nullptr;
    QTreeWidgetItem *m_maintenance = nullptr;
    QTreeWidgetItem *m_channelDimmingItem = nullptr;
    int m_channelDimmingConfigTabIndex = -1;
    QTreeWidgetItem *m_Modelitem_1 = nullptr;
    QTreeWidgetItem *m_childItem_1_2 = nullptr;
    QTreeWidgetItem *m_ReferenceValue = nullptr;
    QTreeWidgetItem *m_Modelitem_3 = nullptr;
    QTreeWidgetItem *m_qualitycontrol_2 = nullptr;
    QTreeWidgetItem *mperform = nullptr;

    int m_Info_Index = 0;
    QList<QTableWidget *> m_tableList;
    QStringList m_CategoryList;
    QJsonObject m_obj_Needle[4];
    QJsonObject m_obj_Debug_Needle[3];
    AnalyticalJson m_Analytical;
    QVariant m_AnalyticalOneData;
    QVariantList m_AnalyticallistData;

    QList<ChannelvaluePanel *> m_displayChannelwidget;
    QList<quint8> QualityChannel;
    bool mbopendallchannel = false;
    QVector<QDoubleSpinBox *> ksetTempSpinboxList;
    QList<QSpinBox *> m_capactityList;
    QList<QSpinBox *> m_limitratioList;
    bool m_initedDimmingTable = false;
    quint8 m_typedequipment = 0;
    QPropertyAnimation *m_propertyAnimation = nullptr;

    QMap<QLabel *, btnLable *> m_pbloodLableList;
    QMap<QLabel *, btnLable *> m_preagpinLableList;
    QMap<QLabel *, btnLable *> m_pHandsLableList;

    bool m_initAdjustcoordinates = false;
    QString m_settButtonQss;
    QString m_setToolButtonQss;
    QString m_setspinBoxQss;
    QString m_doubleSpinboxqss;

    QShortcut *menterShortcut = nullptr;
    QShortcut *m_keypadEnterShortcut = nullptr;
    bool shortcutsConnected = false;
    bool m_maintenanceButtonConnected = false;
    bool m_gripperAirUiReady = false;
    int m_liveHoldAir = -1;
    int m_liveReleaseAir = -1;
    int m_gripperAirDetectStep = 0;
    int m_gripperAirEmptyBaseline = -1;
    int m_gripperAirReleaseAfterDump = -1;
    int m_gripperAirLockedHold = -1;
    int m_gripperGrabSeenMs = -1;
    int m_gripperThrowSeenMs = -1;
    int m_gripperGrabDelayMeasured = -1;
    int m_gripperThrowDelayMeasured = -1;
    int m_gripperAirProbeHeight = 0;
    int m_gripperAirSettleMs = 600;
    int m_gripperAirDumpRetry = 0;
    QVector<int> m_gripperAirSamples;
    QElapsedTimer m_gripperAirPhaseClock;
    QTimer *m_gripperAirPollTimer = nullptr;
    QTimer *m_gripperAirOriginAckFallbackTimer = nullptr;
    bool m_gripperAirPollHold = false;
    bool m_gripperAirAwaitingMotion = false;
    int m_gripperAirMotionKind = 0;
    QLabel *m_gripperAirStepLabel = nullptr;
    QLabel *m_gripperThrowDelayLabel = nullptr;
    QSpinBox *m_gripperThrowDelaySpin = nullptr;
    QPushButton *m_gripperAutoApplyBtn = nullptr;
};

#endif // MACHINESETTING_H
