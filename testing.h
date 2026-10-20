#ifndef TESTING_H
#define TESTING_H

#include <QWidget>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QRadioButton>
#include <QVariantList>
#include <QMenu>
#include <QAction>
#include <QTreeWidget>
#include <QReadWriteLock>
#include <QMutex>
#include <QPointer>
#include <QMap>
#include <QList>
#include <QString>
#include <memory>
#include <array>
#include "json.h"
#include "analyticaljson.h"
#include "customcreatsql.h"
#include "replacethetesttubetray.h"
#include "usbcodedispose.h"
#include "progressbar.h"
#include "tubemappintpostable.h"
#include "machinetaskassignment.h"
#include "canceltaskconfigure.h"
#include "structinstance.h"

class QLabel;
class QTimer;
class Height_Data;

namespace TestingConstants {
    constexpr int BLOOD_TOP_TEXT_HEIGHT = 30;
    constexpr int BLOOD_ROW_SPACING = 2;
    constexpr int TRAY_SPACING = 3;
    constexpr int MIN_TUBE_DIAMETER = 25;
    constexpr int MAX_CHANNELS = 12;
    constexpr int TOTAL_REAGENTS = 10;
    constexpr int REAGENT_TOP_BOTTOM_SPACE = 30;
    constexpr int BIG_RADIUS_DEFAULT = 16;
    constexpr int SMALL_RADIUS_DEFAULT = 13;
    constexpr int TOP_DRAW_BIG_RADIUS = 36;
    constexpr int TOP_DRAW_SMALL_RADIUS = 32;
    constexpr quint8 INVALID_HOLE_VALUE = 255;
    constexpr quint8 BLOOD_HINT_PLACE = 0;
    constexpr quint8 BLOOD_HINT_ADDING = 1;
    constexpr quint8 BLOOD_HINT_TESTING = 2;
    constexpr quint8 BLOOD_HINT_DONE = 3;
    constexpr int TUBES_PER_TRAY = 60;
}

enum class CleanIndexNeedle {
    CleanDoubeNeedle = 0,
    CleanBloodNeedle,
    CleanReagentNeedle
};

struct ModuleVisibility {
    bool showModule1;
    bool showModule2;
    bool showModule3;
};

#define  AREA_REAGENT   0
#define  AREA_EMPTTUBE  1
#define  AREA_BLOOD     2

#define  TUBE_INIT      0
#define  TUBE_CHECKED   1
#define  TUBE_OUTRESULT 2

enum class EmptyTrayId : quint8 {
    Tray0 = 0,
    Tray1 = 1,
    Tray2 = 2,
    Tray3 = 3
};

constexpr int BLOOD_HOLE_TYPE_EVEN = 0;
constexpr int BLOOD_HOLE_TYPE_ODD = 1;

enum TestTrayHole { Tray_oneHole = 0, Tray_twoHole = 60, Tray_threeHole = 120, Tray_fourHole = 180, Tray_totalHole = 240 };

namespace Ui {
    class Testing;
}

class Testing : public QWidget
{
    Q_OBJECT
public:
    explicit Testing(QWidget *parent = nullptr);
    ~Testing() override;

    static UsbCodeDispose* getTaskDll();

    void initequipmentKind(quint8 indextype);
    void allSampleTested();
    void recv_NotifyChannleState(quint8 channelIndexNum, bool bUseing);
    void repPrpheight(const QString& idnum, bool flag);
    void showTestChannelInfo(const quint8& channelIndex, const QString& sampleName, const quint8& reagentIndex);
    void init_testtube_tray(int trayIndex);
    void TotalTaskProgress();
    void _showaddsamplewidget();
    void ensureAddSampleWindowVisible();
    void InitUIEmptyTubeused(int testTubeIndex, bool refreshUi = true);
    void EmptyTubeAssigned(quint8 IndexTube);
    void replacEmptyTestTary(int index_tary);
    void tubeMarked(const QString& outtext, const quint8& tubeIndex);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* e) override;
    void resizeEvent(QResizeEvent* event) override;

signals:
    void SynchronizeEmptyTube(quint8);
    void testingaddsample();
    void tsetfinishedbackorigin();
    void sendRepPrpheight(const QString& idnum, bool flag);

private slots:
    void toggleBlinkState();
    void doAllSampleTestedUIUpdate();

public slots:
    void recvBloodSuck2EmptyTube(bool banemia, quint8 IndexTube);
    void giveupSampleShowHole(const QList<quint8>& holeList);
    void giveupSampleChannelFlash(bool isChannelNormal, quint8 indexChannel);
    void sycnChangeuiTubeStatus(const QString& sample_name, quint8 anemiahole,
                                const QList<quint8>& marktube, int index_add, int all_add_task);
    void slotThrowtesttube();
    void HandleObtainPRPImage(const QString& pathImage);
    void HandleReopencvImageTubePRP(const QString& reId, const QString& pathImage);
    void DrawChannelProgress(quint8 index, double proportion);
    void initTheTaskInterface();
    void EmptyTubeClipMoved(quint8 IndexTube);
    void updateChannelProgressAndStatus(bool isWaitstate, quint8 index_Chn, const QString& status);
    void SampleTestingChangInitColor(QPoint maphole, quint8 indexChn);
    void updateBloodHoleHintStage(quint8 anemiaHole, quint8 richHole, quint8 stage);
    void onsycnDelTestSample(const QString delSampleId);

private:
    struct BloodAreaLayout {
        QList<int> columnStartX;
        int oneWidth;
        int totalCols;
        int perHoleHeight;
        int bigRadius;
        int smallRadius;
    };

    void recalculateChannelLayout();
    void recalculateTrayLayout();
    void setupModuleLayout();
    void markHiddenTrayTubesClippedAway();
    int visibleTrayCount() const;
    void ensureTrayReady(int trayIndex);

    void drawBloodHole(QPainter& painter, quint8 holeId, const QPoint& center,
                       int state, const QString& showText);
    void drawTrayTube(QPainter& painter, const QPointF& center,
                      quint8 holeNum, quint8 bigRadius, quint8 smallRadius);

    bool handlePaintEvents(QObject* watched, QEvent* event);
    bool handleResizeEvents(QObject* watched, QEvent* event);
    void clickAnaemiaHoleCancelTestTask(QList<int> waitTestSampleId);
    void initializeMachineUI(quint8 equipmentIndex);
    void initControlShowChannelProgress(quint8 startChannel, QWidget* progressChannel,
                                        QWidget* ptestChannel, const QPalette& pa);
    void ClickCanelTask(int posx, int posy);
    void VectorReplaceClear(quint8 Index);
    void ComplBackColorBloodArea();
    void VectorSelectedBloodAreaTube(quint8 IndexTube, QString Samplenum);
    void CanceltaskbackTubecolor(int PPPHole, int PRPHoles, QList<quint8> emptytubehole);
    void UpdateBloodHoleColors(QPainter& painter, int State, QMap<quint8, QPoint>& MapBloodHole);
    void CreatBloodZoneAxisPos();
    void DrawBloodTopText(QPainter& painter);
    void DrawBloodHoleInnerText(QPainter& painter, const QMap<quint8, QPoint>& BloodHoleMap);
    void DrawPutBloodReminders(QPainter& painter);
    void plotReagentWellsCoordinates(quint8 totalReagents);
    void showPaintReagents();
    void InitTubeAddBlooded(int tubeIndex, quint8 reagentType, int sampleNumber, bool refreshUi = true);
    void CreatTrayTestTubeUiAxis(int trayIndex);
    void DrawTrayTestTubeUiAxis(QWidget* pTrayWidget, quint8 IndexTray,
                                quint8 BigRadius, quint8 SmalleRadius);
    void showCleaningbit();
    void showDiscardTheCup();
    void showCleanreagent();
    BloodAreaLayout computeBloodAreaLayout() const;
    void drawTray(int trayIndex, QWidget* trayWidget);
    Height_Data *toggleAddSampleWindow();
    void updateBlinkWidgets();

    Ui::Testing* ui;
    mutable QReadWriteLock m_bloodMutex;
    mutable QMutex m_tubeMutex;
    mutable QMutex m_flashMutex;

    QMap<quint8, QPoint> m_BloodHoleInitUiAxis;
    QMap<quint8, QPoint> m_Blood_Tray_Checked;
    QMap<quint8, QPoint> m_Blood_Tray_OutResult;

    QMap<quint8, QPointF> m_Testcups;
    QMap<quint8, QString> mEmptyText;

    QMap<quint8, QPointF> mEmptyTubeAssigned;
    QMap<quint8, QPointF> mEmptyTubeAbsorb_Poorblood;
    QMap<quint8, QPointF> mEmptyTubeAbsorb_Richblood;
    QMap<quint8, QPointF> mEmptyTubeClipMoved;

    QList<quint8> mflashingTubeList;
    bool m_opendedheight = false;
    QMap<quint8, QPointF> m_CleanPos;
    QMap<quint8, QPoint>  m_Reagent_Tray;
    QPointF m_CleanningReagent;
    QPointF m_DiscardCupHole;

    std::array<ProgressBar*, TestingConstants::MAX_CHANNELS> m_channelShowsTheProgress{};
    std::array<QPointer<QLabel>, TestingConstants::MAX_CHANNELS> Channelreminder{};
    int m_channelCount = 0;

    QList<quint8> mreminderPPPandPRPhole;
    QMap<quint8, quint8> m_bloodHoleStage;

    int m_ProTotalTube;
    int m_ThrowTube;
    QMap<QString, quint8> m_TestingSample;

    QTimer* mtimer = nullptr;
    bool m_blinkState;

    quint8  minstrumentType;
    bool mInituiBloodArea;
    bool mInitEmptyArea[4];
    bool m_taskInterfaceInited = false;

    quint8 m_BigRadius;
    quint8 m_SmalleRadius;
    QMap<quint8, QString> m_BloodHoleNum;
    quint8 m_BloodBigRadius;
    quint8 m_BloodSmallRadius;
    bool alreadyinitchannelui;

    static std::unique_ptr<UsbCodeDispose> m_TaskDll;
};

#endif // TESTING_H
