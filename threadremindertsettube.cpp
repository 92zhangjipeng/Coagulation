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

void ThreadReminderTsetTube::_usededtraytubeconfigstate(int index_)
{
    quint16 hole_from = 0,hole_end = 0;
    hole_from = index_ * ONETRAY_TOTALTUBE;
    hole_end = (index_ + 1)*ONETRAY_TOTALTUBE;
    for(int k = hole_from ; k < hole_end ; k++){
        FullyAutomatedPlatelets::pinstancesqlData()->UpdateEmptyTube_State(k,TESTTUBES_CLIPPEDAWAY);//数据库
        FullyAutomatedPlatelets::pinstanceTesting()->InitUIEmptyTubeused(k);//更新界面试管盘内试管全部使用
    }
    FullyAutomatedPlatelets::pinstanceinstrument()->recv_updateTrayUsed(index_ + 1); //同步耗材界面
    return;
}

/*提示废液罐已满*/
void ThreadReminderTsetTube::Indicates_wastetankisfull()
{
    QString errortext = "仪器废液罐已满,请及时处理!";
    int lossS1 = static_cast<int>(equipmentTipInfo::LinqueScrapFull);
    FullyAutomatedPlatelets::pinstancesuppilereminder()->_configalarmindex(lossS1);
    FullyAutomatedPlatelets::pinstancesuppilereminder()->_configalarmText(errortext);
    emit reminderMainUi(PROMPTLOG,errortext);
    FullyAutomatedPlatelets::pinstancesuppilereminder()->move((QApplication::desktop()->width() -
                                                               FullyAutomatedPlatelets::pinstancesuppilereminder()->width()) / 2,
                                                              (QApplication::desktop()->height() -
                                                               FullyAutomatedPlatelets::pinstancesuppilereminder()->height()) / 2);
    FullyAutomatedPlatelets::pinstancesuppilereminder()->setWindowModality(Qt::NonModal);
    FullyAutomatedPlatelets::pinstancesuppilereminder()->update();
    FullyAutomatedPlatelets::pinstancesuppilereminder()->show();
    FullyAutomatedPlatelets::pinstancesuppilereminder()->raise();
    FullyAutomatedPlatelets::pinstancesuppilereminder()->activateWindow();
    return;
}

/*提示外部清洗液不足*/
void ThreadReminderTsetTube::cleaningfluidbalanceisinsufficient()
{
    QString errortext = "仪器外部清洗液余量不足,请及时处理!";
    int warmS2 = static_cast<int>(equipmentTipInfo::LinqueCleanShortage);
    FullyAutomatedPlatelets::pinstancesuppilereminder()->_configalarmindex(warmS2);
    FullyAutomatedPlatelets::pinstancesuppilereminder()->_configalarmText(errortext);
    emit reminderMainUi(PROMPTLOG,errortext); //提示文字
    FullyAutomatedPlatelets::pinstancesuppilereminder()->move(
                (QApplication::desktop()->width() - FullyAutomatedPlatelets::pinstancesuppilereminder()->width()) / 2,
                (QApplication::desktop()->height() - FullyAutomatedPlatelets::pinstancesuppilereminder()->height()) / 2);
    FullyAutomatedPlatelets::pinstancesuppilereminder()->setWindowModality(Qt::NonModal);
    FullyAutomatedPlatelets::pinstancesuppilereminder()->update();
    FullyAutomatedPlatelets::pinstancesuppilereminder()->show();
    return;
}



bool ThreadReminderTsetTube::checkIfTestTubeZoneAllUsed(const int index_tray)
{
    const int _starthole = (index_tray - 1) * ONETRAY_TOTALTUBE;
    const int _endhole = index_tray * ONETRAY_TOTALTUBE;

    QSet<quint8> _tubenumSet;
    for (int n = _starthole; n < _endhole; ++n)
         _tubenumSet.insert(n);

    QVariantList _tubenumInfo;
    FullyAutomatedPlatelets::pinstancesqlData()->FindAllEmptyTube(_tubenumInfo);

    for (const QVariant &signalTube : _tubenumInfo) {
            const AllTubeInfo tempinfo = signalTube.value<AllTubeInfo>();
            if (_tubenumSet.contains(tempinfo.TubeNumbers) && tempinfo.TubeStatus == TESTTUBES_FREETIME)
                return false;
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
    _usededtraytubeconfigstate(index);
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

//    QString outlog;
//    bool  testTubetrayisloose = false;
//    switch(Warmingindex)
//    {
//        case equipmentTipInfo::LinqueScrapFull:
//        {
//            Indicates_wastetankisfull();
//            outlog = "废液满";
//            break;
//        }

//        case equipmentTipInfo::LinqueCleanShortage:
//        {
//            cleaningfluidbalanceisinsufficient();
//            outlog = "清洗液不足";
//            break;
//        }

//        case equipmentTipInfo::TestTubeTrayI:
//        {
//            showTestTubeZoneOneWarning(TEST_THE_TUBEREEL_1);
//            testTubetrayisloose = true;
//            outlog = "试管盘1";
//            break;
//        }
//        case equipmentTipInfo::TestTubeTrayII:
//        {
//            showTestTubeZoneOneWarning(TEST_THE_TUBEREEL_2);
//            testTubetrayisloose = true;
//            outlog = "试管盘2";
//            break;
//        }
//        case equipmentTipInfo::TestTubeTrayIII:
//        {
//            showTestTubeZoneOneWarning(TEST_THE_TUBEREEL_3);
//            testTubetrayisloose = true;
//            outlog = "试管盘3";
//            break;
//        }
//        case equipmentTipInfo::TestTubeTrayIIII:
//        {
//            showTestTubeZoneOneWarning(TEST_THE_TUBEREEL_4);
//            testTubetrayisloose = true;
//            outlog = "试管盘4";
//            break;
//        }
//        default:    break;
//    }
//    if(testTubetrayisloose && true == cglobal::g_StartTesting)
//    {
//         outlog  = outlog + "测试中发生异位";
//         FullyAutomatedPlatelets::mainWindow()->_ReminderInfo("测试暂停",outlog);
//         QLOG_DEBUG()<<outlog<<endl;
//    }
//    FullyAutomatedPlatelets::mainWindow()->consumables_pause();
    return;
}
