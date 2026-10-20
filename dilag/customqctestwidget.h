#ifndef CUSTOMQCTESTWIDGET_H

#define CUSTOMQCTESTWIDGET_H



#ifdef _MSC_VER

#pragma execution_character_set("utf-8")

#endif



#include "qcustomplot.h"



#include <QWidget>

#include <QVector>

#include <QPushButton>

#include <QTableWidget>

#include <QDoubleSpinBox>

#include <QSpinBox>

#include <QMap>

#include <QLabel>

#include <QDate>

#include <array>



class CustomQcTestWidget : public QWidget

{

    Q_OBJECT

public:

    explicit CustomQcTestWidget(quint8 equipmentKind, QWidget *parent = nullptr);



    void onTestSessionFinished();

    void applyResult(const QString &id, quint8 channel,

                     const std::array<double, 3> &measured,

                     const std::array<double, 3> &target,

                     const std::array<double, 3> &bias,

                     bool pass,

                     int waterChannelAd,

                     int standardChannelAd);



private slots:

    void onStartClicked();

    void onTargetChanged();

    void onPrevMonth();

    void onNextMonth();

    void onTodayMonth();



private:

    enum TestTableCol { ColId = 0, ColChannel, ColCount };



    enum ResultCol {

        ResTime = 0,

        ResId,

        ResChannel,

        ResWaterAd,

        ResStdAd,

        ResMeasH,

        ResMeasM,

        ResMeasL,

        ResVerdict

    };



    void buildUi();

    void createChannelButtons(int channelCount);

    void initTestTable();

    void initResultTable();

    void syncTestRow(const QString &channelText, bool add);

    QMap<QString, quint8> collectTestChannels() const;

    void setUiBusy(bool busy);

    void resetTestSelection();

    void loadSettings();

    void saveSettings() const;



    void setupTrendPlot();

    void refreshTrendChart();

    void drawTargetReferenceLines();

    std::array<double, 3> currentTargets() const;



    quint8 m_equipmentKind = 0;

    QDate m_chartMonth;

    QVector<QPushButton *> m_channelButtons;



    QDoubleSpinBox *m_spinTargetH = nullptr;

    QDoubleSpinBox *m_spinTargetM = nullptr;

    QDoubleSpinBox *m_spinTargetL = nullptr;

    QDoubleSpinBox *m_spinTolerance = nullptr;

    QSpinBox *m_spinHoleWater = nullptr;

    QSpinBox *m_spinHoleStandard = nullptr;

    QSpinBox *m_spinHoleHigh = nullptr;

    QSpinBox *m_spinHoleMid = nullptr;

    QSpinBox *m_spinHoleLow = nullptr;



    QTableWidget *m_tableTest = nullptr;

    QTableWidget *m_tableResult = nullptr;

    QPushButton *m_btnStart = nullptr;

    QLabel *m_lblMonth = nullptr;

    QLabel *m_lblRunStatus = nullptr;

    QLabel *m_lblLastCal = nullptr;



    QCustomPlot *m_trendPlot = nullptr;

};



#endif // CUSTOMQCTESTWIDGET_H

