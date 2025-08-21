#include "cglobal.h"
#include "loginui.h"
#include "threadremindertsettube.h"
#include "QsLog/include/QsLog.h"
#include <unordered_map>
#include <operclass/fullyautomatedplatelets.h>

ThreadReminderTsetTube::ThreadReminderTsetTube(QObject *parent) : QObject(parent)
{


}

ThreadReminderTsetTube::~ThreadReminderTsetTube()
{
    QLOG_DEBUG()<<"析构试管盘提示框"<<endl;
    //delete ui;
}

void ThreadReminderTsetTube::usededtraytubeconfigstate(int index)
{
    if (index < 0) {
        QLOG_WARN() << "Invalid tray index:" << index;
        return;
    }

    const quint16 startIndex = index * ONETRAY_TOTALTUBE;
    const quint16 endIndex = (index + 1) * ONETRAY_TOTALTUBE;

    // 获取单例实例
    auto* const sqlData = FullyAutomatedPlatelets::pinstancesqlData();
    auto* const testing = FullyAutomatedPlatelets::pinstanceTesting();
    auto* const instrument = FullyAutomatedPlatelets::pinstanceinstrument();

    // 批量更新试管状态
    for (quint16 k = startIndex; k < endIndex; ++k) {
        sqlData->UpdateEmptyTube_State(k, TESTTUBES_CLIPPEDAWAY);
        testing->InitUIEmptyTubeused(k);
    }

    // 更新耗材界面
    instrument->recv_updateTrayUsed(index + 1);
    return;
}



void ThreadReminderTsetTube::showEquipmentWarning(const QString& message, equipmentTipInfo tipType)
{
    const int code = static_cast<int>(tipType);

    auto reminder = FullyAutomatedPlatelets::pinstancesuppilereminder();
    if (Q_UNLIKELY(!reminder)) {
        QLOG_ERROR() << "Reminder instance is null";
        return;
    }

    reminder->_configalarmindex(code);
    reminder->_configalarmText(message);
    emit reminderMainUi(PROMPTLOG, message);

    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    const int x = (screenGeometry.width() - reminder->width()) / 2;
    const int y = (screenGeometry.height() - reminder->height()) / 2;

    reminder->move(x, y);
    reminder->setWindowModality(Qt::NonModal);
    reminder->show();
    reminder->raise();
    reminder->activateWindow();
}


/*提示废液罐已满*/
void ThreadReminderTsetTube::Indicates_wastetankisfull()
{
    showEquipmentWarning(tr("仪器废液罐已满,请及时处理!"),
                            equipmentTipInfo::LinqueScrapFull);
}

/*提示外部清洗液不足*/
void ThreadReminderTsetTube::cleaningfluidbalanceisinsufficient()
{
    showEquipmentWarning(tr("仪器外部清洗液余量不足,请及时处理!"),
                           equipmentTipInfo::LinqueCleanShortage);
    return;
}



bool ThreadReminderTsetTube::checkIfTestTubeZoneAllUsed(const int index_tray)
{
    if (index_tray <= 0) {
        QLOG_WARN() << "Invalid tray index:" << index_tray;
         return false;
    }

    const int startHole = (index_tray - 1) * ONETRAY_TOTALTUBE;
    const int endHole = index_tray * ONETRAY_TOTALTUBE;

    QVariantList emptyTubes;
    FullyAutomatedPlatelets::pinstancesqlData()->FindAllEmptyTube(emptyTubes);

    // 检查目标范围内的每个试管是否都被占用
    for (int tubeNumber = startHole; tubeNumber < endHole; ++tubeNumber) {
        bool foundFree = false;

        for (const QVariant &tubeVariant : emptyTubes) {
            const AllTubeInfo tubeInfo = tubeVariant.value<AllTubeInfo>();

            if (tubeInfo.TubeNumbers == tubeNumber &&
                tubeInfo.TubeStatus == TESTTUBES_FREETIME) {
                foundFree = true;
                break;
            }
        }

        // 如果找到一个空闲试管，立即返回false
        if (foundFree) {
            return false;
        }
    }
    return true;
}


void ThreadReminderTsetTube::showTestTubeZoneOneWarning( const int zoneNumber){

    const QString zoneName = tr("试杯区%1").arg(zoneNumber);
    const QString warningTemplate = tr("%1脱离试杯位");
    const QString fullWarning = warningTemplate.arg(zoneName);
    QLOG_INFO()<<fullWarning<<endl;
    // 检查试杯区是否已用完
    if (checkIfTestTubeZoneAllUsed(zoneNumber)) {
        emit reminderMainUi(PROMPTLOG, tr("%1使用完，脱位不提示").arg(zoneName));
        return;
    }

    // 假设 equipmentTipInfo 是一个枚举类型
    messageBoxMap = {
        { TEST_THE_TUBEREEL_1, messageBox_},
        { TEST_THE_TUBEREEL_2, messageBox_II},
        { TEST_THE_TUBEREEL_3, messageBox_III},
        { TEST_THE_TUBEREEL_4, messageBox_IIII}
    };

    // 创建或更新提示框
    PMessageBox* pWidgetTube = ensureMessageBoxInitialized(zoneNumber);

    // 设置消息框内容
	pWidgetTube->setindexTray(zoneNumber);
	pWidgetTube->setWindowTitle(zoneName);
   // messageBox_->setText(fullWarning);


    // 连接信号槽（确保只连接一次）
    connectMessageBoxSignals(pWidgetTube);

    // 显示消息框
    positionMessageBoxCentered(pWidgetTube);
    showMessageBox(pWidgetTube);
}

//辅助函数分解
PMessageBox *ThreadReminderTsetTube::ensureMessageBoxInitialized(const int zoneNumber)
{
    auto it = messageBoxMap.find(zoneNumber);

    if (it != messageBoxMap.end() || !it->second) {
            PMessageBox* pWidget = new PMessageBox(nullptr, zoneNumber);
            pWidget->setAttribute(Qt::WA_DeleteOnClose);
            connect(pWidget, &PMessageBox::destroyed, this, [this, zoneNumber]() {
                messageBoxMap.erase(zoneNumber);
            });
            messageBoxMap[zoneNumber] = pWidget;
    }
    return messageBoxMap[zoneNumber];  // 返回Map中的最新指针
}

void ThreadReminderTsetTube::connectMessageBoxSignals(PMessageBox *pWidget)
{
    //static bool connected = false;
    //if (!connected) {
        connect(pWidget, &PMessageBox::_makesureupdateTraytube,
                this, &ThreadReminderTsetTube::handleTrayUpdateConfirmed);
        connect(pWidget, &PMessageBox::_cancelupdateTraytube,
                this, &ThreadReminderTsetTube::handleTrayUpdateCanceled);
        //connected = true;
   // }
}

void ThreadReminderTsetTube::positionMessageBoxCentered(PMessageBox *pWidget)
{
    if (const auto screen = QGuiApplication::primaryScreen()) {
        const QRect screenGeometry = screen->geometry();
        pWidget->move(
            (screenGeometry.width() - pWidget->width()) / 2,
            (screenGeometry.height() - pWidget->height()) / 2
        );
    }
}

void ThreadReminderTsetTube::showMessageBox(PMessageBox *pWidget)
{
    pWidget->setWindowModality(Qt::ApplicationModal);
    pWidget->show();
    pWidget->raise();
    pWidget->activateWindow();
}

void ThreadReminderTsetTube::handleTrayUpdateConfirmed(int index)
{
    usededtraytubeconfigstate(index);
}

void ThreadReminderTsetTube::handleTrayUpdateCanceled(int index)
{
    QLOG_DEBUG() << "忽略试管盘" << index + 1 << "脱位提示";
    emit reminderMainUi(PROMPTLOG, tr("忽略试管盘%1脱位提示").arg(index + 1));
}


void ThreadReminderTsetTube::recvequipmentReminder(const int Warmingindex)
{
    QString outlog;
    bool testTubetrayisloose = false;
    //int trayNumber = 0;

    // 使用结构化绑定处理试管盘警告
    const auto handleTrayWarning = [&](int reelConstant, int number) {
        showTestTubeZoneOneWarning(reelConstant);
        testTubetrayisloose = true;
        outlog = QString("试管盘%1").arg(number);
    };

    switch(Warmingindex) {
            case equipmentTipInfo::LinqueScrapFull:
                Indicates_wastetankisfull();
                outlog = "废液满";
                break;

            case equipmentTipInfo::LinqueCleanShortage:
                cleaningfluidbalanceisinsufficient();
                outlog = "清洗液不足";
                break;

            case equipmentTipInfo::TestTubeTrayI:   handleTrayWarning(TEST_THE_TUBEREEL_1, 1); break;
            case equipmentTipInfo::TestTubeTrayII:  handleTrayWarning(TEST_THE_TUBEREEL_2, 2); break;
            case equipmentTipInfo::TestTubeTrayIII: handleTrayWarning(TEST_THE_TUBEREEL_3, 3); break;
            case equipmentTipInfo::TestTubeTrayIIII:handleTrayWarning(TEST_THE_TUBEREEL_4, 4); break;

            default: break;
        }

    // 优化条件判断逻辑
    if (Q_LIKELY(testTubetrayisloose && cglobal::g_StartTesting)) {
        FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder(
            "测试暂停",
            QString("%1测试中发生异位").arg(outlog)
        );
        QLOG_DEBUG() << outlog << "测试中发生异位";
    }

    // 使用RAII优化资源管理
    QMetaObject::invokeMethod(FullyAutomatedPlatelets::mainWindow(),
                            &MainWindow::updateContinueButtonState,
                            Qt::QueuedConnection);
    return;
}
