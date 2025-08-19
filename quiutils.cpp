#pragma execution_character_set("utf-8")
#include <qlocale.h>
#include <QPalette>
#include <QPixmap>
#include <QBrush>
#include <QWidget>
#include <QTime>
#include <QCoreApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QTextCodec>
#include <QRegExp>
#include <QProcess>
#include <QTranslator>
#include <stdio.h>
#include <stdlib.h>
#include <QElapsedTimer>
#include <QPixmap>
#include <QPainter>
#include <QTextStream>
#include <QPushButton>
#include <unordered_map>

#ifdef Q_OS_UNIX
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#else
#include <QBitArray>
#include <qt_windows.h>
#endif
#include "globaldata.h"
#include "loadequipmentpos.h"
#include "quiutils.h"


double QUIUtils::getNonRoundingNumber(double number, int digit)
{
    bool isNegative = false;
    int placeholderCount = 10;
    if(number < 0){
        number = - number;
        isNegative = true;
    }

    QStringList strNum = QString::number(number,'f',digit + placeholderCount).split(".");
    int iNum = strNum[0].toInt();
    QString dNumStr = strNum[1];
    dNumStr.chop(placeholderCount);

    int dNum = dNumStr.toInt();
    double res = iNum*1.0 + dNum/std::pow(10,digit);
    if(isNegative){
        res = -res;
    }
    return res;
}


void QUIUtils::CalculateTheConsumptionOfCleaningFluid(quint16 _usedcleanlinque, 
													double& LastRatio, quint16& updataRatio)
{
    ConsumablesOper* consumables = ConsumablesOper::GetpInstance();
    quint16 CleanCapacity = 0;//清洗液瓶/总量
    quint16 CleanLinqueAllowanceRatio = 0; //清洗液剩余比例
    quint16 CleanLinqueAllowTotal = 0; //清洗液剩余总量 瓶子里还有多少
    consumables->updateReagentTotal(READ_OPERRAT,INDEX_CLEANLINQUE_CONSUMABLE,CleanCapacity);
    if(CleanCapacity == 0)
        CleanCapacity = 60000;
    double one_ratio_value = static_cast<double>(static_cast<double>(CleanCapacity)/MAX_RATIO_SWITCT);
    consumables->getCleanLinqueAllowance(CleanLinqueAllowanceRatio);
	double lasteTotal = static_cast<double>(static_cast<double>(CleanLinqueAllowanceRatio)/ MAX_RATIO_SWITCT);
    CleanLinqueAllowTotal = static_cast<double>(lasteTotal * CleanCapacity);
    double midvalue = (static_cast<double>(CleanLinqueAllowTotal - _usedcleanlinque))/one_ratio_value;
    LastRatio = midvalue/MAX_RATIO_SWITCT; //剩余比显示 = （剩余总量-消耗)/比列/max512

    updataRatio = static_cast<quint16>(LastRatio * MAX_RATIO_SWITCT);
    return;
}


void QUIUtils::CalculateTheConsumptionOfReagent(quint8 reagentIndex,
                                                quint8 usedVolume,
                                                double &remainingRatio,
                                                quint16 &updatedRatio)
{
	constexpr quint16 DEFAULT_CAPACITY = 1000; // 显式定义默认容量

	// 获取试剂总量
	quint16 totalCapacity = 0;
	ConsumablesOper::GetpInstance()->updateReagentTotal(READ_OPERRAT, 
														reagentIndex, 
														totalCapacity);

	// 处理零容量情况（根据业务需求设置默认值）
	if (totalCapacity == 0) {
		totalCapacity = DEFAULT_CAPACITY;
		QLOG_WARN() << "检测到零容量试剂，使用默认值:" << DEFAULT_CAPACITY
			<< "，类型:" << reagentIndex<<endl;
	}

	// 获取当前剩余比例
	quint16 currentRatio = 0;
	ConsumablesOper::GetpInstance()->updateReplaceLocRatio(READ_OPERRAT, 
															reagentIndex, 
															currentRatio);

	// 计算剩余总量（避免重复类型转换）
	const double remainingTotal =
		(static_cast<double>(currentRatio) / MAX_RATIO_SWITCT) * totalCapacity;

	// 计算新的剩余量（合并计算公式）
    const double scaledCapacity =  static_cast<double>(totalCapacity) / MAX_RATIO_SWITCT;
    const double newRemaining = (remainingTotal - usedVolume) / scaledCapacity;

	// 计算结果处理
    remainingRatio = newRemaining / MAX_RATIO_SWITCT;  // 标准化到[0,1]范围
    updatedRatio = static_cast<quint16>(remainingRatio * MAX_RATIO_SWITCT);

	// 边界保护（防止负值）
    if (remainingRatio < 0.0) {
		QLOG_ERROR() << "试剂余量计算结果为负，已重置为0，类型:" << reagentIndex;
        remainingRatio = 0.0;
        updatedRatio = 0;
	}
	 //调试日志示例（需要时启用）
	QLOG_DEBUG() << QString("试剂计算[%1] 总量:%2ml 当前比例:%3/%4 计算结果:%5%")
	.arg(reagentIndex)
	.arg(totalCapacity)
	.arg(currentRatio)
	.arg(MAX_RATIO_SWITCT)
    .arg(remainingRatio * 100, 0, 'f', 2);
	
}

void QUIUtils::_ObtainSuppileDataInIFile(QString mpathfile,quint8 index_reag,quint8 &BottleNum,quint16& RatioReminder,
                                           quint8 &BottleLimit,quint16& BottleCapacity)
{
    QSettings *configInI = new QSettings(mpathfile, QSettings::IniFormat);
    switch(index_reag)
    {
        case  INDEX_CLEANLINQUE_CONSUMABLE:
        {
            BottleNum = configInI->value("/WholeSupplies/S1bottom").toUInt();
            RatioReminder = configInI->value("/CapacityRatio/S1ratio").toInt();
            BottleLimit =   configInI->value("/ReminderLimit/S1Limit").toUInt();
            BottleCapacity = configInI->value("/CapacityAll/S1Capacity").toInt();
            break;
        }
        case  INDEX_AA_CONSUMABLE:
        {
            BottleNum = configInI->value("/WholeSupplies/LAAbottom").toUInt();
            RatioReminder = configInI->value("/CapacityRatio/LAAratio").toInt();
            BottleLimit =   configInI->value("/ReminderLimit/LAALimit").toUInt();
            BottleCapacity = configInI->value("/CapacityAll/LAACapacity").toInt();
            break;
        }
        case  INDEX_AA_1_CONSUMABLE:
        {
            BottleNum =    configInI->value("/WholeSupplies/LAAbottom").toUInt();
            RatioReminder = configInI->value("/CapacityRatio/RAAratio").toInt();
            BottleLimit =   configInI->value("/ReminderLimit/RAALimit").toUInt();
            BottleCapacity = configInI->value("/CapacityAll/LAACapacity").toInt();
            break;
        }
        case  INDEX_ADP_CONSUMABLE:
        {
             BottleNum = configInI->value("/WholeSupplies/LADPbottom").toUInt();
             RatioReminder = configInI->value("/CapacityRatio/LADPratio").toInt();
             BottleLimit =   configInI->value("/ReminderLimit/LADPLimit").toUInt();
             BottleCapacity = configInI->value("/CapacityAll/LADPCapacity").toInt();
             break;
        }
        case  INDEX_ADP_1_CONSUMABLE:
        {
             BottleNum = configInI->value("/WholeSupplies/LADPbottom").toUInt();
             RatioReminder = configInI->value("/CapacityRatio/RADPratio").toInt();
             BottleLimit =   configInI->value("/ReminderLimit/RADPLimit").toUInt();
             BottleCapacity = configInI->value("/CapacityAll/LADPCapacity").toInt();
             break;
        }
        case  INDEX_EPI_CONSUMABLE:
        {
            BottleNum = configInI->value("/WholeSupplies/LEPIbottom").toUInt();
            RatioReminder = configInI->value("/CapacityRatio/LEPIratio").toInt();
            BottleLimit =   configInI->value("/ReminderLimit/LEPILimit").toUInt();
            BottleCapacity = configInI->value("/CapacityAll/LEPICapacity").toInt();
            break;
        }
        case  INDEX_EPI_1_CONSUMABLE:
        {
            BottleNum = configInI->value("/WholeSupplies/LEPIbottom").toUInt();
            RatioReminder = configInI->value("/CapacityRatio/REPIratio").toInt();
            BottleLimit =   configInI->value("/ReminderLimit/REPILimit").toUInt();
            BottleCapacity = configInI->value("/CapacityAll/LEPICapacity").toInt();
            break;
        }
        case  INDEX_COL_CONSUMABLE:
        {
            BottleNum = configInI->value("/WholeSupplies/LCOLbottom").toUInt();
            RatioReminder = configInI->value("/CapacityRatio/LCOLratio").toInt();
            BottleLimit =   configInI->value("/ReminderLimit/LCOLLimit").toUInt();
            BottleCapacity = configInI->value("/CapacityAll/LCOLCapacity").toInt();
            break;
        }
        case  INDEX_COL_1_CONSUMABLE:
        {
            BottleNum = configInI->value("/WholeSupplies/LCOLbottom").toUInt();
            RatioReminder = configInI->value("/CapacityRatio/RCOLratio").toInt();
            BottleLimit =   configInI->value("/ReminderLimit/RCOLLimit").toUInt();
            BottleCapacity = configInI->value("/CapacityAll/LCOLCapacity").toInt();
            break;
        }
        case  INDEX_RIS_CONSUMABLE:
        {
            BottleNum = configInI->value("/WholeSupplies/LRISbottom").toUInt();
            RatioReminder = configInI->value("/CapacityRatio/LRISratio").toInt();
            BottleLimit =   configInI->value("/ReminderLimit/LRISLimit").toUInt();
            BottleCapacity = configInI->value("/CapacityAll/LRISCapacity").toInt();
            break;
        }
        case  INDEX_RIS_1_CONSUMABLE:
        {
            BottleNum = configInI->value("/WholeSupplies/LRISbottom").toUInt();
            RatioReminder = configInI->value("/CapacityRatio/RRISratio").toInt();
            BottleLimit =   configInI->value("/ReminderLimit/RRISLimit").toUInt();
            BottleCapacity = configInI->value("/CapacityAll/LRISCapacity").toInt();
            break;
        }
        case OUTSIDE_CLEANLINQUE_S2:
        {
            BottleNum = configInI->value("/WholeSupplies/S2bottom").toUInt();
            RatioReminder = 0;
            BottleLimit =   0;
            BottleCapacity = 0;
            break;
        }
        case INDEX_TESTTUBE_CONSUMABLE:
        {
            BottleNum = configInI->value("/WholeSupplies/TestTaryNum").toUInt();
            RatioReminder = 0;
            BottleLimit =   0;
            BottleCapacity = 0;
            break;
        }
        default:break;
    }
    delete configInI;
}


void QUIUtils::sycnBottleRatioPara(quint8 index_reag, quint16 RatioReminder)
{
    // 参数有效性校验
   if ( RatioReminder > MAX_RATIO_SWITCT) {
       qWarning() << " Ratio:" << RatioReminder;
       return;
   }
    // 使用静态查找表维护键名映射
    static const QHash<quint8, QString> keyMap = {
        {INDEX_CLEANLINQUE_CONSUMABLE, "S1ratio"},
        {INDEX_AA_CONSUMABLE,          "LAAratio"},
        {INDEX_AA_1_CONSUMABLE,        "RAAratio"},
        {INDEX_ADP_CONSUMABLE,         "LADPratio"},
        {INDEX_ADP_1_CONSUMABLE,       "RADPratio"},
        {INDEX_EPI_CONSUMABLE,         "LEPIratio"},
        {INDEX_EPI_1_CONSUMABLE,       "REPIratio"},
        {INDEX_COL_CONSUMABLE,         "LCOLratio"},
        {INDEX_COL_1_CONSUMABLE,       "RCOLratio"},
        {INDEX_RIS_CONSUMABLE,         "LRISratio"},
        {INDEX_RIS_1_CONSUMABLE,       "RRISratio"}
    };

    // 通过哈希查找替代switch-case
    if (keyMap.contains(index_reag)) {
        const QString fullKey =  keyMap.value(index_reag);
        INI_File().wConfigPara(fullKey,RatioReminder);
        QLOG_DEBUG() << "更新试剂键:" << fullKey << "值(%):" << RatioReminder;
    } else {
        QLOG_ERROR() << "Unknown reagent index:" << index_reag;
    }
}

void QUIUtils::sycnBottleWholeNum(quint8 index_reag, quint8 BottleNum)
{
    static const std::unordered_map<quint8, QString> kIndexMapping = {
            {INDEX_CLEANLINQUE_CONSUMABLE,  "S1Numberbottles"},
            {INDEX_AA_CONSUMABLE, "AANumberbottles"},
            {INDEX_ADP_CONSUMABLE, "ADPNumberbottles"},
            {INDEX_EPI_CONSUMABLE, "EPINumberbottles"},
            {INDEX_COL_CONSUMABLE, "COLNumberbottles"},
            {INDEX_RIS_CONSUMABLE,"RISNumberbottles"},
            {OUTSIDE_CLEANLINQUE_S2,"S2Numberbottles"},
            {INDEX_TESTTUBE_CONSUMABLE,"TubeNumberplate"}
    };
    const QString targetIndex = [&]() {
        auto it = kIndexMapping.find(index_reag);
        return (it != kIndexMapping.end()) ? it->second : "S1Numberbottles";
    }();

    INI_File().wConfigPara(targetIndex,BottleNum);
}

void QUIUtils::_sycnBottleLimit(QString mpathfile, quint8 index_reag, quint8 BottleLimit)
{
    QSettings *configIniWrite = new QSettings(mpathfile, QSettings::IniFormat);
    switch(index_reag)
    {
        case  INDEX_CLEANLINQUE_CONSUMABLE:
        {
            configIniWrite->setValue("/ReminderLimit/S1Limit",BottleLimit);
            break;
        }
        case  INDEX_AA_CONSUMABLE:
        {
            configIniWrite->setValue("/ReminderLimit/LAALimit",BottleLimit);
            break;
        }
        case  INDEX_ADP_CONSUMABLE:
        {
            configIniWrite->setValue("/ReminderLimit/LADPLimit",BottleLimit);
            break;
        }
        case  INDEX_EPI_CONSUMABLE:
        {
            configIniWrite->setValue("/ReminderLimit/LEPILimit",BottleLimit);
            break;
        }
        case  INDEX_COL_CONSUMABLE:
        {
            configIniWrite->setValue("/ReminderLimit/LCOLLimit",BottleLimit);
            break;
        }
        case  INDEX_RIS_CONSUMABLE:
        {
            configIniWrite->setValue("/ReminderLimit/LRISLimit",BottleLimit);
            break;
        }

    }
    delete configIniWrite;
}

void QUIUtils::_sycnBottleCapacity(QString mpathfile, quint8 index_reag, quint16 BottleCapacity)
{
    QSettings *configIniWrite = new QSettings(mpathfile, QSettings::IniFormat);
    switch(index_reag)
    {
        case  INDEX_CLEANLINQUE_CONSUMABLE:
        {
            configIniWrite->setValue("/CapacityAll/Cleaningfluid",BottleCapacity);
            break;
        }
        case  INDEX_AA_CONSUMABLE:
        {
            configIniWrite->setValue("/CapacityAll/LAACapacity",BottleCapacity);
            break;
        }
        case  INDEX_ADP_CONSUMABLE:
        {
            configIniWrite->setValue("/CapacityAll/LADPCapacity",BottleCapacity);
            break;
        }
        case  INDEX_EPI_CONSUMABLE:
        {
            configIniWrite->setValue("/CapacityAll/LEPICapacity",BottleCapacity);
            break;
        }
        case  INDEX_COL_CONSUMABLE:
        {
            configIniWrite->setValue("/CapacityAll/LCOLCapacity",BottleCapacity);
            break;
        }
        case  INDEX_RIS_CONSUMABLE:
        {
            configIniWrite->setValue("/CapacityAll/LRISCapacity",BottleCapacity);
            break;
        }
    }
    delete configIniWrite;
}


QString QUIUtils::bitArray2String(QBitArray array)
{
	uint value = 0;
	uint total = array.size();
	for (uint i = 0; i < total; ++i)
	{
		value <<= 1;
		value += (int)array.at(i);
	}
	//qDebug() << value <<__FILE__<<__LINE__; //转化出的整形
	QString str;
    str.setNum(value, 16);
	return str;
}

int QUIUtils::deskWidth()
{
    static int width = 0;
    if (width == 0)
    {
        QDesktopWidget desktop;
        width = desktop.width();
    }

    return width;
}

int QUIUtils::deskHeight()
{
    static int height = 0;
    if (height == 0)
    {
        QDesktopWidget desktop;
        height = desktop.height();
    }

    return height;
}

bool QUIUtils::removeDir(const QString &dirName)
{
    if(dirName.isEmpty())
    {
        return true;
    }
    QDir dir(dirName);
    foreach(QFileInfo mfi, dir.entryInfoList())
    {
        if(!mfi.isFile())
        {
            continue;
        }
        QFile file(mfi.absoluteFilePath());
        file.remove();
        file.close();
    }
    dir.rmdir(dirName);

    return true;
}

bool QUIUtils::removeFile(const QString &dirName)
{
    QFile file(dirName);
    if(file.exists())
    {
        file.remove();
    }
    return true;
}

bool QUIUtils::isEnglish(QString str)
{
    if(str.isEmpty())
    {
        return false;
    }
    QByteArray ba = str.toLatin1();
    const char *pch = ba.data();
    bool bRet = true;
    while(*pch)
    {
        if((*pch>='A' && *pch<='Z') || (*pch>='a' && *pch<='z'))
        {
        }
        else
        {
            bRet = false;
            break;
        }
        pch++;
    }
    return bRet;
}

bool QUIUtils::isNumber(QString str)
{
    QByteArray ba = str.toLatin1();
    const char *s = ba.data();
    while(*s && *s>='0' && *s<='9')
    {
        s++;
    }
    if(*s)
    {
        return false;
    }
    return true;
}

void QUIUtils::msleep(const int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while (QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
        #ifdef Q_OS_UNIX
            struct timeval	timeout ;
            timeout.tv_sec = 0;
            timeout.tv_usec = 500;
            if(select(1, NULL, NULL, NULL, &timeout) == -1)
            {
                struct timespec tv;
                tv.tv_sec = 0;
                tv.tv_nsec = 500 * 1000;
                nanosleep(&tv, NULL);
            }
        #else
            Sleep(100);
        #endif
    }
    return;
}

QString QUIUtils::getTodatTime()
{
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString Todaytime = QString("%1/%2/%3").arg(current_date_time.toString("yyyy")).arg(current_date_time.toString("MM")).
		arg(current_date_time.toString("dd"));
	return  Todaytime;
}

void QUIUtils::deleteLocalLog()
{
    //日志存储目录
    QString logPath = getappload("suoweiLogFile");
    QDir dir(logPath);
    QStringList filters;//设置过滤器
    filters<<"*.txt";//过滤留下txt文件
    dir.setNameFilters(filters);

    QList<QString> dataList; //用来存放从log文件名获取到的日期

    QDateTime current_date_time = QDateTime::currentDateTime();//获取当前时间
    //QString dataStr = current_date_time.toString("yyyyMMdd");//时间格式转换

    foreach (QString file, dir.entryList(filters,QDir::Files)) {
        dataList += logPath + '/' +  file;
    }

    if(dataList.size() > 7)
    {
        foreach (QString filename, dataList) 
		{
            QFileInfo info(filename);

            QDateTime dt = info.lastModified();
            uint filedate = dt.toTime_t();
            uint todayData = current_date_time.toTime_t();
            uint ruler =  todayData - filedate;
            ruler /= (60 * 60 *24);
            if(ruler > 7)
            {
                QFile tempFile(info.filePath());
                tempFile.remove();

            }
        }
    }

}


QString  QUIUtils::getappload(QString fileflodr)
{
  return QString(QDir(QCoreApplication::applicationDirPath()).filePath(fileflodr));
}

bool QUIUtils::isDirExist(const  QString &fullPath)
{
    if (fullPath.isEmpty()) {
        QLOG_ERROR() << "传入路径为空" << __FUNCTION__ << __LINE__;
        return false;
    }

    QFileInfo dirInfo(fullPath);
    if (dirInfo.isDir()) {
        QLOG_DEBUG() << "文件夹[" << fullPath << "]已存在" << __FUNCTION__ << __LINE__;
        return true;
    } else if (dirInfo.exists()) {
        QLOG_ERROR() << "路径[" << fullPath << "]已存在，但不是一个文件夹" << __FUNCTION__ << __LINE__;
        return false;
    }
    QDir dir;
    if (!dir.mkpath(fullPath)) {
        #ifdef Q_OS_WIN
            DWORD errorCode = GetLastError();
            LPSTR messageBuffer = nullptr;
            FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                           nullptr, errorCode, 0, (LPSTR)&messageBuffer, 0, nullptr);
            QLOG_ERROR() << "Error:" << QString::fromLocal8Bit(messageBuffer);
            LocalFree(messageBuffer);
        #else
                QLOG_ERROR() << "Error:" << strerror(errno);
        #endif
        return false;
    }
    QLOG_DEBUG() << "创建文件夹成功: " << fullPath << __FUNCTION__ << __LINE__;
    return true;
}


void QUIUtils::delay_ms(int Time, bool IsThread)
{
    if(Time < 0) return;
    QElapsedTimer  tmr;
    tmr.start();
    while(true)
    {
        if(IsThread)
        {
            QCoreApplication::processEvents();
        }
        else
        {
            #ifdef Q_OS_UNIX
                msleep(1);
            #else
                Sleep(1);
            #endif
        }
        if (tmr.elapsed() >= Time)
        {
            break;
        }
    }
}

/**
* @brief label显示图标和文字
* @param 图标路径
* @param 提示文字
* @return
*/
void QUIUtils::QLabeldisplayIcon(QLabel* plabTip, QString pathIcon, QString SetTextTip,quint8 setTextColor)
{
    QPalette pe;
    QPixmap *pixmap = new QPixmap(pathIcon);
    pixmap->scaled(plabTip->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    plabTip->setScaledContents(true);
    plabTip->setPixmap(*pixmap);
    plabTip->setText(SetTextTip);
    pe.setColor(QPalette::WindowText,QColor(0 ,0 ,0));
    plabTip->setPalette(pe);
    plabTip->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    //QHBoxLayout* pHLayout = new QHBoxLayout();
   // pHLayout->addWidget(plabTip);
    delete pixmap;
    pixmap = nullptr;

//    if(setTextColor == 1 || setTextColor == 2)
//    {
//        for(QObject *chlid : plabTip->children())
//        {
//            delete chlid; //要改变图标状态
//            chlid = nullptr;
//        }
//    }
//    QPushButton *pbtnIcon = new QPushButton;
//    pbtnIcon->setParent(plabTip);
//    pbtnIcon->setIconSize(QSize(32,32));
//    pbtnIcon->setMinimumHeight(32);
//    pbtnIcon->setIcon(QIcon(pathIcon));
//    pbtnIcon->setStyleSheet("border:none");
//    pbtnIcon->setFlat(true);

//    QHBoxLayout* pLay = new QHBoxLayout(plabTip);

//    if(setTextColor != 0 && setTextColor != 5 && setTextColor != 6)
//        pLay->setContentsMargins(0,5,0,0);
//    else
//        pLay->setContentsMargins(0,0,0,0);

//    pLay->addSpacing(5);
//    pLay->addWidget(pbtnIcon);
//    pLay->addStretch(5);
//    plabTip->setLayout(pLay);
//    if(setTextColor != 0 && setTextColor != 6)
//        plabTip->setContentsMargins(0,0,10,5);
//    else
//        plabTip->setContentsMargins(0,0,0,0);
//    QPalette pe;
//    //plabTip->setStyleSheet("color:#000000;background-color:#fff9ef;border-width: 0px;border-style: solid;border-color: #FFFFFF;");
//    switch(setTextColor)
//    {
//        case 0:
//            plabTip->setText("     " + SetTextTip); //登录界面切换用户
//        break;
//        case 1:
//            pe.setColor(QPalette::WindowText,QColor(0 ,0 ,0));
//            plabTip->setPalette(pe);
//            plabTip->setText("    " + SetTextTip);
//        break;
//        case 2:
//            pe.setColor(QPalette::WindowText,QColor(0 ,0 ,0));
//            plabTip->setPalette(pe);
//            plabTip->setText("    " + SetTextTip); //XY坐标
//        break;
//        case 3:
//            plabTip->setText("     " + SetTextTip);
//        break;
//        case 4:
//            pe.setColor(QPalette::WindowText,QColor(200 ,50 ,50));
//            plabTip->setPalette(pe);
//            plabTip->setText("         " + SetTextTip);
//        break;
//        case 5: plabTip->setText("       " + SetTextTip); break;
//        case 6: plabTip->setText("      " + SetTextTip); break;
//    default:
//        break;
//    }
//    plabTip->show();
}


/**
 * @brief hexToQString
 * @param pHex
 * @param len
 * @return
 */
QString QUIUtils::hexToQString(unsigned char *pHex, int len)
{
    int index = 0;
    QString mstr = "";

    if(!pHex)
        return "";

    for(index = 0; index < len; index ++)
    {
        mstr = mstr + QString::number(pHex[index], 16) + " ";
    }
    return mstr;
}


QStringList  QUIUtils::QByteArrayToQStringList(QByteArray bamessage )
{
    QString back_data;
    QDataStream out(&bamessage, QIODevice::ReadOnly);
    while (!out.atEnd())
    {
       qint8 outchar = 0;
       out >> outchar;
       QString str = QString("%1").arg(outchar & 0xFF, 2, 16, QLatin1Char('0')).toUpper() + QString(" ");
       back_data += str;
    }
    back_data = back_data.simplified(); // 确保去除多余的空格
    QStringList outlist = back_data.split(" ");
    if (!outlist.isEmpty() && outlist.last().isEmpty())
       outlist.removeLast();
    return outlist;
}

//QStringList 转 QByteArray
QByteArray QUIUtils::StringListConversionByteArray (QStringList data)
{
    QByteArray byteArray;
    QDataStream out(&byteArray, QIODevice::WriteOnly);
    out << data;
    return byteArray;
}

//解析运动到的位置
int QUIUtils::ParseSportLocation(const QStringList recv_data ,const int Index)
{
    int Sport_Location = 0;
    const int Binary = 2; //二进制
    quint8 area_nums = recv_data.at(5).toUInt(nullptr, 16);
    QString hexMessage = QString("%1").arg(area_nums, 0, 10);
    QString areanumstr = QString("%1").arg(QString::number(hexMessage.toInt(), Binary), 8, QChar('0'));
    switch (Index)
    {
        case 0:
            Sport_Location = areanumstr.mid(4, Binary).toUInt(nullptr, Binary);  //运动到哪个区域
        break;
        case 1:
            Sport_Location = areanumstr.mid(6, Binary).toInt(nullptr, Binary); //运动的时候相对与哪根针
        break;
        case 2:
            Sport_Location = areanumstr.mid(0, 1).toInt(nullptr, Binary);     //血样针或者试剂针液面探测状态 0：未探测到
        break;
        case 3:
            Sport_Location = areanumstr.mid(0, 1).toInt(nullptr, Binary);     //抓手夹试管是否夹到试管 0：有夹到
        break;
    default:
        break;
    }
    return Sport_Location;
}


int QUIUtils::_backmotorexecutionstatus(int hex_data,int index_addr,int errorbit,QString &_printLog)
{
    QString  _printText;
    QString  hex = QString("%1").arg(hex_data, 0, 10);
    QString  hexstr = QString("%1").arg(QString::number(hex.toInt(), 2), 8, QChar('0'));
    int report_ = hexstr.mid(7,1).toInt(nullptr, 2);
    if(report_ == 1)
		_printText = QString::fromUtf8("报告");
    int witch_motor = hexstr.mid(0, 3).toInt(nullptr,2);
    if(index_addr == MAIN_CONTROL)
    {
        switch (witch_motor)
        {
            case 0:  _printText += QString::fromUtf8("X电机");                    break;
            case 1:  _printText += QString::fromUtf8("Y电机");                    break;
            case 2:  _printText += QString::fromUtf8("1000ul柱塞泵电机");         break;
            case 3:  _printText += QString::fromUtf8("100ul柱塞泵电机");          break;
            case 4:  _printText += QString::fromUtf8("XY电机");                   break;
            default: break;
        }
    }
    else if(index_addr == Z_AXIS)
    {
        switch (witch_motor)
        {
            case 0:  _printText += QString::fromUtf8("抓手");                  break;
            case 1:  _printText += QString::fromUtf8("试剂针");                break;
            case 2:  _printText += QString::fromUtf8("血样针");                break;
            default: break;
        }
    }
    //运动模式
    int speed_mode =  hexstr.mid(3,2).toInt(nullptr,2);
    switch(speed_mode)
    {
        case 0:  _printText += QString::fromUtf8("位置模式");          break;
        case 1:  _printText += QString::fromUtf8("正速度(液面探测)");  break;
        case 2:  _printText += QString::fromUtf8("负速度模式(复位)");  break;
        default:break;
    }
    //运动状态
   int sport_status = hexstr.mid(5,2).toInt(nullptr,2);
   switch (sport_status)
   {
        case MOTOR_WAITING:  _printText += QString::fromUtf8("等待");      break;
        case MOTOR_RUNNING:  _printText += QString::fromUtf8("运行中");    break;
        case MOTOR_FINISH:   _printText += QString::fromUtf8("执行完成");  break;
        case MOTOR_ERROR:
			if (index_addr == MAIN_CONTROL)
			{
				_printText += QString::fromUtf8("电机是异常[");
				switch (errorbit)
				{
                    case 1: _printText += QString::fromUtf8("Z轴通信超时]");                     break;
                    case 2: _printText += QString::fromUtf8("血样针或试剂针未复位,xy执行错误]");  break;
					case 3: _printText += QString::fromUtf8("x电机运动超时]");      break;
					case 4: _printText += QString::fromUtf8("y电机运动超时]");      break;
					case 5: _printText += QString::fromUtf8("xy电机运动超时]");     break;
					default: break;
				}
			}
			else if(index_addr == Z_AXIS)
                   _printText += QString::fromUtf8("从机掉线");
        break;
        default: break;
    }
    _printLog = _printText;
    return sport_status;
}

//解析到电机运动状态
int QUIUtils::Recve_Motor_motion_state(const QStringList recv_data , const int Index)
{
    int Motor_motion_state = recv_data.at(4).toUInt(nullptr, 16);
    const int Binary = 2; //二进制
    QString hexMessage = QString("%1").arg(Motor_motion_state, 0, 10);
    QString Statestr = QString("%1").arg(QString::number(hexMessage.toInt(), Binary), 8, QChar('0'));
    int leng = Statestr.length();
    switch (Index)
    {
        case THESTATEOFMOTOR:   Motor_motion_state = Statestr.mid(leng - 3, Binary).toUInt(nullptr, Binary);  //电机状态
        break;
        case MOTOR_SPORTMODE:   Motor_motion_state = Statestr.mid(leng - 5, Binary).toUInt(nullptr, Binary);  //运动模式
        break;
        case RUNNING_MOTOR:     Motor_motion_state = Statestr.mid(0, 3).toUInt(nullptr, Binary);  //运动的电机
        break;
        default:
            break;
    }
    return Motor_motion_state;
}

/*解析消息*/
int QUIUtils::ObtainCommandNumber(const QStringList Recv_data,const quint8 Index)
{
    int ObtainValue = 0;
    QString transformationStr;
    switch(Index)
    {
        case SLAVE_ADDRESS:
            transformationStr =  Recv_data.at(SLAVE_ADDRESS);
            ObtainValue = transformationStr.toInt(nullptr,16);
        break;
        case  COMMANDNUMBER:
            transformationStr =  Recv_data.at(COMMANDNUMBER);
            ObtainValue = transformationStr.toInt(nullptr,16);
    }
    return ObtainValue;
}

QString QUIUtils::equipmentdoAction(const quint8 indexAct)
{
    // 预定义试剂类型和动作映射表（可扩展）
    static const QHash<QString, QString> reagentMap = {
        {"AA",  "AA试剂"}, {"ADP", "ADP试剂"}, {"EPI", "EPI试剂"},
        {"COL", "COL试剂"}, {"RIS", "RIS试剂"}
    };

    QString reagent; // 存储试剂类型（如 "AA"）
    QString action;  // 存储动作类型（如 "吸试剂夹富血到放到测试通道"）

    switch (indexAct) {
            case FOCUS_ADDING_SAMPLE:       return QString::fromUtf8("加样到空试管");
            case FOCUS_CLIP_ANEMIA_TO_CHN:  return QString::fromUtf8("夹贫血到通道");
            case FOCUS_THORW_ANEMIA:        return QString::fromUtf8("丟通道内贫血");
            case FOCUS_CLEAN_BLOODPIN:      return QString::fromUtf8("清洗血样针");
            case FOCUS_CLEAN_DOUBLEPIN:     return QString::fromUtf8("清洗双针") ;

            // 统一处理试剂测试动作（FOCUS_TESTING_XX）
            case FOCUS_TESTING_AA:  reagent = "AA"; action = "吸%1夹富血到放到测试通道"; break;
            case FOCUS_TESTING_ADP: reagent = "ADP"; action = "吸%1夹富血到放到测试通道"; break;
            case FOCUS_TESTING_EPI: reagent = "EPI"; action = "吸%1夹富血到放到测试通道"; break;
            case FOCUS_TESTING_COL: reagent = "COL"; action = "吸%1夹富血到放到测试通道"; break;
            case FOCUS_TESTING_RIS: reagent = "RIS"; action = "吸%1夹富血到放到测试通道"; break;

            // 统一处理吐出试剂动作（FOCUS_SPIT_XX_TESTING）
            case FOCUS_SPIT_AA_TESTING:  reagent = "AA"; action = "吐出试剂%1到通道"; break;
            case FOCUS_SPIT_ADP_TESTING: reagent = "ADP"; action = "吐出试剂%1到通道"; break;
            case FOCUS_SPIT_EPI_TESTING: reagent = "EPI"; action = "吐出试剂%1到通道"; break;
            case FOCUS_SPIT_COL_TESTING: reagent = "COL"; action = "吐出试剂%1到通道"; break;
            case FOCUS_SPIT_RIS_TESTING: reagent = "RIS"; action = "吐出试剂%1到通道"; break;

            // 统一处理清洗试剂针（FOCUS_CLEAN_EREAGENTPIN_XX）
            case FOCUS_CLEAN_EREAGENTPIN_AA:  reagent = "AA"; action = "洗试剂%1针"; break;
            case FOCUS_CLEAN_EREAGENTPIN_ADP: reagent = "ADP"; action = "洗试剂%1针"; break;
            case FOCUS_CLEAN_EREAGENTPIN_EPI: reagent = "EPI"; action = "洗试剂%1针"; break;
            case FOCUS_CLEAN_EREAGENTPIN_COL: reagent = "COL"; action = "洗试剂%1针"; break;
            case FOCUS_CLEAN_EREAGENTPIN_RIS: reagent = "RIS"; action = "洗试剂%1针"; break;

            // 统一处理丢弃试管杯（FOCUS_THROW_XX_TUBE）
            case FOCUS_THROW_AA_TUBE:  reagent = "AA"; action = "丟测试%1试管杯"; break;
            case FOCUS_THROW_ADP_TUBE: reagent = "ADP"; action = "丟测试%1试管杯"; break;
            case FOCUS_THROW_EPI_TUBE: reagent = "EPI"; action = "丟测试%1试管杯"; break;
            case FOCUS_THROW_COL_TUBE: reagent = "COL"; action = "丟测试%1试管杯"; break;
            case FOCUS_THROW_RIS_TUBE: reagent = "RIS"; action = "丟测试%1试管杯"; break;

            default:
                QLOG_WARN() << "未知动作索引:" << indexAct;
                return QString::fromUtf8("未知操作");
    }
    // 动态生成标准化描述
    if (reagentMap.contains(reagent)) {
        return QString::fromUtf8(action.arg(reagentMap[reagent]).toStdString().c_str());
    } else {
        QLOG_ERROR() << "未定义的试剂类型:" << reagent;
        return QString::fromUtf8("操作配置错误");
    }
}

void OutputinfoZAxis(){

}

QString QUIUtils::outputstheZ_axis_status(const QStringList data_)
{
     QString outstr;
     bool b_print = false;


	 quint8  index_ = QString(data_.at(4)).toUInt(nullptr, 16); 
	 quint8  index_five = QString(data_.at(5)).toUInt(nullptr, 16);
	 //第4个字节
	 QString  hex = QString("%1").arg(index_, 0, 10);
	 QString  hexstr = QString("%1").arg(QString::number(hex.toInt(), 2), 8, QChar('0'));

	 int _ztype, _zstate, _zmode, _zindex;

	 //第5个字节
	 QString  hex_five = QString("%1").arg(index_five, 0, 10);
	 QString  hexstr_five = QString("%1").arg(QString::number(hex_five.toInt(), 2), 8, QChar('0'));

	 int  z_stop_r, z_stop_l, z_downmode;
	 
     _zindex = hexstr.mid(0, 3).toInt(nullptr,2);

     _zmode = hexstr.mid(3,2).toInt(nullptr,2);

     _zstate = hexstr.mid(5,2).toInt(nullptr,2);

     _ztype = hexstr.mid(7,1).toInt(nullptr,2);

     if(_ztype == 1)
     {
        outstr = QString::fromUtf8("报告");
     }
     switch(_zindex)
     {
         case 0:     outstr += QString::fromUtf8("抓手Hands");      break;
         case 1:     outstr += QString::fromUtf8("试剂针ReagentPin");    break;
         case 2:     outstr += QString::fromUtf8("血样针BloodSamplePin");    break;
     }
     switch(_zmode)
     {
         case 0:  outstr += QString::fromUtf8("位置模式"); break;
         case 1:  outstr += QString::fromUtf8("正速度(液面探测)"); break;
         case 2:
            outstr += QString::fromUtf8("负速度复位");
            b_print = true;
         break;
         case 3:  outstr += QString::fromUtf8("保持模式"); break;
     }
     switch(_zstate)
     {
         case 0: outstr += QString::fromUtf8("等待,(");               break;
         case 1: outstr += QString::fromUtf8("运行中,(");             break;
         case 2: outstr += QString::fromUtf8("完成,(");           break;
         case 3: outstr += QString::fromUtf8("命令错误从机掉线,(");   break;
     }

     z_downmode = hexstr_five.mid(2,1).toInt(nullptr,2);
     //QLOG_DEBUG()<<"z_downmode"<<hexstr.mid(2,1)<<"转2进制="<< z_downmode;

     z_stop_l = hexstr_five.mid(1,1).toInt(nullptr,2);
     //QLOG_DEBUG()<<"z_stop_l"<<hexstr.mid(1,1)<<"转2进制="<< z_stop_l;

     z_stop_r = hexstr_five.mid(0,1).toInt(nullptr,2);
     //QLOG_DEBUG()<<"z_stop_r"<<hexstr.mid(0,1)<<"转2进制="<< z_stop_r;
     if(z_stop_l == 0)
     {
         outstr += QString::fromUtf8("脱离光耦)");
     }
     else if(z_stop_l == 1)
     {
         outstr += QString::fromUtf8("在光耦)");
     }

     if(b_print && z_stop_l == 0){
        QLOG_DEBUG()<<"收到Z复位命令"<<data_;
        QLOG_DEBUG()<<"第4字节="<<data_.at(4)<<"==>十进制=>"<<index_<<"toHex=>"<<hexstr;
        QLOG_DEBUG()<<"第5字节="<<data_.at(5)<<"==>十进制=>"<<hexstr_five<<"toHex=>"<<index_five;
        QLOG_DEBUG()<<outstr<<__FUNCTION__<<endl;
     }
     return  outstr;
}


/*获得XY运动的坐标*/
QPoint QUIUtils::coordinatePositionXYMotion(const QStringList recv_data)
{
    if(recv_data.size() != 15) return QPoint(0,0);
    QString Xposition = QString("%1%2%3%4")
        .arg(recv_data.at(9), 0, 16)  // 第三个参数16表示十六进制
        .arg(recv_data.at(8), 0, 16)
        .arg(recv_data.at(7), 0, 16)
        .arg(recv_data.at(6), 0, 16);

    QString Yposition = QString("%1%2%3%4")
        .arg(recv_data.at(13), 0, 16)  // 第三个参数16表示十六进制
        .arg(recv_data.at(12), 0, 16)
        .arg(recv_data.at(11), 0, 16)
        .arg(recv_data.at(10), 0, 16);
    bool ok;
    int x = Xposition.toInt(&ok, 16);
    if (!ok) {
     QLOG_ERROR()<<"x坐标转换失败"<<endl;
    }
    int y = Yposition.toInt(&ok, 16);
    if (!ok) {
     QLOG_ERROR()<<"y坐标转换失败"<<endl;
    }
    return QPoint(x,y);
}



void  QUIUtils::parseDataratio(QString &str)
{
    char array[]={'%'}; //需要去除的字符
    int length = sizeof (array)/sizeof (char);
    for(int i = 0; i < length; i++)
    {
        QString tmp = QString(array[i]);
        if(str.contains(tmp))
        {
            str = str.replace(tmp,"");
        }
    }
}

void QUIUtils::itemReferenceTestValue(QStringList &keyListItem)
{
    int indexReagent = 0;
    int numtime = 0;
    QString Reagentname = "";
    QString outTestReferenceValue = "";
    for(int i = 0 ; i < 20; i++)
    {
        indexReagent = i/4 + 1;
        numtime = i%4;
        switch(indexReagent)
        {
            case AA_REAGENT:  Reagentname =  QString("AA聚集率-");   break;
            case ADP_REAGENT: Reagentname =  QString("ADP聚集率-");  break;
            case EPI_REAGENT: Reagentname =  QString("EPI聚集率-");  break;
            case COL_REAGENT: Reagentname =  QString("COL聚集率-");  break;
            case RIS_REAGENT: Reagentname =  QString("RIS聚集率-");  break;
        default:break;
        }
        switch(numtime)
        {
            case 0: outTestReferenceValue = QString("%1%2").arg(Reagentname).arg(1); break;
            case 1: outTestReferenceValue = QString("%1%2").arg(Reagentname).arg(2); break;
            case 2: outTestReferenceValue = QString("%1%2").arg(Reagentname).arg(3); break;
            case 3: outTestReferenceValue = QString("%1%2").arg(Reagentname).arg("Max"); break;
        default:break;
        }
        keyListItem.push_back(outTestReferenceValue);
    }
}

/*电机异常输出提示*/
QString QUIUtils::ReminderMotor(const int Addrs,const int Motors)
{
    QString OutText = "";
    switch(Addrs)
    {
        case MAIN_CONTROL:
            switch(Motors)
            {
                case 0: OutText = QString("X电机"); break;
                case 1: OutText = QString("Y电机");  break;
                case 2: OutText = QString("1000ul柱塞泵");  break;
                case 3: OutText = QString("100ul柱塞泵");  break;
                case 4:OutText = QString("XY电机"); break;
                default: break;
            }
        break;
        case Z_AXIS:
            switch(Motors)
            {
                case 0: OutText = QString("抓手");     break;
                case 1: OutText = QString("试剂针");  break;
                case 2: OutText = QString("血样针");  break;
                default: break;
            }
        break;
        default:break;
    }
    return OutText;
}

/*电机异常映射*/
QString QUIUtils::MotorStateMapStr(const quint8 IndexStatus,const int ErrorEnum)
{
    QString CompleteStatus = "";
    switch(IndexStatus)
    {
        case MOTOR_WAITING: CompleteStatus = "等待";          break;
        case MOTOR_RUNNING: CompleteStatus = "运行中";        break;
        case MOTOR_FINISH:  CompleteStatus = "完成";          break;
        case MOTOR_ERROR:   CompleteStatus = "执行异常";      break;
        default:break;
    }
    switch(ErrorEnum)
    {
        case 1: CompleteStatus = QString("%1:%2").arg(CompleteStatus).arg("Z轴通信超时");        break;
        case 2: CompleteStatus = QString("%1:%2").arg(CompleteStatus).arg("血样针或试剂针未复位,xy执行错误");         break;
        case 3: CompleteStatus = QString("%1:%2").arg(CompleteStatus).arg("x电机运动超时");      break;
        case 4: CompleteStatus = QString("%1:%2").arg(CompleteStatus).arg("y电机运动超时");      break;
        case 5: CompleteStatus = QString("%1:%2").arg(CompleteStatus).arg("xy电机运动超时");     break;
    default: break;
    }
    return CompleteStatus;
}



/*
* 找字符串内数字
*/
int QUIUtils::StringFindintnum(QString Str)
{
    int  nTime;
    QRegExp rx("(\\d+)");  // 匹配数字
    int pos = 0;
    while ((pos = rx.indexIn(Str, pos)) != -1)
    {
           nTime= rx.cap(0).toInt();
           pos += rx.matchedLength();
    }
    return nTime;
}

void QUIUtils::reminderfullcoder(quint8 indexaddr_,QString& fullCode_)
{
	static const QHash<quint8, QString> fullCodeMap = {
		{ MODULE_1, QString::fromUtf8("模组1缓存已满!") },
		{ MODULE_2, QString::fromUtf8("模组2缓存已满!") },
		{ MODULE_3, QString::fromUtf8("模组3缓存已满!") },
		{ Z_AXIS, QString::fromUtf8("Z轴电机缓存已满!") },
		{ MAIN_CONTROL, QString::fromUtf8("主板控制缓存已满!") }
	};

	fullCode_ = fullCodeMap.value(indexaddr_, "未知模块缓存已满!");
}




QString QUIUtils::index_reagent_mapping_reagentName(QString outText,const quint8 indexReag)
{
    QString outprint = "";
    switch(indexReag)
    {
        case AA_REAGENT:  outprint = outText +"AA"; break;
        case ADP_REAGENT: outprint = outText +"ADP"; break;
        case EPI_REAGENT: outprint = outText +"EPI"; break;
        case COL_REAGENT: outprint = outText +"COL"; break;
        case RIS_REAGENT: outprint = outText +"RIS"; break;
        case ANEMIA:  outprint = outText +"贫血"; break;
    default:break;
    }
    return outprint;
}


/*
* 样本下标映射样本数据库键
*/
QString QUIUtils::IndexPorgectMapString(quint8 IndexReagents ,bool Result)
{
    QString UPdataKey;
    if (false == Result)
    {
        switch (IndexReagents)
        {
            case AA_REAGENT:  UPdataKey = "AA数据"; break;
            case ADP_REAGENT: UPdataKey = "ADP数据"; break;
            case EPI_REAGENT: UPdataKey = "EPI数据"; break;
            case COL_REAGENT: UPdataKey = "COL数据"; break;
            case RIS_REAGENT: UPdataKey = "RIS数据"; break;
            default:
                UPdataKey = "AA数据";
            break;
        }
    }
    else
    {
        switch (IndexReagents)
        {
            case AA_REAGENT:  UPdataKey = "AA";  break;
            case ADP_REAGENT: UPdataKey = "ADP"; break;
            case EPI_REAGENT: UPdataKey = "EPI"; break;
            case COL_REAGENT: UPdataKey = "COL"; break;
            case RIS_REAGENT: UPdataKey = "RIS"; break;
            default:
                UPdataKey = "AA";
            break;
        }
    }
    return UPdataKey;
}


/* 输出获取测试结果*/
QString stage(double v, int precision) //precision为需要保留的精度，2位小数变为2
{
    QString _out = QString::number(v, 'f', precision);
    return _out;
}

QString QUIUtils::OutPrintTestedResult(QVector<double> TestingDataList)
{
    QString _outputtestresult = "0%,0%,0%,0%";
    int index_sixty,index_onehunderdeighty;
    QString index_max_val,index_sixty_val,index_onehunderdeighty_val,index_end_val;
    if(TestingDataList.size() == 300){
        auto itmax = std::max_element(std::begin(TestingDataList), std::end(TestingDataList));
        double max_val = *itmax;
        index_max_val = stage(max_val*100,2) + "%";

        index_sixty = TestingDataList.size()/5 - 1;
        index_onehunderdeighty = TestingDataList.size()/2 + 29;

        double end_val= *(std::end(TestingDataList) - 1 );
        index_end_val =  stage(end_val*100, 2)+ "%";

        index_sixty_val =  stage(TestingDataList.at(index_sixty)*100, 2) + "%";

        index_onehunderdeighty_val = stage(TestingDataList.at(index_onehunderdeighty)*100,2)+ "%";
        _outputtestresult = QString("%1,%2,%3,%4").arg(index_sixty_val).arg(index_onehunderdeighty_val).arg(index_end_val).arg(index_max_val);

    }else{
        QLOG_ERROR()<<"采集数据个数异常"<<endl;
        return _outputtestresult;
    }
    QLOG_DEBUG()<<"测试结果值:"<<_outputtestresult;
    return _outputtestresult;
}






//读取仪器坐标命令
QByteArray QUIUtils::ReadcoordinateArry(quint8 cmd_num)
{
    QByteArray sendData = QByteArray::fromHex(QString::number(MAIN_CONTROL).toUtf8());
    sendData.push_back(0x15); //功能码
    sendData.push_back(QByteArray::fromHex(QString::number(cmd_num,HEX_SWITCH).toUtf8())); //命令编号
    sendData.push_back(QByteArray::fromHex(QString::number(0).toUtf8())); //缓存容量
    //读写状态
    QBitArray  bitycode(8);
    bitycode.fill(false);
    bitycode.setBit(7,true); //读数据
    QString sSendHex = bitArray2String(bitycode);
    QByteArray byte_ = QByteArray::fromHex(sSendHex.toLatin1());
    sendData.push_back(byte_);
    for(int i = 0; i< 10; i++)
    {
        QString otherdata = QString("%1").arg(QString::number(0, HEX_SWITCH), 2, '0');
        QByteArray datatmp =  QByteArray::fromHex(otherdata.toLatin1());
        sendData.push_back(datatmp);
    }
    return sendData;
}




//写数据
//uint转QbyteArray isEndianTyped: true 大端 false 小端
QByteArray QUIUtils::qint16ToQByteArray(quint16 input, bool isEndianTyped)
{
	QByteArray output;
	if (isEndianTyped)
	{
		output[3] = (uchar)(0x000000ff & input);
		output[2] = (uchar)((0x0000ff00 & input) >> 8);
	}
	else
	{
		output[0] = (uchar)(0x000000ff & input);
		output[1] = (uchar)((0x0000ff00 & input) >> 8);
	}
	return output;
}

void QUIUtils::WriteEquipmentType(quint8 equipmentKind, quint8 index_num,QByteArray& configorder)
{
    configorder.clear();
    configorder = QByteArray::fromHex(QString::number(MAIN_CONTROL).toUtf8());

    configorder.push_back(0x15); //功能码

    QString Hexnum = QString::number(index_num, HEX_SWITCH);
    configorder.push_back(QByteArray::fromHex(Hexnum.toUtf8())); //命令编号

    configorder.push_back(QByteArray::fromHex(Hexnum.toUtf8())); //缓存容量

	//写状态(缓存 == false : true ==写)
    QBitArray  bitycode(8);
    bitycode.fill(false);
    bitycode[4] = true;
	QString sSendHex = bitArray2String(bitycode);
	QByteArray byte_ = QByteArray::fromHex(sSendHex.toLatin1());
    configorder.push_back(byte_);


    QString ParaState = QString("%1").arg(QString::number(0, 16), 2, '0');
    QByteArray ParaState_arry = QByteArray::fromHex(ParaState.toLatin1());
    configorder.push_back(ParaState_arry);

    configorder.push_back(QByteArray::fromHex(QString::number(equipmentKind,HEX_SWITCH).toUtf8()));

    //未使用
    configorder.push_back(ParaState_arry);

    configorder.push_back(ParaState_arry);

    //保存时间
    QDateTime dateTime = QDateTime::currentDateTime();//获取系统当前的时间
    QString strdata = dateTime.toString("yyyy-MM-dd");
    QStringList setdate = strdata.split("-");
    QByteArray EndianArry;
    //年
    quint16 year = QString(setdate.at(0)).toShort(nullptr, 10);
    EndianArry = qint16ToQByteArray(year, false);
    configorder.push_back(EndianArry);
    //月
    quint16 monthunshort = QString(setdate.at(1)).toShort(nullptr, 10);
    EndianArry = qint16ToQByteArray(monthunshort, false);
    configorder.push_back(EndianArry);
    //日
    quint16 dayshort = QString(setdate.at(2)).toShort(nullptr, 10);
    EndianArry = qint16ToQByteArray(dayshort, false);
    configorder.push_back(EndianArry);

    QLOG_DEBUG()<<"写入仪器型号和保存时间命令"<<configorder.toHex(' ').trimmed().toUpper()<<endl;
    return;

}




//缓存命令
QByteArray QUIUtils::bufferCodeAxis(quint8 indexNum, int stateoper, QMap<quint8,quint16> confAxispos)
{
    QByteArray sendData = QByteArray::fromHex(QString::number(MAIN_CONTROL).toUtf8());
    sendData.push_back(0x15); //功能码

    QString strHex = QString::number(indexNum, HEX_SWITCH);
    sendData.push_back(QByteArray::fromHex(strHex.toUtf8())); //命令编号

    sendData.push_back(QByteArray::fromHex(QString::number(0).toUtf8())); //缓存容量

    //写状态(缓存false : true 写)
    QBitArray  bitycode(8);
    bitycode.fill(false);
    if(stateoper == AXISPOS_WRITE)
       bitycode.setBit(4, true);
    QString sSendHex = bitArray2String(bitycode);
    QByteArray byte_ = QByteArray::fromHex(sSendHex.toLatin1());
    sendData.push_back(byte_);

    auto iter = confAxispos.constBegin();
    while(iter != confAxispos.constEnd())
    {
        quint16 data_ = iter.value();
        QByteArray EndianArry = qint16ToQByteArray(data_, false);
        sendData.push_back(EndianArry);
        iter++;
    }
    return sendData;
}

//写试剂限位
void QUIUtils::_writeReagentLimitOrder(QByteArray& buffer)
{
    buffer.clear();
    buffer = QByteArray::fromHex(QString::number(MAIN_CONTROL,HEX_SWITCH).toUtf8());
    buffer.push_back(0x15);                                   //功能码
    buffer.push_back(QByteArray::fromHex(QString::number(REAGENT_LIMIT,HEX_SWITCH).toUtf8())); //命令编号
    buffer.push_back(QByteArray::fromHex(QString::number(1,HEX_SWITCH).toUtf8()));             //缓存命令

    QBitArray  bitycode(8);
    bitycode.fill(false);
    bitycode.setBit(4, true);
    QString sSendHex = bitArray2String(bitycode);
    QByteArray byte_ = QByteArray::fromHex(sSendHex.toLatin1());
    buffer.push_back(byte_);

    QList<quint8> indexList;
    indexList<<INDEX_AA_CONSUMABLE<<INDEX_ADP_CONSUMABLE<<INDEX_EPI_CONSUMABLE<<INDEX_COL_CONSUMABLE<<INDEX_RIS_CONSUMABLE
            <<INDEX_TESTTUBE_CONSUMABLE<<INDEX_CLEANLINQUE_CONSUMABLE;
    for(int n = 0; n < indexList.size(); n++)
    {
        quint8 limitValue = 0;
        quint8 index_reag = indexList.at(n);
        ConsumablesOper::GetpInstance()->updateReagentLimit(READ_OPERRAT,index_reag,limitValue);
        buffer.push_back(QByteArray::fromHex(QString::number(limitValue,HEX_SWITCH).toUtf8()));
    }

    quint16 cleancapaity = 0;
    ConsumablesOper::GetpInstance()->updateReagentTotal(READ_OPERRAT,INDEX_CLEANLINQUE_CONSUMABLE,cleancapaity);
    QLOG_DEBUG()<<"写入S1清洗液总容量"<<cleancapaity;
    QByteArray EndianArry = qint16ToQByteArray(cleancapaity, false);
    buffer.push_back(EndianArry);

    buffer.push_back(QByteArray::fromHex(QString::number(0,16).toUtf8()));
    QString framedata  = buffer.toHex(' ').trimmed().toUpper();
    QLOG_DEBUG() << "写试剂限位命令:" << framedata<<endl;
    return;
}


//试剂容量
void QUIUtils::_writeReagentCapacity(QByteArray &buffer)
{
    buffer.clear();
    buffer = QByteArray::fromHex(QString::number(MAIN_CONTROL,HEX_SWITCH).toUtf8());
    buffer.push_back(0x15);                                   //功能码
    buffer.push_back(QByteArray::fromHex(QString::number(REAGENT_CAPACITY,HEX_SWITCH).toUtf8())); //命令编号
    buffer.push_back(QByteArray::fromHex(QString::number(1,HEX_SWITCH).toUtf8()));             //缓存命令

    QBitArray  bitycode(8);
    bitycode.fill(false);
    bitycode.setBit(4, true);
    QString sSendHex = bitArray2String(bitycode);
    QByteArray byte_ = QByteArray::fromHex(sSendHex.toLatin1());
    buffer.push_back(byte_);

    QList<quint8> indexList;
    indexList<<INDEX_AA_CONSUMABLE<<INDEX_ADP_CONSUMABLE<<INDEX_EPI_CONSUMABLE<<INDEX_COL_CONSUMABLE<<INDEX_RIS_CONSUMABLE;

    for(int n = 0; n < indexList.size(); n++)
    {
        quint16 reagcapaity = 0;
        quint8 index_reag = indexList.at(n);
        ConsumablesOper::GetpInstance()->updateReagentTotal(READ_OPERRAT,index_reag,reagcapaity);
        QByteArray EndianArry = qint16ToQByteArray(reagcapaity, false);
        buffer.push_back(EndianArry);
    }
    QString framedata  = buffer.toHex(' ').trimmed().toUpper();
    QLOG_DEBUG()<< "写试剂总容量命令:" << framedata<<endl;
    return;
}

void QUIUtils::_writeParaNumIOrder(QByteArray &buffer,quint16 (&moduleI)[3],bool (&ChanState)[12],
                            bool scanbar, bool rightReagent, bool initcatchcups, quint16 dimmlED){

    buffer.clear();
    buffer = QByteArray::fromHex(QString::number(MAIN_CONTROL,HEX_SWITCH).toUtf8());
    buffer.push_back(0x15);                                   //功能码
    buffer.push_back(QByteArray::fromHex(QString::number(EQUIPMENTPARA_I,HEX_SWITCH).toUtf8())); //命令编号
    buffer.push_back(QByteArray::fromHex(QString::number(1,HEX_SWITCH).toUtf8()));

    QBitArray  bitycode(8);
    bitycode.fill(false);
    bitycode.setBit(4, true);
    QString sSendHex = bitArray2String(bitycode);
    QByteArray byte_ = QByteArray::fromHex(sSendHex.toLatin1());
    buffer.push_back(byte_);

    QByteArray EndianArry = qint16ToQByteArray(moduleI[0], false);
    buffer.push_back(EndianArry);

    EndianArry = qint16ToQByteArray(moduleI[1], false);
    buffer.push_back(EndianArry);

    EndianArry = qint16ToQByteArray(moduleI[2], false);
    buffer.push_back(EndianArry);

    QBitArray  bitChnState(8);
    bitChnState.fill(false);
    for(int n = 0; n < 8; n++)
    {
        bitChnState.setBit(n, ChanState[n]);
    }
    sSendHex = bitArray2String(bitChnState);
    byte_ = QByteArray::fromHex(sSendHex.toLatin1());
    buffer.push_back(byte_);

    QBitArray  bitChnStateOthers(8);
    bitChnState.fill(false);
    for(int n = 0 ; n < 4; n++)
        bitChnStateOthers.setBit(n,ChanState[n+8]);

    bitChnStateOthers.setBit(4,scanbar);
    bitChnStateOthers.setBit(5,rightReagent);
    bitChnStateOthers.setBit(6,initcatchcups);
    QLOG_DEBUG()<<"开机抓杯子状态设置"<<initcatchcups <<"==="<<bitChnStateOthers;
    sSendHex = bitArray2String(bitChnStateOthers);
    byte_ = QByteArray::fromHex(sSendHex.toLatin1());
    buffer.push_back(byte_);

    EndianArry = qint16ToQByteArray(dimmlED, false);
    buffer.push_back(EndianArry);
    QLOG_DEBUG()<<"0x01写入:"<<buffer.toHex(' ').trimmed().toUpper()<<"leng="<<buffer.size()<<endl;
    return;
}

void QUIUtils::_writeParaNumIIHandsOrder(QByteArray &buffer,quint8 throwdownmm,quint8 (&handsinierTray)[4],quint8 (&HandsinnerModule)[3],bool brebackCatch){

    buffer.clear();
    buffer = QByteArray::fromHex(QString::number(MAIN_CONTROL,HEX_SWITCH).toUtf8());
    buffer.push_back(0x15);                                   //功能码
    buffer.push_back(QByteArray::fromHex(QString::number(HANDSPARADATA_II,HEX_SWITCH).toUtf8())); //命令编号
    buffer.push_back(QByteArray::fromHex(QString::number(1,HEX_SWITCH).toUtf8()));

    QBitArray  funcbit(8);
    funcbit.fill(false);
    funcbit.setBit(4, true);
    QString fucnstr = bitArray2String(funcbit);
    QByteArray fucnbyte_ = QByteArray::fromHex(fucnstr.toLatin1());
    buffer.push_back(fucnbyte_);

    buffer.push_back(QByteArray::fromHex(QString::number(throwdownmm,HEX_SWITCH).toUtf8()));

    for(int n = 0; n < 4; n++)
        buffer.push_back(QByteArray::fromHex(QString::number(handsinierTray[n],HEX_SWITCH).toUtf8()));

    for(int f = 0; f < 3; f++)
        buffer.push_back(QByteArray::fromHex(QString::number(HandsinnerModule[f],HEX_SWITCH).toUtf8()));

    QBitArray  bitycode(8);
    bitycode.fill(false);
    bitycode.setBit(0, brebackCatch);
    QString sSendHex = bitArray2String(bitycode);
    QByteArray byte_ = QByteArray::fromHex(sSendHex.toLatin1());
    buffer.push_back(byte_);

    buffer.push_back(QByteArray::fromHex(QString::number(0,HEX_SWITCH).toUtf8()));
    QLOG_DEBUG()<<"0x02写入:"<<buffer.toHex(' ').trimmed().toUpper()<<"leng="<<buffer.size()<<endl;
    return;
}

void QUIUtils::_writeParaNumBloodOrder(QByteArray &buffer,
                                         quint16 cleanLinqueblood,
                                         quint16 suckppporprp,
                                         quint16 suckairs,
                                         quint32  cleanTime,bool iswrite)
{
    buffer.clear();
    // 1. 处理MAIN_CONTROL
    buffer.append(static_cast<char>(MAIN_CONTROL));

    buffer.push_back(static_cast<char>(0x15));  // 功能码

    // 2. 处理命令编号BLOODPINPARAOTHERDATA（假设为quint8）
    buffer.push_back(static_cast<char>(BlOODPINPARADATA));

    // 3. 直接写入固定值1（quint8）
    buffer.push_back(static_cast<unsigned char>(0x01));

    // 4. 处理funcbit（QBitArray）
    QBitArray funcbit(8);
    funcbit.fill(false);
    funcbit.setBit(4, iswrite);

    // 将 QBitArray 转换为 quint8
    // 高位在前：索引 4 对应二进制第 3 位 写入flash
    quint8 funcByte = 0;
    for (int i = 0; i < funcbit.size(); ++i) {
        if (funcbit.testBit(i)) {
            funcByte |= (1 << (7 - i));
        }
    }
    buffer.push_back(static_cast<char>(funcByte));


    QDataStream stream(&buffer, QIODevice::Append);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << cleanLinqueblood<<suckppporprp<<suckairs<<cleanTime;

    QLOG_DEBUG()<<"0x04写入:"<<buffer.toHex(' ').trimmed().toUpper()<<"leng="<<buffer.size()<<endl;
    return;
}

void QUIUtils::_writeParaBloodOtherOrder(QByteArray &buffer, quint8 (&bloodpara)[10])
{
    buffer.clear();
    // 1. 处理MAIN_CONTROL
    buffer.append(static_cast<char>(MAIN_CONTROL));

    buffer.push_back(static_cast<char>(0x15));  // 功能码

    // 2. 处理命令编号BLOODPINPARAOTHERDATA（假设为quint8）
    buffer.push_back(static_cast<char>(BLOODPINPARAOTHERDATA));

    // 3. 直接写入固定值1（quint8）
    buffer.push_back(static_cast<unsigned char>(0x01));

    // 4. 处理funcbit（QBitArray）
    QBitArray funcbit(8);
    funcbit.fill(false);
    funcbit.setBit(4, true);

	// 将 QBitArray 转换为 quint8
	// 高位在前：索引 4 对应二进制第 3 位 写入flash
	quint8 funcByte = 0;
	for (int i = 0; i < funcbit.size(); ++i) {
		if (funcbit.testBit(i)) {
			funcByte |= (1 << (7 - i));
		}
	}
	buffer.push_back(static_cast<char>(funcByte));

    // 5. 直接写入bloodpara数组
    for (int n = 0; n < 10; n++) {
        buffer.push_back(static_cast<char>(bloodpara[n]));
    }

    QLOG_DEBUG() << "0x16写入:" << buffer.toHex(' ').trimmed().toUpper() << "leng=" << buffer.size();
    return;
}

void QUIUtils::_writeReagPinParaDataOrder(QByteArray &bufferI, QByteArray &bufferII,
                                            quint8 (&ReagentSuck)[5],quint8 (&ReagentDown)[5], quint8 (&otherdata)[8])
{
    bufferI.clear();
    bufferI = QByteArray::fromHex(QString::number(MAIN_CONTROL,HEX_SWITCH).toUtf8());
    bufferI.push_back(0x15);//功能码
    bufferI.push_back(QByteArray::fromHex(QString::number(PARAREAGENTPINDATA_I,HEX_SWITCH).toUtf8())); //命令编号
    bufferI.push_back(QByteArray::fromHex(QString::number(1,HEX_SWITCH).toUtf8()));

    QBitArray  funcbit(8);
    funcbit.fill(false);
    QString fucnstr = bitArray2String(funcbit);
    QByteArray fucnbyte_ = QByteArray::fromHex(fucnstr.toLatin1());
    bufferI.push_back(fucnbyte_);

    // 直接写入ReagentSuck和ReagentDown的原始字节
    bufferI.append(reinterpret_cast<const char*>(ReagentSuck), 5);
    bufferI.append(reinterpret_cast<const char*>(ReagentDown), 5);
    QLOG_DEBUG()<<"0x17缓存:"<< bufferI.toHex(' ').trimmed().toUpper()<<"leng="<< bufferI.size()<<endl;

    bufferII.clear();
    bufferII = QByteArray::fromHex(QString::number(MAIN_CONTROL,HEX_SWITCH).toUtf8());
    bufferII.push_back(0x15);                                   //功能码
    bufferII.push_back(QByteArray::fromHex(QString::number(PARAREAGENTPINDATA_II,HEX_SWITCH).toUtf8())); //命令编号
    bufferII.push_back(QByteArray::fromHex(QString::number(1,HEX_SWITCH).toUtf8()));
    QBitArray funcbitother(8);
    funcbitother.fill(false);
    funcbitother.setBit(4, true);
    fucnstr = bitArray2String(funcbitother);
    fucnbyte_ = QByteArray::fromHex(fucnstr.toLatin1());
    bufferII.push_back(fucnbyte_);


    bufferII.append(reinterpret_cast<const char*>(otherdata),8);
    bufferII.append(2,0x00);
    QLOG_DEBUG()<<"0x18写入:"<< bufferII.toHex(' ').trimmed().toUpper()<<"leng="<< bufferII.size()<<endl;
    return;
}



QByteArray QUIUtils::WriteBottleLimitArry(QMap<int,quint8> ParaData,const bool Writedirectly)
{
    QByteArray buffer;
    buffer.clear();
    // 1. 处理MAIN_CONTROL
    buffer.append(static_cast<char>(MAIN_CONTROL));

    buffer.push_back(static_cast<char>(0x15));  // 功能码

    // 2. 处理命令编号BLOODPINPARAOTHERDATA（假设为quint8）
    buffer.push_back(static_cast<char>(PARALIMINTBOTTLE));

    // 3. 直接写入固定值1（quint8）
    buffer.push_back(static_cast<unsigned char>(0x01));

    // 4. 处理funcbit（QBitArray）
    QBitArray funcbit(8);
    funcbit.fill(false);
    funcbit.setBit(4, Writedirectly);

    // 将 QBitArray 转换为 quint8
    // 高位在前：索引 4 对应二进制第 3 位 写入flash
    quint8 funcByte = 0;
    for (int i = 0; i < funcbit.size(); ++i) {
        if (funcbit.testBit(i)) {
            funcByte |= (1 << (7 - i));
        }
    }
    buffer.push_back(static_cast<char>(funcByte));

    auto itmap = ParaData.constBegin();
    while(itmap != ParaData.constEnd())
    while(itmap != ParaData.constEnd())
    {
        buffer.push_back(static_cast<char>(itmap.value()));
        itmap++;
    }
    QLOG_DEBUG() << "0x19写入:" << buffer.toHex(' ').trimmed().toUpper() << "leng=" << buffer.size();
	return buffer;
}


// 辅助函数：直接追加quint16到QByteArray
void appendQint16(QByteArray &arr, quint16 value, bool bigEndian) {
    const char* data = reinterpret_cast<const char*>(&value);
    if (bigEndian) {
        arr.append(data[1]).append(data[0]);  // 大端序
    } else {
        arr.append(data[0]).append(data[1]);  // 小端序
    }
}
QByteArray QUIUtils::writeGripperParaDataArry(const quint16 lessMax,const quint16 bigthanMin,const quint16 suckTime,
                                       const bool Writedirectly)
{
    QByteArray buffer;
    buffer.clear();
    // 1. 处理MAIN_CONTROL
    buffer.append(static_cast<char>(MAIN_CONTROL));

    buffer.push_back(static_cast<char>(0x15));  // 功能码

    // 2. 处理命令编号BLOODPINPARAOTHERDATA（假设为quint8）
    buffer.push_back(static_cast<char>(CONTROLGRIPPERPARA));

    // 3. 直接写入固定值1（quint8）
    buffer.push_back(static_cast<unsigned char>(0x01));

    QBitArray funcbit(8);
    funcbit.fill(false);
    funcbit.setBit(4, Writedirectly);

    // 将 QBitArray 转换为 quint8
    // 高位在前：索引 4 对应二进制第 3 位 写入flash
    quint8 funcByte = 0;
    for (int i = 0; i < funcbit.size(); ++i) {
        if (funcbit.testBit(i)) {
            funcByte |= (1 << (7 - i));
        }
    }
    buffer.push_back(static_cast<char>(funcByte));

    appendQint16(buffer, lessMax, false);    // 自定义追加函数
    appendQint16(buffer, bigthanMin, false);  // 复用buffer避免临时对象
    appendQint16(buffer, suckTime, false);

	quint16 disUsedByte = 0;
	appendQint16(buffer, disUsedByte, false);
	appendQint16(buffer, disUsedByte, false);

    //日志输出简化
    QLOG_DEBUG() << "0x1A写入:" << buffer.toHex(' ').trimmed().toUpper() << "长度:" << buffer.size() << endl;
    return buffer;
}


void QUIUtils::get6_7NumcodeArry(QByteArray &x_Buffer, quint8 bufferState_x,  QByteArray &y_Buffer, quint8 bufferState_y)
{
    x_Buffer.clear();
    y_Buffer.clear();
    QMap<quint8,quint16> confAxispos_x ,confAxispos_y;
    confAxispos_x.clear();
    confAxispos_y.clear();
    QPoint OriginPos(0,0);

    SingletonAxis::GetInstance()->originPos(READ_OPERRAT, OriginPos);
    confAxispos_x.insert(0,OriginPos.x());
    confAxispos_y.insert(0,OriginPos.y());

    QPoint cleanzoneoffBloodNedl(0,0);
    SingletonAxis::GetInstance()->cleanZoneAxisPos(READ_OPERRAT,MOTOR_BLOOD_INDEX ,cleanzoneoffBloodNedl);
    confAxispos_x.insert(1,cleanzoneoffBloodNedl.x());
    confAxispos_y.insert(1,cleanzoneoffBloodNedl.y());

    QPoint cleanzoneoffReagNedl(0,0);
    SingletonAxis::GetInstance()->cleanZoneAxisPos(READ_OPERRAT,MOTOR_REAGNET_INDEX ,cleanzoneoffReagNedl);
    confAxispos_x.insert(2,cleanzoneoffReagNedl.x());
    confAxispos_y.insert(2,cleanzoneoffReagNedl.y());

    QPoint reagZoneAxispos(0,0);
    SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT,0,reagZoneAxispos);
    confAxispos_x.insert(3,reagZoneAxispos.x());
    confAxispos_y.insert(3,reagZoneAxispos.y());

    QPoint throwholpos(0,0);
    SingletonAxis::GetInstance()->throwTubeHolePos(READ_OPERRAT, throwholpos);
    confAxispos_x.insert(4,throwholpos.x());
    confAxispos_y.insert(4,throwholpos.y());

    x_Buffer = QUIUtils::bufferCodeAxis(AXIS_ORIGIN_X,bufferState_x,confAxispos_x);
    QLOG_DEBUG()<<"0x06:"<<"原点x长度:"<<x_Buffer.size()<<x_Buffer.toHex(' ').trimmed().toUpper()<<"buffer状态"<<bufferState_x;

    y_Buffer = QUIUtils::bufferCodeAxis(AXIS_ORIGIN_Y,bufferState_y, confAxispos_y);
    QLOG_DEBUG()<<"0x07:"<<"原点y长度:"<<y_Buffer.size()<<y_Buffer.toHex(' ').trimmed().toUpper()<<"buffer状态"<<bufferState_y<<endl;
    return;
}

void  QUIUtils::get89NumberCodeArry(QByteArray &x_Buffer,quint8 bufferState_x,QByteArray &y_Buffer,quint8 bufferState_y)
{
    x_Buffer.clear();
    y_Buffer.clear();
    int totalnum = 5;
    QMap<quint8,quint16> confAxispos_x ,confAxispos_y;
    confAxispos_x.clear();
    confAxispos_y.clear();
    for(int i = 0 ; i < totalnum ;i++)
    {
        QPoint chnPos(0,0);
        SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT, i, MOTOR_REAGNET_INDEX,chnPos);
        confAxispos_x.insert(i,chnPos.x());
        confAxispos_y.insert(i,chnPos.y());
    }
    x_Buffer = QUIUtils::bufferCodeAxis(AXIS_CHN1_5_X,bufferState_x,confAxispos_x);
    QLOG_DEBUG()<<"0x08:"<<"leng="<<x_Buffer.size()<<x_Buffer.toHex(' ').trimmed().toUpper()<<"buffer状态"<<bufferState_x;
    y_Buffer = QUIUtils::bufferCodeAxis(AXIS_CHN1_5_Y,bufferState_y, confAxispos_y);
    QLOG_DEBUG()<<"0x09:"<<"leng="<<y_Buffer.size()<<y_Buffer.toHex(' ').trimmed().toUpper()<<"buffer状态"<<bufferState_y<<endl;
    return;
}

void  QUIUtils::get0a0bNumberCodeArry(QByteArray &x_Buffer, quint8 bufferState_x, QByteArray &y_Buffer, quint8 bufferState_y)
{
    x_Buffer.clear();
    y_Buffer.clear();
    QMap<quint8,quint16> confAxispos_x ,confAxispos_y;
    confAxispos_x.clear();
    confAxispos_y.clear();
    for(int i = 5 ; i < 10 ;i++)
    {
        QPoint chnPos(0,0);
        SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT, i,MOTOR_REAGNET_INDEX,chnPos);
        confAxispos_x.insert(i-5,chnPos.x());
        confAxispos_y.insert(i-5,chnPos.y());
    }
    x_Buffer = QUIUtils::bufferCodeAxis(AXIS_CHN6_10_X,bufferState_x,confAxispos_x);
    QLOG_DEBUG()<<"0x0A缓存leng:"<<x_Buffer.size()<<x_Buffer.toHex(' ').trimmed().toUpper()<<"buffer状态"<<bufferState_x;
    y_Buffer = QUIUtils::bufferCodeAxis(AXIS_CHN6_10_Y,bufferState_y,confAxispos_y);
    QLOG_DEBUG()<<"0x0B写入leng:"<<y_Buffer.size()<<y_Buffer.toHex(' ').trimmed().toUpper()<<"buffer状态"<<bufferState_y<<endl;
    return;
}

void  QUIUtils::get0c0dNumberCodeArry(QByteArray &x_Buffer, quint8 bufferState_x, QByteArray &y_Buffer, quint8 bufferState_y)
{
    x_Buffer.clear();
    y_Buffer.clear();
    QMap<quint8,quint16> confAxispos_x ,confAxispos_y;
    confAxispos_x.clear();
    confAxispos_y.clear();
    for(int i = 10 ; i < 12 ;i++)
    {
        QPoint chnPos(0,0);
        SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT, i,MOTOR_REAGNET_INDEX,chnPos);
        confAxispos_x.insert(i-10,chnPos.x());
        confAxispos_y.insert(i-10,chnPos.y());
    }
    for(int k = 0 ; k < 3; k++)
    {
         QPoint chnPosoffsetHands(0,0);
         SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT, k,MOTOR_HANDS_INDEX,chnPosoffsetHands);
         confAxispos_x.insert(k+2, chnPosoffsetHands.x());
         confAxispos_y.insert(k+2, chnPosoffsetHands.y());
    }
    x_Buffer = QUIUtils::bufferCodeAxis(AXIS_CHN11_12_X,bufferState_x,confAxispos_x);
    QLOG_DEBUG()<<"0x0C缓存leng:"<<x_Buffer.size()<<x_Buffer.toHex(' ').trimmed().toUpper()<<"buffer状态"<<bufferState_x;
    y_Buffer = QUIUtils::bufferCodeAxis(AXIS_CHN11_12_Y,bufferState_y,confAxispos_y);
    QLOG_DEBUG()<<"0x0D写入leng:"<<y_Buffer.size()<<y_Buffer.toHex(' ').trimmed().toUpper()<<"buffer状态"<<bufferState_y<<endl;
    return;
}

void  QUIUtils::get0e0fNumberCodeArry(QByteArray &x_Buffer, quint8 bufferState_x, QByteArray &y_Buffer, quint8 bufferState_y)
{
    x_Buffer.clear();
    y_Buffer.clear();
    QMap<quint8,quint16> confAxispos_x ,confAxispos_y;
    confAxispos_x.clear();
    confAxispos_y.clear();
    for(int k = 3 ; k < 8; k++)
    {
         QPoint chnPosoffsetHands(0,0);
         SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT, k,MOTOR_HANDS_INDEX,chnPosoffsetHands);
         confAxispos_x.insert(k-3, chnPosoffsetHands.x());
         confAxispos_y.insert(k-3, chnPosoffsetHands.y());
    }
    x_Buffer = QUIUtils::bufferCodeAxis(AXIS_CHN4_8_HANDSX,bufferState_x,confAxispos_x);
    QLOG_DEBUG()<<"0x0E缓存leng:"<<x_Buffer.size()<<x_Buffer.toHex(' ').trimmed().toUpper()<<"buffer状态"<<bufferState_x;
    y_Buffer = QUIUtils::bufferCodeAxis(AXIS_CHN4_8_HANDSY,bufferState_y,confAxispos_y);
    QLOG_DEBUG()<<"0x0F写入leng:"<<y_Buffer.size()<<y_Buffer.toHex(' ').trimmed().toUpper()<<"buffer状态"<<bufferState_y<<endl;
    return;
}

void  QUIUtils::get1011NumberCodeArry(QByteArray &x_Buffer, quint8 bufferState_x, QByteArray &y_Buffer, quint8 bufferState_y)
{
    x_Buffer.clear();
    y_Buffer.clear();
    QMap<quint8,quint16> confAxispos_x ,confAxispos_y;
    confAxispos_x.clear();
    confAxispos_y.clear();
    for(int k = 8 ; k < 12; k++)
    {
         QPoint chnPosoffsetHands(0,0);
         SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT, k,MOTOR_HANDS_INDEX,chnPosoffsetHands);
         confAxispos_x.insert(k-8, chnPosoffsetHands.x());
         confAxispos_y.insert(k-8, chnPosoffsetHands.y());
    }
    QPoint bloodSamplePos(0,0);
    SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT,0,bloodSamplePos);
    confAxispos_x.insert(4,bloodSamplePos.x());
    confAxispos_y.insert(4,bloodSamplePos.y());
    x_Buffer = QUIUtils::bufferCodeAxis(AXIS_CHN9_12_HANDSX,bufferState_x,confAxispos_x);
    QLOG_DEBUG()<<"0x10缓存leng:"<<x_Buffer.size()<<x_Buffer.toHex(' ').trimmed().toUpper()<<"buffer状态"<<bufferState_x;
    y_Buffer = QUIUtils::bufferCodeAxis(AXIS_CHN9_12_HANDSY,bufferState_y,confAxispos_y);
    QLOG_DEBUG()<<"0x11写入leng:"<<y_Buffer.size()<<y_Buffer.toHex(' ').trimmed().toUpper()<<"buffer状态"<<bufferState_y<<endl;
    return;

}

void  QUIUtils::get1213NumberCodeArry(QByteArray &x_Buffer, quint8 bufferState_x, QByteArray &y_Buffer, quint8 bufferState_y)
{
    x_Buffer.clear();
    y_Buffer.clear();
    QMap<quint8,quint16> confAxispos_x ,confAxispos_y;
    confAxispos_x.clear();
    confAxispos_y.clear();
    QPoint traytubeoffsetBloodNeld[4];
    QPoint traytubeoffsetHands_1(0,0);
    for(int i = 0; i < 4; i++)
    {
        SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,i+(i*59),MOTOR_BLOOD_INDEX,traytubeoffsetBloodNeld[i]);
        confAxispos_x.insert(i,traytubeoffsetBloodNeld[i].x());
        confAxispos_y.insert(i,traytubeoffsetBloodNeld[i].y());
    }
    SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,0,MOTOR_HANDS_INDEX,traytubeoffsetHands_1);
    confAxispos_x.insert(4,traytubeoffsetHands_1.x());
    confAxispos_y.insert(4,traytubeoffsetHands_1.y());

    x_Buffer = QUIUtils::bufferCodeAxis(AXIS_TRAY_OFFSET_BLOODPINX,bufferState_x,confAxispos_x);
    QLOG_DEBUG()<<"0x12缓存leng:"<<x_Buffer.size()<<x_Buffer.toHex(' ').trimmed().toUpper()<<"buffer状态"<<bufferState_x;
    y_Buffer = QUIUtils::bufferCodeAxis(AXIS_TRAY_OFFSET_BLOODPINY,bufferState_y,confAxispos_y);
    QLOG_DEBUG()<<"0x13写入leng:"<<y_Buffer.size()<<y_Buffer.toHex(' ').trimmed().toUpper()<<"buffer状态"<<bufferState_y<<endl;
    return;

}

void  QUIUtils::get1415NumberCodeArry(QByteArray &x_Buffer, quint8 bufferState_x, QByteArray &y_Buffer, quint8 bufferState_y)
{
    x_Buffer.clear();
    y_Buffer.clear();
    QMap<quint8,quint16> confAxispos_x ,confAxispos_y;
    confAxispos_x.clear();
    confAxispos_y.clear();
    QPoint traytubeoffsetHands[3];
    for(int i = 1; i < 4; i++)
    {
        SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,i+(i*59),MOTOR_HANDS_INDEX,traytubeoffsetHands[i-1]);
        confAxispos_x.insert(i-1,traytubeoffsetHands[i - 1].x());
        confAxispos_y.insert(i-1,traytubeoffsetHands[i - 1].y());
    }
    QPoint notuse(0,0);
    confAxispos_x.insert(3,notuse.x());
    confAxispos_y.insert(3,notuse.y());
    confAxispos_x.insert(4,notuse.x());
    confAxispos_y.insert(4,notuse.y());
    x_Buffer = QUIUtils::bufferCodeAxis(AXIS_TRAY_OFFSET_HANDSX,bufferState_x,confAxispos_x);
    QLOG_DEBUG()<<"0x14缓存leng:"<<x_Buffer.size()<<x_Buffer.toHex(' ').trimmed().toUpper()<<"buffer状态"<<bufferState_x;
    y_Buffer = QUIUtils::bufferCodeAxis(AXIS_TRAY_OFFSET_HANDSY,bufferState_y,confAxispos_y);
    QLOG_DEBUG()<<"0x15写入leng:"<<y_Buffer.size()<<y_Buffer.toHex(' ').trimmed().toUpper()<<"buffer状态"<<bufferState_y<<endl;
    return;
}

void QUIUtils::mapEquipmentAxisCommmd(QMap<quint8,QByteArray> &sendArry)
{
    QByteArrayList ReadBuffAxis;
    ReadBuffAxis.clear();
    QByteArray xArry;
    QByteArray yArry;
    QUIUtils::get6_7NumcodeArry(xArry,AXISPOS_BUFFER,yArry,AXISPOS_BUFFER);
    sendArry.insert(AXIS_ORIGIN_X,xArry);
    sendArry.insert(AXIS_ORIGIN_Y,yArry);

    QUIUtils::get89NumberCodeArry(xArry,AXISPOS_BUFFER,yArry,AXISPOS_BUFFER);
    sendArry.insert(AXIS_CHN1_5_X,xArry);
    sendArry.insert(AXIS_CHN1_5_Y,yArry);

    QUIUtils::get0a0bNumberCodeArry(xArry,AXISPOS_BUFFER,yArry,AXISPOS_BUFFER);
    sendArry.insert(AXIS_CHN6_10_X,xArry);
    sendArry.insert(AXIS_CHN6_10_Y,yArry);

    QUIUtils::get0c0dNumberCodeArry(xArry,AXISPOS_BUFFER,yArry,AXISPOS_BUFFER);
    sendArry.insert(AXIS_CHN11_12_X,xArry);
    sendArry.insert(AXIS_CHN11_12_Y,yArry);

    QUIUtils::get0e0fNumberCodeArry(xArry,AXISPOS_BUFFER,yArry,AXISPOS_BUFFER);
    sendArry.insert(AXIS_CHN4_8_HANDSX,xArry);
    sendArry.insert(AXIS_CHN4_8_HANDSY,yArry);

    QUIUtils::get1011NumberCodeArry(xArry,AXISPOS_BUFFER,yArry,AXISPOS_BUFFER);
    sendArry.insert(AXIS_CHN9_12_HANDSX,xArry);
    sendArry.insert(AXIS_CHN9_12_HANDSY,yArry);

    QUIUtils::get1213NumberCodeArry(xArry,AXISPOS_BUFFER,yArry,AXISPOS_BUFFER);
    sendArry.insert(AXIS_TRAY_OFFSET_BLOODPINX,xArry);
    sendArry.insert(AXIS_TRAY_OFFSET_BLOODPINY,yArry);

    QUIUtils::get1415NumberCodeArry(xArry,AXISPOS_BUFFER,yArry,AXISPOS_WRITE);
    sendArry.insert(AXIS_TRAY_OFFSET_HANDSX,xArry);
    sendArry.insert(AXIS_TRAY_OFFSET_HANDSY,yArry);

    return;
}


void  QUIUtils::clearequipmentpara(QByteArray &clear_data)
{
    clear_data = QByteArray::fromHex(QString::number(MAIN_CONTROL).toUtf8());
    clear_data.push_back(0x15); //功能码
    QString strHex = QString::number(0, 16);
    clear_data.push_back(QByteArray::fromHex(strHex.toUtf8())); //命令编号
    clear_data.push_back(QByteArray::fromHex(QString::number(0).toUtf8())); //缓存容量
    clear_data.push_back(QByteArray::fromHex(QString::number(0).toUtf8()));  //写
	strHex = QString::number(170, 16); //0xaa
	clear_data.push_back(QByteArray::fromHex(strHex.toUtf8()));  //清空
    for(int i = 0; i< 9; i++)
    {
        QString otherdata = QString("%1").arg(QString::number(0, 16), 2, '0');
        QByteArray datatmp =  QByteArray::fromHex(otherdata.toLatin1());
        clear_data.push_back(datatmp);
    }
    return;
}

//确认读卡充值
void QUIUtils::ConfirmReadthecardRecharge(quint8 suppilesKind,quint8 indexnum_,quint8 totalSuppiles,
                                            QByteArray &sendData)
{

    sendData.clear();
    sendData.reserve(16); // 预分配空间

    // 命令头部分
    sendData.append(static_cast<char>(MAIN_CONTROL)); // 主控码
    sendData.append(0x16);                            // 功能码

    // 命令参数
    sendData.append(static_cast<char>(indexnum_));    // 命令编号
    sendData.append(static_cast<char>(0x00));

    // 填充4个0字节（位置4-7）
    sendData.append(QByteArray(4, 0x00));


    // 第8字节：高4位耗材类型 | 低4位读卡成功标志
    constexpr quint8 READ_SUCCESS_FLAG = 0x02; // 0001b
    quint8 typeByte = static_cast<quint8>((suppilesKind << 4) | READ_SUCCESS_FLAG);
    sendData.append(static_cast<char>(typeByte));

    // 确认数量
    sendData.append(static_cast<char>(totalSuppiles));

    // 耗材余量编码（全0）
    sendData.append(static_cast<char>(0x00));

    // 填充4字节空数据（小端序）
    QDataStream stream(&sendData, QIODevice::Append);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << quint16(0) << quint16(0);

    QLOG_DEBUG() << "发送读卡成功充值写入:" << indexnum_
                 << sendData.toHex(' ').toUpper()<<"长度:"<<sendData.length();
    return;
}


//耗材的读操作命令
void QUIUtils::readSuppliesControlOrder(QByteArray& outarry, quint8 indexnumcode,quint8 indexReag)
{
    outarry.clear();
    outarry.reserve(16); // 预分配空间

    // 命令头部分
    outarry.append(static_cast<char>(MAIN_CONTROL)); // 主控码 [[8]]
    outarry.append(0x16);                            // 功能码

    // 命令编号和缓存容量
    outarry.append(static_cast<char>(indexnumcode)); // 直接添加字节 [[8]]
    outarry.append(static_cast<char>(0x00));        // 缓存容量固定为0

    // 控制位设置（第5字节）
    quint8 controlByte = 0x01; // 读
    outarry.append(static_cast<char>(controlByte));

    // 填充3个0字节（第6-8字节）
    outarry.append(QByteArray(3, 0x00));

    // 耗材位置编码（第9字节）
    quint8 positionByte = static_cast<quint8>((indexReag << 4) & 0xF0); // 高4位 [[10]]
    outarry.append(static_cast<char>(positionByte));

    // 单位设置（第10字节）
    outarry.append(static_cast<char>(0x00)); // 单位固定为瓶

    // 耗材余量编码（第11字节）
    outarry.append(static_cast<char>(positionByte)); // 与位置编码相同

    // 填充4字节空数据（小端序）
    QDataStream stream(&outarry, QIODevice::Append);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << quint16(0) << quint16(0); // 写入两个16位0值 [[6]]

    QString outtext = GlobalData::mapReagentNames(indexReag);
    QLOG_DEBUG()<<outtext<<"读取试剂余量命令"<< outarry.toHex(' ').trimmed().toUpper();
    return;
}


//耗材的缓存 瓶数和百分比操作
void QUIUtils::writeSuppliesBottleControlOrder(QByteArray& outarry, quint8 indexnumcode, quint8 indexReag,
                                                  quint8 configBottle, quint16 bottleRatio,const bool isBuffer)
{
    outarry.clear();
    outarry.reserve(16); // 预分配空间

    // 命令头部分
    outarry.append(static_cast<char>(MAIN_CONTROL)); // 主控码
    outarry.append(0x16);                            // 功能码

    // 命令参数
    outarry.append(static_cast<char>(indexnumcode)); // 命令编号
    outarry.append(static_cast<char>(0x00));         // 缓存容量


    // 控制字节（全0）
    outarry.append(static_cast<char>(0x00)); // 替换原QBitArray操作

    // 填充3个0字节
    outarry.append(QByteArray(3, 0x00));

    // 耗材位置编码（高4位indexReag，低4位0x1)
    quint8 positionByte  = 0;
    if(isBuffer)
        positionByte = static_cast<quint8>((indexReag << 4) | CONSUMABLES_REDUCE);
    else
        positionByte = static_cast<quint8>((indexReag << 4) | BUFFER_WRITE);
    outarry.append(static_cast<char>(positionByte));

    // 瓶数配置
    outarry.append(static_cast<char>(configBottle));

    // 百分比编码（复用位置字节结构)
    if(!isBuffer)
         outarry.append(static_cast<char>(0x00)); //写入
    else
         outarry.append(static_cast<char>(positionByte));

    // 百分比值（小端序）
    QDataStream stream(&outarry, QIODevice::Append);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << bottleRatio;

    // 填充最后2字节空数据
    stream << quint16(0);
    QString outtext = isBuffer? "缓存": "写入";
    QLOG_DEBUG()<<outtext<<"试剂耗材余量命令"<<indexReag<< outarry.toHex(' ').trimmed().toUpper()
               <<"长度="<<outarry.length()<<endl;
    return;
}



//遍历主板命令(读)
void QUIUtils::Traverse_the_motherboard(QByteArray &MotherboardCommd)
{
    MotherboardCommd.clear();
    MotherboardCommd.reserve(16); // 预分配15字节空间

    // 命令头构造
    MotherboardCommd.append(static_cast<char>(MAIN_CONTROL)); // 主控码（直接字节转换）
    MotherboardCommd.append(0x16);                            // 功能码

    // 命令参数部分
    MotherboardCommd.append(static_cast<unsigned char>(0xFE));  // 命令编号254 EVERYTIME_READ_MAINSTATE (0xFE)
    MotherboardCommd.append(static_cast<char>(0x00));                     // 缓存容量1
    MotherboardCommd.append(0x01);                     // 读操作标识

    // 填充后续11个空字节（位置5-15）
    MotherboardCommd.append(QByteArray(10, 0x00));      // 一次性填充11个0

    /* 字节结构说明：
    字节0 : 主控码
    字节1 : 功能码(0x16)
    字节2 : 命令编号(0xFF)
    字节3 : 缓存容量(0x01)
    字节4 : 读操作标识(0x01)
    字节5-15 : 保留位全0
    */
    return;
}


//遍历模组命令(读)
void QUIUtils::Traverse_the_Module(int IndexModul, QByteArray &moduleCommand)
{
    moduleCommand.clear();
    moduleCommand.reserve(16); // 预分配15字节空间

    // 命令头构造
    moduleCommand.append(static_cast<char>(IndexModul)); // 模块地址
    moduleCommand.append(0x17);                          // 功能码

    // 命令参数部分
    moduleCommand.append(static_cast<char>(IndexModul)); // 命令编号（与模块地址相同）
    moduleCommand.append(0x01);                         // 缓存命令
    moduleCommand.append(0x01);                         // 读操作标识

    // 填充后续10个空字节（位置5-14）
    moduleCommand.append(QByteArray(10, 0x00));

    /* 字节结构说明：
    字节0 : 模块地址 (IndexModul)
    字节1 : 功能码(0x17)
    字节2 : 命令编号(IndexModul)
    字节3 : 缓存命令(0x01)
    字节4 : 读操作标识(0x01)
    字节5-14 : 保留位全0
    */
}



//生成试剂区其它坐标
void  QUIUtils::CreatReagArsOtherAxis(QPoint indexFirstPos,QMap<quint8,QPoint> &ReagentZoneOffsetKitsPin)
{
    QPoint OtherAxis(0,0);
    constexpr int COLUMNS = 2;   // X轴方向列数
    constexpr int ROWS = 5;      // Y轴方向行数
    constexpr int X_STEP = 250;  // X轴步长
    constexpr int Y_STEP = 240;
    int m = 0;
    for(int i = 0; i < COLUMNS ; i++){
        for(int k = 0; k< ROWS; k++)
        {
            OtherAxis.setX(indexFirstPos.x() + i*X_STEP);
            OtherAxis.setY(indexFirstPos.y() + k*Y_STEP);
            ReagentZoneOffsetKitsPin.insert(m,OtherAxis);
            m++;
        }
    }
    //QLOG_DEBUG()<<"试剂区坐标"<<ReagentZoneOffsetKitsPin;
}

//生成血样区坐标
void  QUIUtils::creatBloodSampleAxis(int equipType, QPoint firstPos, QMap<quint8,QPoint> &bloodSampleZone)
{
    const uint inGroupLSpace = 170; //组内竖直间距
    const uint  inGroupHSpace = 170; //组内横向间距
    uint GroupSpace = 380; //组与组大间距
    uint holenum = 0;
    QPoint tmpAxis(0,0);
    int GroupTube = 0; //面板的竖直个数
    switch(equipType)
    {
        case KS600:
            GroupTube =   6;
            GroupSpace = 525;
        break;
        case KS800:
            GroupTube =  8;
            GroupSpace = 565;
        break;
        case KS1200:
            GroupTube =  12;
            GroupSpace = 380;
        break;
    default:
		QLOG_ERROR() << "写入血样孔坐标读取仪器类型异常" << __FUNCTION__ << __LINE__ << endl;
		break;
    }
    for(int g = 0 ; g < GroupTube  ; g++)
    {
        for(int r = 0; r < 5; r++)
        {
            for(int l = 0 ;l < 2; l++)
            {
                if((l+1)/2 == 0)
                   tmpAxis.setX(firstPos.x() + g*GroupSpace);
                else
                   tmpAxis.setX(firstPos.x() + g*GroupSpace + inGroupHSpace);
                tmpAxis.setY(firstPos.y() + r*inGroupLSpace);
                bloodSampleZone.insert(holenum,tmpAxis);
                holenum++;
            }
        }
    }
}


//生成测试管区坐标
void  QUIUtils::creatTeatTubeAxiis(int indexZ, int equipType, QMap<quint8,QPoint> &TsetTubeAxispos,
                                     QPoint arrStartpos[], int length)
{
	QPoint heandPos[4] = {};
    for (int i = 0 ; i < length; ++i)
    {
       heandPos[i] = arrStartpos[i];
    }
    //1200 试管盘间距
    const uint spaceTube = 150; //内部试管横 纵向间距相同
    QPoint tmpaxis(0,0);
    int totalTray = 0;
    int m = 0;
    switch(equipType)
    {
        case KS600: totalTray = 2; break;
        case KS800: totalTray = 3; break;
        case KS1200: totalTray = 4; break;
    }
    if(indexZ == MOTOR_HANDS_INDEX)
    {
        for(int t = 0; t < totalTray; t++)
        {
            for(int r = 0; r < 10; r++)
            {
                for(int L = 0; L < 6; L++)
                {
                    tmpaxis.setX(heandPos[t].x() + L*spaceTube);
                    tmpaxis.setY(heandPos[t].y() + r*spaceTube);
                    TsetTubeAxispos.insert(m,tmpaxis);
                    m++;
                }
            }
        }
    }
    else if(indexZ == MOTOR_BLOOD_INDEX)
    {
        for(int t = 0; t < totalTray; t++)
        {
            for(int r = 0; r < 10; r++)
            {
                for(int L = 0; L < 6; L++)
                {
                    tmpaxis.setX(heandPos[t].x() + L*spaceTube);
                    tmpaxis.setY(heandPos[t].y() + r*spaceTube);
                    TsetTubeAxispos.insert(m,tmpaxis);
                    m++;
                }
            }
        }
    }
}




double QUIUtils::reagent_pin_clean_deplete(int turn_)
{
    double deplete_ul = (turn_ * 100)/7200;
    QLOG_DEBUG()<<"清洗试剂针转动/总圈* 容量(ul)"<<turn_<<"="<<deplete_ul;
    return deplete_ul;
}

double QUIUtils::bloody_pin_clean_deplete(int turn_)
{
    double deplete_ul = (turn_ * 2500)/7200;
    QLOG_DEBUG()<<"清洗血样针转动/总圈* 容量(ul)"<<turn_<<"*"<<2500<<"/"<<7200<<"="<<deplete_ul;
    return deplete_ul;
}


//测试抓手抓取测试杯区试管准确度
void QUIUtils::test_hands_catchtube_precise(QPoint form_axis,QPoint end_axis,int down_mm ,QByteArrayList &directives_list)
{
    quint8 code_num = 0;
    QByteArray directives_;
    directives_list.clear();
    bool bRepeattheGrabCup = INI_File().rConfigPara(REPEATGRABCUP).toBool(); //重抓标志
    directives_ = Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(form_axis,OFFSET_ZHANDS,TRAY_TESTENPTY,code_num);
    directives_list.push_back(directives_);

    //抓手下降
    directives_ = Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,down_mm,TRAY_TESTENPTY,code_num,
                                                                 false,down_mm,false,GRABCUPWITHGRIPPER);
    directives_list.push_back(directives_);

    //抓手复位
    directives_ = Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX, down_mm, TRAY_TESTENPTY, code_num,bRepeattheGrabCup);
    directives_list.push_back(directives_);

    directives_ =  Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(end_axis, MOTOR_HANDS_INDEX, TRAY_TESTENPTY, code_num);
    directives_list.push_back(directives_);

    //抓手下降(在放试管盘)
    directives_ = Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,down_mm,TRAY_TESTENPTY,
                                                                 code_num,false,down_mm,false,GRIPPERNORMAL);
    directives_list.push_back(directives_);

    int move_back_putube_y = std::abs(end_axis.y() - PUTTUBEMOVE);
    QPoint back_axis(end_axis.x(),move_back_putube_y);
    directives_ =  Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(back_axis, MOTOR_HANDS_INDEX, TRAY_TESTENPTY, code_num);
    directives_list.push_back(directives_);

    //抓手复位
    directives_ = Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX, 0, TRAY_TESTENPTY, code_num,false);
    directives_list.push_back(directives_);

    return;
}


//复位全部Z轴
void QUIUtils::allZAxisBackOrigin(QByteArrayList &ZbackOrigin,quint8 hadsize)
{
    int faileddownHeigh = 0;
    quint8 _num = hadsize;
    quint8 totalArry = 3;
    QByteArray *pBackNeedle = new QByteArray[totalArry];
    pBackNeedle[0] = Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,faileddownHeigh,TRAY_TESTCHANNEL,_num,false);
    pBackNeedle[1] = Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_REAGNET_INDEX,faileddownHeigh,TRAY_TESTCHANNEL,_num,false);
    pBackNeedle[2] = Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,faileddownHeigh,TRAY_TESTCHANNEL,_num,false);
    for(int n = 0; n < totalArry; n++)
        ZbackOrigin.push_back(pBackNeedle[n]);
    delete []pBackNeedle;
    return;
}

void QUIUtils::_equipmentbackoriginloc(QByteArrayList &_backOrigin)
{
    _backOrigin.clear();
    allZAxisBackOrigin(_backOrigin, _backOrigin.size());
    quint8 _num = _backOrigin.size()%255;
    QPoint originLoc(0,0);
    SingletonAxis::GetInstance()->originPos(READ_OPERRAT,originLoc);
    _backOrigin.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(originLoc,0,0,_num));

    //负速度模式复位
    _backOrigin.push_back(Testing::m_TaskDll->_DLLXYMoveReposition_(_num,MACHINEBACK_SPEED/2));
    _backOrigin.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(QPoint(0,0),0,0,_num));
    _backOrigin.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(originLoc,0,0,_num));
    return;
}


//灌注动作开机
QByteArrayList QUIUtils::perfusionAction(uint &back_leng,const bool enoughReagent,const int cleantime)
{
    QPoint Originaxis(0,0);
    SingletonAxis::GetInstance()->originPos(READ_OPERRAT,Originaxis);
    int  benbackOrigin = 0, benfull = 7200;
    quint8 bloodPinDown =   12;     //血样针开机清洗高度
    quint8 reagentPindown = 15;     //试剂针开机清洗高度
    QByteArrayList activeCommandarry;
    allZAxisBackOrigin(activeCommandarry,0); //执行动作前提Z复位
    activeCommandarry.reserve(38);
    quint8 num = activeCommandarry.count()%255;
    auto *pactive = Testing::m_TaskDll;
    activeCommandarry.push_back(pactive->_DLLXYMoveReposition_(num,MACHINEBACK_SPEED)); //负速度模式复位
    activeCommandarry.push_back(pactive->DLL_XYMoveSpecifiedPosition(Originaxis,0,0,num));
    activeCommandarry.push_back(pactive->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,bloodPinDown,0,num,
                                                                   false,bloodPinDown,false,GRIPPERNORMAL));
    activeCommandarry.push_back(pactive->BigBenActive(false,benbackOrigin,num,DIS_WASHES_PUMPS,0));
    activeCommandarry.push_back(pactive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num,false));   //血样针复位
    activeCommandarry.push_back(pactive->BigBenActive(true, benfull * INIT_CLEAN_RATIO, num, DIS_WASHES_PUMPS,0));
    activeCommandarry.push_back(pactive->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,bloodPinDown,0, num,false,
                                                                 bloodPinDown,false,GRIPPERNORMAL));
    activeCommandarry.push_back(pactive->BigBenActive(false,benbackOrigin,num,WASHES_PUMPS,cleantime));
    activeCommandarry.push_back(pactive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num,false));
    activeCommandarry.push_back(pactive->DLL_ZMoveSpecifiedPosition(MOTOR_REAGNET_INDEX,reagentPindown,0,num,
                                                                   false,reagentPindown,false,GRIPPERNORMAL));
    activeCommandarry.push_back(pactive->SmallBenActive(false,benbackOrigin,DIS_WASHES_PUMPS,num,0));
    activeCommandarry.push_back(pactive->DLL_ZAxis_Reset(MOTOR_REAGNET_INDEX,0,0,num,false));
    activeCommandarry.push_back(pactive->SmallBenActive(true,benfull,DIS_WASHES_PUMPS,num,0));
    activeCommandarry.push_back(pactive->DLL_ZMoveSpecifiedPosition(MOTOR_REAGNET_INDEX,reagentPindown,0,num,false,
                                                                   reagentPindown,false,GRIPPERNORMAL));
    activeCommandarry.push_back(pactive->SmallBenActive(false,benbackOrigin,WASHES_PUMPS,num,cleantime));
    activeCommandarry.push_back(pactive->DLL_ZAxis_Reset(MOTOR_REAGNET_INDEX,0,0,num,false));
    activeCommandarry.push_back(pactive->_DLLXYMoveReposition_(num,MACHINEBACK_SPEED));
    activeCommandarry.push_back(pactive->DLL_XYMoveSpecifiedPosition(Originaxis,0,0,num));

    if(enoughReagent){
       cleaningDoubleNeedleAction(activeCommandarry,1);
    }
    back_leng = activeCommandarry.count();
    return activeCommandarry;
}

void QUIUtils::_Locatetheorigin(QByteArrayList &ZbackOrigin, QPoint loc_,int down_)
{
    quint8 _num = 0%255;
    QByteArray command_;

    //执行动作前提Z复位
    allZAxisBackOrigin(ZbackOrigin,_num);

    _num = ZbackOrigin.size();

    command_ = Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(loc_,0,0,_num); //XY移动到血样针试剂位置
    ZbackOrigin.push_back(command_);

    command_ = Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,down_,0,_num,false,down_,false,GRIPPERNORMAL);
    ZbackOrigin.push_back(command_);

    command_ = Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_REAGNET_INDEX,down_,0,_num,false,down_,false,GRIPPERNORMAL);

    ZbackOrigin.push_back(command_);
    return;
}


//清洗双针动作
void QUIUtils::cleaningDoubleNeedleAction(QByteArrayList &CleanActionarry,int reagent_total)
{
    QPoint CleanAxis(0,0);
    QPoint CleanReagOffBloodPin(0,0);         //血样针到清洗试剂坐标
    QPoint CleanReagOffReagnetPin(0,0);      //试剂针到清洗位置
    const auto *pinstance =  SingletonAxis::GetInstance();

    pinstance->originPos(READ_OPERRAT,CleanAxis);
    pinstance->cleanZoneAxisPos(READ_OPERRAT,MOTOR_BLOOD_INDEX,CleanReagOffBloodPin);
    pinstance->cleanZoneAxisPos(READ_OPERRAT,MOTOR_REAGNET_INDEX,CleanReagOffReagnetPin);

    //先Z轴全部复位开始
    allZAxisBackOrigin(CleanActionarry, CleanActionarry.size());
    quint8 _num = CleanActionarry.count()%255;

    int failedBloodpin =   INI_File().GetFailedCleanLinqueHigh();          //清洗液探测失败血样针
    int failedreagentpin = INI_File().getFailedCleanLinqueReagNeedle();  //清洗液探测失败试剂针
    int washbloodPinVol =  INI_File().GetAbsorbWashingfluidX2();         //洗血样针洗的清洗剂量
    int washReagPinVol =   INI_File().GetAbsorbWashingfluidX1();          //洗试剂针吸清洗液的量
    //double  suckSpitPRPRatio = INI_File().getPRPConvertTheratioColumn();
    double suckSpitPPPRatio =   INI_File().GetPPPConversionScale();

    int CleanReagent_ul = 0;              //吸清洗液X1 试剂针
    int CleanReagentBlood_ul = 0;         //吸清洗液X1 血样针

    quint8 totalcode = 17;
    QByteArray *poutdata = new QByteArray[totalcode];
    auto *pActive = Testing::m_TaskDll;

    //XY移动到血样针试剂位置
    poutdata[0] = pActive->DLL_XYMoveSpecifiedPosition(CleanReagOffBloodPin,0,0,_num);

    //血样针下针 液面探测 falilinquehigh = 探测速度
    poutdata[1] = pActive->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,
                                                      failedBloodpin,
                                                      2,_num,true,
                                                      failedBloodpin,false,GRIPPERNORMAL);

    suckSpitPPPRatio = (suckSpitPPPRatio <= 0)? 1.00 : suckSpitPPPRatio;
    washbloodPinVol =   (washbloodPinVol <= 0)? 250 : washbloodPinVol;

    reagent_total = 1;
    CleanReagentBlood_ul = washbloodPinVol * reagent_total * suckSpitPPPRatio +(0.2*washbloodPinVol) + 720;

    QLOG_DEBUG()<<"吸血样针清洗液=:"<<"单次洗清洗液量 *"<<"吸测试的试剂*"<<"转换比+"<<"0.2*单次洗清洗液量"<<"+" << 720;
    QLOG_DEBUG()<<washbloodPinVol<<"*"<<reagent_total<<"*"<< suckSpitPPPRatio <<"+"<<0.2*washbloodPinVol<<"+"<<720<<"="<<CleanReagentBlood_ul<<endl;

    //洗血样针吸清洗液==吸
    poutdata[2] = pActive->BigBenActive(true,CleanReagentBlood_ul,_num,DIS_WASHES_PUMPS,0);

    //血样针复位
    poutdata[3] = pActive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,_num,false);

    //XY移动到试剂针位置
    poutdata[4] = pActive->DLL_XYMoveSpecifiedPosition(CleanReagOffReagnetPin,0,0,_num);

    //试剂针下针 液面探测 falilinquehigh = 探测速度
    poutdata[5] = pActive->DLL_ZMoveSpecifiedPosition(MOTOR_REAGNET_INDEX,
                                                      failedreagentpin,0,
                                                      _num,true,failedreagentpin,
                                                      false,GRIPPERNORMAL);
    QVector<double> MaxRatio;
    for(int i = 0 ; i < 5; i++){
       MaxRatio.push_back(INI_File().getTypesReagentSuckRatio(i+1));
    }
    auto maxvalue = std::max_element(std::begin(MaxRatio), std::end(MaxRatio));
    double AbsorbRatio = *maxvalue; /*试剂针吸 清洗液的量按Max*/
    CleanReagent_ul = washReagPinVol*(SMALL_BEN_RATIO* AbsorbRatio) + SMALL_BEN_INHALE_ARI/2;
    if(CleanReagent_ul > 7200){
        QLOG_DEBUG()<<"清洗试剂泵超MAX量程,请检验参数:吸试剂针清洗量"<<CleanReagent_ul;
        QLOG_DEBUG()<<"试剂针洗清洗量"<<washReagPinVol<<"*"<<AbsorbRatio<<"*"<<SMALL_BEN_RATIO<<"+"<<"360";
        CleanReagent_ul = 7000;
    }

    //试剂针吸清洗液
    poutdata[6] = pActive->SmallBenActive(true,CleanReagent_ul,DIS_WASHES_PUMPS,_num,0);

    //试剂针复位
    poutdata[7] = pActive->DLL_ZAxis_Reset(MOTOR_REAGNET_INDEX,0,0,_num,false);

    //XY移动到原点清洗位
    poutdata[8] = pActive->DLL_XYMoveSpecifiedPosition(CleanAxis,0,0,_num);

    //血样针下针
    poutdata[9] = pActive->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,CLEANING_DOWNHIGN,0,_num,false,
                                                                CLEANING_DOWNHIGN,false,GRIPPERNORMAL);
    //试剂针下针
    poutdata[10] = pActive->DLL_ZMoveSpecifiedPosition(MOTOR_REAGNET_INDEX,CLEANING_DOWNHIGN,0,_num,
                                                                 false,CLEANING_DOWNHIGN,false,GRIPPERNORMAL);

    //大泵吐到底复位清洗
    poutdata[11] = pActive->BigBenActive(false,0,_num,WASHES_PUMPS,(int)INI_File().getWashesTime());

    //小泵吐到底复位清洗
    poutdata[12] = pActive->SmallBenActive(false,0,WASHES_PUMPS,_num,(int)INI_File().getWashesTime());

    //双针复位
    poutdata[13] = pActive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,_num,false);
    poutdata[14] = pActive->DLL_ZAxis_Reset(MOTOR_REAGNET_INDEX,0,0,_num,false);

    //XY负速度模式复位
    poutdata[15] = pActive->_DLLXYMoveReposition_(_num,MACHINEBACK_SPEED/2);

    //定在原点清洗位
    poutdata[16] = pActive->DLL_XYMoveSpecifiedPosition(CleanAxis,0,0,_num);

    for(int n = 0 ; n < totalcode; n++)
       CleanActionarry.push_back(poutdata[n]);
    delete []poutdata;
    return;
}


//清洗血样针
void QUIUtils::CleanBloodPinActionCommd(int HadPorjects,QByteArrayList &CleanActionarry)
{
    auto &ini =  INI_File();
    const int failedBloodpin =  ini.GetFailedCleanLinqueHigh();//清洗液探测失败血样针
    int washbloodPinVol = ini.GetAbsorbWashingfluidX2();//洗血样针洗的清洗剂量
    double suckSpitPPPRatio =  ini.GetPPPConversionScale();


    //血样针到清洗试剂坐标
    QPoint CleanAxis(0,0),CleanReagOffBloodPin(0,0);
    SingletonAxis::GetInstance()->originPos(READ_OPERRAT,CleanAxis);
    SingletonAxis::GetInstance()->cleanZoneAxisPos(READ_OPERRAT,MOTOR_BLOOD_INDEX,CleanReagOffBloodPin);
    const quint8 totalArrycode =  10;


	suckSpitPPPRatio = (suckSpitPPPRatio == 0)?  2.88: ini.GetPPPConversionScale();
	washbloodPinVol = (washbloodPinVol <= 0)?  1: ini.GetAbsorbWashingfluidX2();
    if(washbloodPinVol <= 0){
        QLOG_WARN()<<"测试时清洗[样本针]吸试剂量<= 0";
    }/*血样针吸清洗挤的量*/
    int CleanReagentBlood_ul = washbloodPinVol*HadPorjects* suckSpitPPPRatio +(0.2*washbloodPinVol);

    //先Z轴全部复位开始
    CleanActionarry.clear();
    quint8 num = CleanActionarry.count()%255;
    CleanActionarry.reserve(totalArrycode);

    auto *pActive = Testing::m_TaskDll;
    CleanActionarry.push_back(pActive->DLL_XYMoveSpecifiedPosition(CleanReagOffBloodPin,0,0,num));
    CleanActionarry.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,failedBloodpin,0,num,
                                                                  true,failedBloodpin,false,GRIPPERNORMAL) );
    CleanActionarry.push_back(pActive->BigBenActive(true,CleanReagentBlood_ul,num,DIS_WASHES_PUMPS,0) );
    CleanActionarry.push_back(pActive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num,false) );
    CleanActionarry.push_back(pActive->DLL_XYMoveSpecifiedPosition(CleanAxis,0,0,num) );
    CleanActionarry.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,CLEANING_DOWNHIGN,0,num,
                                                                  false,CLEANING_DOWNHIGN,false,GRIPPERNORMAL));
    CleanActionarry.push_back(pActive->BigBenActive(false,0,num,WASHES_PUMPS,static_cast<quint32>(INI_File().getWashesTime())) );
    CleanActionarry.push_back(pActive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num,false) );
    CleanActionarry.push_back(pActive->_DLLXYMoveReposition_(num,MACHINEBACK_SPEED/2) );
    CleanActionarry.push_back(pActive->DLL_XYMoveSpecifiedPosition(CleanAxis,0,0,num) );
    return;
}

//清洗试剂针
void QUIUtils::CleanReagentsPinActionCommd(QVector<QByteArray> &CleanActionarry)
{
    QPoint CleanAxis(0,0),CleanReagOffReagnetPin(0,0);      //试剂针到清洗位置
    SingletonAxis::GetInstance()->originPos(READ_OPERRAT,CleanAxis);
    SingletonAxis::GetInstance()->cleanZoneAxisPos(READ_OPERRAT,MOTOR_REAGNET_INDEX,CleanReagOffReagnetPin);
    const int cleanBloodytotal = 10;
    quint8 _num = (cleanBloodytotal + CleanActionarry.count())%255;
    int failedreagentpin = INI_File().getFailedCleanLinqueReagNeedle();  //清洗液探测失败试剂针
    int washReagPinVol = INI_File().GetAbsorbWashingfluidX1();          //洗试剂针吸清洗液的量

    int totalCleanReagentNeedle = 10 ;
    QByteArray *pCleanReagentNeedle = new QByteArray[totalCleanReagentNeedle];

    pCleanReagentNeedle[0] = Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(CleanReagOffReagnetPin,0,0,_num);

    pCleanReagentNeedle[1] = Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_REAGNET_INDEX,failedreagentpin,0,
                                                                _num,true,failedreagentpin,false,GRIPPERNORMAL);
    QVector<double> MaxRatio;
    for(int i = 0 ; i < 5; i++)
       MaxRatio.push_back(INI_File().getTypesReagentSuckRatio(i+1));
    auto maxvalue = std::max_element(std::begin(MaxRatio), std::end(MaxRatio));
    double AbsorbRatio = *maxvalue; /*试剂针吸 清洗液的量按Max*/
    //吸清洗液X1 试剂针
    int CleanReagent_ul = washReagPinVol*(SMALL_BEN_RATIO * AbsorbRatio) + SMALL_BEN_INHALE_ARI/2;

    //试剂针吸清洗液
    pCleanReagentNeedle[2] = Testing::m_TaskDll->SmallBenActive(true,CleanReagent_ul,DIS_WASHES_PUMPS,_num,0);

    //试剂针复位
    pCleanReagentNeedle[3] = Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_REAGNET_INDEX,0,0,_num,false);

    //XY移动到原点清洗位
    pCleanReagentNeedle[4] = Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(CleanAxis,0,0,_num);

    pCleanReagentNeedle[5] = Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_REAGNET_INDEX,CLEANING_DOWNHIGN,0,
                                                      _num,false,CLEANING_DOWNHIGN,false,GRIPPERNORMAL);

    //小泵吐到底复位清洗
    pCleanReagentNeedle[6] = Testing::m_TaskDll->SmallBenActive(false,0,WASHES_PUMPS,_num,(int)INI_File().getWashesTime());

    pCleanReagentNeedle[7] = Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_REAGNET_INDEX,0,0,_num,false);

    pCleanReagentNeedle[8] = Testing::m_TaskDll->_DLLXYMoveReposition_(_num,MACHINEBACK_SPEED/2);

    pCleanReagentNeedle[9] = Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(CleanAxis,0,0,_num);

    for(int n = 0; n < totalCleanReagentNeedle; n++)
        CleanActionarry.push_back(pCleanReagentNeedle[n]);
    delete []pCleanReagentNeedle;
    return ;
}




int QUIUtils::suckPPPEndSplitPPP(QByteArrayList &out_directives,
                                   const QPoint suckPPPLoc,
                                   const QPoint splitPPPloc)
{
    out_directives.clear(); //PPP
    auto &ini = INI_File();
    auto *pActive = Testing::m_TaskDll;

    const double ppp_ratio = ini.GetPPPConversionScale(); //PPP样本系数
    const int fail_retract_height = ini.GetFailedLinqueHigh();//液面探测失败下降高度
    double suck_volume = ini.GetLearnSamplevolume();

    const bool use_air_calibration = ini.rConfigPara(FIRSTSUCKAIRS).toBool(); //吸空气校准
    const int  tube_down_height = static_cast<int>(ini.GetEmptyTubeDownHigh());

    allZAxisBackOrigin(out_directives,out_directives.size());//先Z轴复位
    quint8  num = out_directives.count()%255;

    int suckPPPul = 0;

    out_directives.push_back(pActive->DLL_XYMoveSpecifiedPosition(suckPPPLoc,0,0,num));
    if(use_air_calibration)
    {
        out_directives.push_back(pActive->BigBenActive(true,BIG_BEN_INHALE_ARI/2,num,DIS_WASHES_PUMPS,0));
        suckPPPul = suck_volume * ppp_ratio + BIG_BEN_INHALE_ARI/2;
    }
    else{
        suckPPPul = suck_volume * ppp_ratio + BIG_BEN_INHALE_ARI/2;
    }

    out_directives.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,fail_retract_height,0,num,
                                                                 true,fail_retract_height,false,GRIPPERNORMAL));
    //吸贫血量 + 空气量
    out_directives.push_back(pActive->BigBenActive(true,suckPPPul,num,DIS_WASHES_PUMPS,0)); //吸PPP的量
    //QLOG_DEBUG()<<"suckPPPsetp:"<< suckPPPul<<endl;
    out_directives.push_back(pActive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num,false)); //血样针复位
    out_directives.push_back(pActive->DLL_XYMoveSpecifiedPosition(splitPPPloc,0,0,num));
    out_directives.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,tube_down_height,0,
                                                                 num,false,tube_down_height,false,GRIPPERNORMAL));

    int spitPPPul =  BIG_BEN_INHALE_ARI/2; //吐出贫血剩一半空气
    out_directives.push_back(pActive->BigBenActive(false,spitPPPul,num,DIS_WASHES_PUMPS,0));
    out_directives.push_back(pActive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num,false)); //血样针复位
    return 0;
}



int QUIUtils::SuckPRPandSpitoutPRP(QByteArrayList &out_directives,
                                    int Testheigt,
                                    QPoint suck_bloodypos ,
                                    QList<QPoint> spit_bloody_axisList)
{
    auto &ini = INI_File();
    double  suckSpitPRPRatio    =   ini.getPRPConvertTheratioColumn();//PRP样本系数
    int     suckmiter           =   ini.GetLearnSamplevolume();      //单份血样吸取的样本量
    int     EmptyDownPinHigh    =   ini.GetEmptyTubeDownHigh();      //血样针在空试管区下降高度
    int     The_security_value  =   ini.GetSecurityValue();          //空回值
    const bool  bsuckAir        =   ini.rConfigPara(FIRSTSUCKAIRS).toBool();   //吸空气校准
    const int   firstSuckAir    =   ini._getsuckairsuckPRP() + BIG_BEN_INHALE_ARI/2;
    int     total_              =   spit_bloody_axisList.size();

    quint8 num_ = out_directives.size()%255;
    auto *pActive = Testing::m_TaskDll;

    out_directives.push_back(pActive->DLL_XYMoveSpecifiedPosition(suck_bloodypos,0,0,num_));
    if(bsuckAir)
        out_directives.push_back(pActive->BigBenActive(true,firstSuckAir,num_,DIS_WASHES_PUMPS,0));
    else
        QLOG_DEBUG()<<"PRP加样不吸空气";

    out_directives.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,Testheigt,0,num_,
                                                                 false,Testheigt,false,GRIPPERNORMAL));
    double addSuckReaugent = suckmiter * 0.2;

    //吸取的富血样本量: 样本个数 * 单个量 * 720+360
    int suck_ul = total_ * suckmiter * suckSpitPRPRatio + firstSuckAir + addSuckReaugent;
    int TotalSuck_mm = suck_ul; //吸的总量
    QLOG_DEBUG()<<QString("PRP吸取的量:(%1)[试剂个数==%2]").arg(suck_ul).arg(total_);
    out_directives.push_back(pActive->BigBenActive(true,suck_ul,num_,DIS_WASHES_PUMPS,0));
    out_directives.push_back(pActive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num_,false)); //血样针复位

    //吐富血
    int spit_ul = 0;
    for(int k = 0; k < total_ ; k++)
    {
        QPoint BloodyinEmptyAxis = spit_bloody_axisList.at(k);
        out_directives.push_back(pActive->DLL_XYMoveSpecifiedPosition(BloodyinEmptyAxis,0,0,num_));
        out_directives.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,EmptyDownPinHigh,
                                                                    0,num_,false,EmptyDownPinHigh,false,GRIPPERNORMAL));
        if(k == 0)
        {
            TotalSuck_mm = TotalSuck_mm - (suckmiter * suckSpitPRPRatio + The_security_value);
            spit_ul = TotalSuck_mm;
        }
        else
        {
            TotalSuck_mm = TotalSuck_mm - (suckmiter * suckSpitPRPRatio);
            spit_ul = TotalSuck_mm ;
        }
        QLOG_DEBUG()<<QUIUtils::index_reagent_mapping_reagentName("吐血样试剂",k+1)<<"== ["<<spit_ul<<"setp]"<<endl;
        out_directives.push_back(pActive->BigBenActive(false,spit_ul,num_,DIS_WASHES_PUMPS,0));
        out_directives.push_back(pActive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num_,false)); //血样针复位
    }
    return 1;
}


quint8 QUIUtils::_hansdownheightinnertubetray(quint8 _hole)
{
    const quint8 maxIndex = 3; // 最大允许的托盘索引
    quint8 index_tray = _hole / ONETRAY_TOTALTUBE; // 直接计算区间

    // 边界检查：若超过最大值则归零（或按需求处理）
    if (index_tray > maxIndex) {
        index_tray = 0; // 可根据实际需求调整为其他值或断言
    }
    return INI_File()._gethandsdownheightinTesttray(index_tray);
}


void QUIUtils::ClipAnemiatoTestChn(quint8 indexChn,quint8 anaemia_hole, QVector<QByteArray> &outarry,
                                     QPoint &AnemiainEmptyHoleAxis,quint8 &suckBack,quint8 &splitBack)
{

    quint8 _num = 0 % 255;
    int HandsGripDownHigh = static_cast<int>(INI_File()._gethandsdownheiht(indexChn));//在通道抓贫血高度
    quint8 _downheighthandsintrays = _hansdownheightinnertubetray(anaemia_hole);
    bool bRepeattheGrabCup = INI_File().rConfigPara(REPEATGRABCUP).toBool(); //重抓标志

    auto* axisInstance = SingletonAxis::GetInstance();
    QPoint ChnAxis(0,0);
    axisInstance->testTaryZoneAxisPos(READ_OPERRAT,anaemia_hole,MOTOR_HANDS_INDEX,AnemiainEmptyHoleAxis);
    axisInstance->chnZoneAxisPos(READ_OPERRAT,indexChn,MOTOR_HANDS_INDEX,ChnAxis);

    //抓手到试管盘吸杯后复位
    outarry.clear();
    outarry.reserve(8);
    auto *pActive = Testing::m_TaskDll;
    outarry.push_back(pActive->DLL_XYMoveSpecifiedPosition(AnemiainEmptyHoleAxis,0,0,_num));
    outarry.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,_downheighthandsintrays,0,_num,false,
                                                          _downheighthandsintrays,bRepeattheGrabCup,GRABCUPWITHGRIPPER));
    outarry.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,_downheighthandsintrays,0,_num,false,
                                                         _downheighthandsintrays,bRepeattheGrabCup,GRABCUPWITHGRIPPER));
    suckBack = _num;
    outarry.push_back(pActive->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,_downheighthandsintrays,0,_num,bRepeattheGrabCup));
    outarry.push_back(pActive->DLL_XYMoveSpecifiedPosition(ChnAxis,0,0,_num));
    outarry.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,HandsGripDownHigh,0,_num,false,HandsGripDownHigh,
                                                          false,GRIPPERNORMAL));
    outarry.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,HandsGripDownHigh,0,_num,false,HandsGripDownHigh,
                                                          false,GRIPPERNORMAL));
    splitBack = _num;
    outarry.push_back(pActive->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,HandsGripDownHigh,0,_num,false));
    return;
}




void QUIUtils::initequipmentgrabcups(quint8 index_equipment,QByteArrayList &_throwthecups)
{
    quint8 totalChannel = 0,indexcode,numCode;
    int _throwcupshight =   (int)INI_File()._gethandsdownthrowcpus();
    bool bRepeattheGrabCup = INI_File().rConfigPara(REPEATGRABCUP).toBool(); //重抓标志
    QPoint ChannelAxis(0,0),ThrowAxis(0,0);
    switch(index_equipment)
    {
        case KS600: totalChannel = 4; break;
        case KS800: totalChannel = 8; break;
        case KS1200:totalChannel = 12; break;
        default: QLOG_ERROR()<<"初始化丢杯仪器类型异常"<<__FUNCTION__<<__LINE__<<endl; break;
    }
    SingletonAxis::GetInstance()->throwTubeHolePos(READ_OPERRAT,ThrowAxis);
    for(int n = 0; n < totalChannel; n++){
        indexcode = _throwthecups.size()%255;
        numCode = indexcode;
        SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT,n,MOTOR_HANDS_INDEX,ChannelAxis);

        quint8 totalActive = 6;
        QByteArray *poutArrycode = new QByteArray[totalActive];
        poutArrycode[0] = Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(ChannelAxis,MOTOR_HANDS_INDEX,0,numCode);
        int handsdowninchn =  (int)INI_File()._gethandsdownheiht(n);//抓手在通道下降高度
        poutArrycode[1] = Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,handsdowninchn,0,numCode,false,
                                                                         handsdowninchn,bRepeattheGrabCup,GRABCUPWITHGRIPPER);
        poutArrycode[2] = Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,handsdowninchn,0,numCode,bRepeattheGrabCup);
        poutArrycode[3] = Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(ThrowAxis,0,0,numCode);
        poutArrycode[4] = Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,_throwcupshight,0,
                                                       numCode,false, _throwcupshight,false,GRIPPERNORMAL);
        poutArrycode[5] = Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,_throwcupshight,0,numCode,false);
        for(quint8 k = 0; k < totalActive; k++)
            _throwthecups.push_back(poutArrycode[k]);
        delete []poutArrycode;
    }
    //XY负速度模式复位
    QPoint CleanAxis(0,0);
    SingletonAxis::GetInstance()->originPos(READ_OPERRAT,CleanAxis);
    quint8 num_ = _throwthecups.size()%255;
    _throwthecups.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(CleanAxis,0,0, num_));
    _throwthecups.push_back(Testing::m_TaskDll->_DLLXYMoveReposition_(num_,MACHINEBACK_SPEED/2));
    _throwthecups.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(QPoint(0,0),0,0, num_));
    _throwthecups.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(CleanAxis,0,0, num_));
    return;
}


void QUIUtils::ThrowTestChnCup(quint8 indexChn, QVector<QByteArray> &outarry ,quint8 &issuckBack,quint8 &issplitBack)
{
    quint8 _num = outarry.count() % 255; // 先计算再清空
    outarry.clear();
    const quint8 totalcode = 8;
    outarry.reserve(totalcode);

    // 读取配置（考虑缓存优化）
    int chndownhands = INI_File()._gethandsdownheiht(indexChn);
    int _throwcupheigh = INI_File()._gethandsdownthrowcpus();
    bool bRepeattheGrabCup = INI_File().rConfigPara(REPEATGRABCUP).toBool(); //重抓标志

    // 获取坐标
    QPoint ChnAxis(0, 0), throwAxis(0, 0);
    SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT, indexChn, MOTOR_HANDS_INDEX, ChnAxis);
    SingletonAxis::GetInstance()->throwTubeHolePos(READ_OPERRAT, throwAxis);

    outarry.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(ChnAxis, 0, 0, _num));

    outarry.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX, chndownhands,
                            0, _num, false, chndownhands, bRepeattheGrabCup, GRABCUPWITHGRIPPER));

    outarry.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX, chndownhands,
                            0, _num, false, chndownhands, bRepeattheGrabCup, GRABCUPWITHGRIPPER));

    issuckBack = _num;
    outarry.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX, chndownhands, 0, _num, bRepeattheGrabCup));
    outarry.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(throwAxis, 0, 0, _num));

    outarry.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX, _throwcupheigh, 0, _num,
                                                                     false, _throwcupheigh, false, GRIPPERNORMAL));

    outarry.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX, _throwcupheigh, 0, _num,
                                                                     false, _throwcupheigh, false, GRIPPERNORMAL));
    issplitBack = _num;
    outarry.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX, _throwcupheigh, 0, _num, false));
    return;
}


int QUIUtils::SuckReagentNumberofSteps(const unsigned char suckIndexReagent)
{
    auto &ini = INI_File();
    unsigned char suckReagentVol  =   ini.getTypesReagentSuckVolume(suckIndexReagent);    //吸试剂的量
    double  AbsorbRatio     =   ini.getTypesReagentSuckRatio(suckIndexReagent);     //试剂系数
    double  addSuckRatio    =   ini.getTypesReagentSuckAdd_Ratio(suckIndexReagent); //试剂多吸的比例系数
    const double suckStep =  suckReagentVol * AbsorbRatio * SMALL_BEN_RATIO;

    QLOG_DEBUG()<<"测试试剂类型: "<<GlobalData::mapIndexReagentnames(suckIndexReagent);
    QLOG_DEBUG()<<"预吸测试量:"<<suckReagentVol<<"μL";
    QLOG_DEBUG()<<"实际吸取试剂步数: "<< suckStep<<"+空气量:"<<SMALL_BEN_INHALE_ARI;
    QLOG_DEBUG()<<"实际吸"<<suckStep/SMALL_BEN_RATIO;


    int outMoveStep  = SMALL_BEN_INHALE_ARI + suckStep + suckStep * addSuckRatio;
    (outMoveStep > 7200)? outMoveStep = 7200 : outMoveStep = outMoveStep;
    return outMoveStep;
}





void QUIUtils::suckReagentClipTubetoChnPut(quint8 indexReag,
                                             int TestChn,
                                             quint8 emptyHole,
                                             QVector<QByteArray> &outputArry,
                                             QPoint &bloodyHoleAxis,
                                             quint8 &suckPRPindex,
                                             quint8 &ktestPRPsuckBacknum ,
                                             quint8 &ktestPRPsplitBacknum)
{

    bool SuckAirs = true, bLeveldetection = true,bRepeattheGrabCup,bsuck_air;
    quint8 offsetZNeedle = 0, IndexZone = 0;
    quint8 indexCode = 0%255;
    int setWashTime = 0;
    int _handsdowninchn  = INI_File()._gethandsdownheiht(TestChn); //抓手在通道抓杯
    quint8 hansdowninntotubetray = _hansdownheightinnertubetray(emptyHole);

    //试剂探测失败下降高度
    int  falilinquehigh    =    INI_File().GetFailedReagentsLinqueHigh();
    bRepeattheGrabCup =         INI_File().rConfigPara(REPEATGRABCUP).toBool(); //重抓标志
    bsuck_air       =           INI_File().rConfigPara(FIRSTSUCKAIRS).toBool();    //吸空气校准


    //试剂位置
    quint8 testingReag = 0XFF;
    testingReag = indexReag - 1;
    QPoint AbsorbReaentAxis(0,0),ChnAxis(0,0);
    //试剂坐标从0-9下标开始 indexReag = AA-RIS 0 - 4
    SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT,testingReag,AbsorbReaentAxis);
    SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,emptyHole,MOTOR_HANDS_INDEX,bloodyHoleAxis);
    SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT,TestChn,MOTOR_HANDS_INDEX,ChnAxis);

    auto *pActive = Testing::m_TaskDll;
    outputArry.clear();
    quint8 totalnum = (bsuck_air)?  13: 12;
    outputArry.reserve(totalnum);


    outputArry.push_back(pActive->DLL_XYMoveSpecifiedPosition(AbsorbReaentAxis,offsetZNeedle,IndexZone,indexCode));

    if(bsuck_air){
        outputArry.push_back(pActive->SmallBenActive(SuckAirs,
                                                     SMALL_BEN_INHALE_ARI,
                                                     DIS_WASHES_PUMPS,
                                                     indexCode,
                                                     setWashTime
                                                     )
                             );
    }

    outputArry.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_REAGNET_INDEX,falilinquehigh,IndexZone,
                                                           indexCode,bLeveldetection,falilinquehigh,false,GRIPPERNORMAL));
    //总步数吸试剂
    int TotalSuckStep = SuckReagentNumberofSteps(indexReag);

    outputArry.push_back(pActive->SmallBenActive(SuckAirs,
                                                 TotalSuckStep,
                                                 DIS_WASHES_PUMPS,
                                                 indexCode,
                                                 setWashTime)
                         );

    outputArry.push_back(pActive->DLL_ZAxis_Reset(MOTOR_REAGNET_INDEX,0,0,indexCode,false));

    suckPRPindex = indexCode;
    outputArry.push_back(pActive->DLL_XYMoveSpecifiedPosition(bloodyHoleAxis,0,0,indexCode));
    outputArry.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,hansdowninntotubetray,0,indexCode,false,
                                                             hansdowninntotubetray,bRepeattheGrabCup,GRABCUPWITHGRIPPER));
    outputArry.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,hansdowninntotubetray,0,indexCode,false,
                                                    hansdowninntotubetray,bRepeattheGrabCup,GRABCUPWITHGRIPPER));

    ktestPRPsuckBacknum = indexCode;
    outputArry.push_back(pActive->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,hansdowninntotubetray,0,
                                                               indexCode,bRepeattheGrabCup));
    outputArry.push_back(pActive->DLL_XYMoveSpecifiedPosition(ChnAxis,0,0,indexCode));
    outputArry.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,_handsdowninchn,0,
                                                             indexCode,false,_handsdowninchn,false,GRIPPERNORMAL));
    outputArry.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,_handsdowninchn,0,
                                                             indexCode,false,_handsdowninchn,false,GRIPPERNORMAL));
    ktestPRPsplitBacknum = indexCode;
    outputArry.push_back(pActive->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX, falilinquehigh,0,indexCode,false));
    return;
}


void QUIUtils::spitReanentToTestChn(const int ChnIndex,quint8 indexReagent, QVector<QByteArray> &SpitReagentActive)
{
    quint8 conde_num = 0;

    quint8 spit_high = INI_File().getTypesReagentNeedleDownHigh(indexReagent);

    QLOG_DEBUG()<<"试剂"<<indexReagent<<"在测试通道下降高度"<<spit_high;
    SpitReagentActive.reserve(SpitReagentActive.size() + 4);

    constexpr int halfBenInhale = BIG_BEN_INHALE_ARI/2;
    QPoint Channelpos(0,0);

    SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT, ChnIndex, MOTOR_REAGNET_INDEX, Channelpos);

    // 使用 emplace_back 减少临时对象（若 QVector 支持）
    SpitReagentActive.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(Channelpos, 0, 0, conde_num));
    SpitReagentActive.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_REAGNET_INDEX, spit_high, 0, conde_num,
                                                                               false, spit_high, false, GRIPPERNORMAL));

    SpitReagentActive.push_back(Testing::m_TaskDll->SmallBenActive(false, halfBenInhale, DIS_WASHES_PUMPS, conde_num, 0));
    SpitReagentActive.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_REAGNET_INDEX, 0, 0, conde_num, false));

    return;
}


void QUIUtils::suckCupMoveChannelPut(const QPoint GrriperSuckHoleAxis,
                                        const QPoint channelPutAxis,
                                        quint8 gripperDown,
                                        QByteArrayList &grippersuckCup2Channel,
                                        quint8 &sucknum,
                                        quint8 &splitnum){


    quint8 num = 0;
    grippersuckCup2Channel.clear();
    grippersuckCup2Channel.reserve(6);
    auto *pActive = Testing::m_TaskDll;
    grippersuckCup2Channel.push_back(pActive->DLL_XYMoveSpecifiedPosition(GrriperSuckHoleAxis,0,0,num));

    sucknum = num;
    grippersuckCup2Channel.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,gripperDown,0,num,
                                                                      false,gripperDown,false,GRABCUPWITHGRIPPER));
    grippersuckCup2Channel.push_back(pActive->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,0,0,num,false));

    grippersuckCup2Channel.push_back(pActive->DLL_XYMoveSpecifiedPosition(channelPutAxis,0,0,num));

    splitnum = num;
    grippersuckCup2Channel.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,gripperDown,0,num,
                                                                        false,gripperDown,false,GRIPPERNORMAL));
    grippersuckCup2Channel.push_back(pActive->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,0,0,num,false));
}

void QUIUtils::ThrowChannelCup(quint8 downsuck, const QPoint channelAxis,
                                const QPoint throwCupAxis, quint8 &throwSucknum,
                                quint8 &throwSplitnum,QByteArrayList &throwCupsList){
    quint8 num = 0;
    throwCupsList.clear();
    throwCupsList.reserve(6);
    auto *pActive = Testing::m_TaskDll;
    throwCupsList.push_back(pActive->DLL_XYMoveSpecifiedPosition(channelAxis,0,0,num));

    throwSucknum = num;
    throwCupsList.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,downsuck,0,num,
                                                                      false,downsuck,false,GRABCUPWITHGRIPPER));
    throwCupsList.push_back(pActive->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,0,0,num,false));

    throwCupsList.push_back(pActive->DLL_XYMoveSpecifiedPosition(throwCupAxis,0,0,num));

    throwSplitnum = num;
    throwCupsList.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,downsuck,0,num,
                                                                        false,downsuck,false,GRIPPERNORMAL));
    throwCupsList.push_back(pActive->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,0,0,num,false));
}


void QUIUtils::qualitysucksampleact(QPoint suckSampleAxis,QPoint splitSampleAxis,
                                      QByteArrayList &outdataList)
{
    //吸蒸馏水、标准物质、稀释标准物质
    auto &ini = INI_File();
    int LeveldetectionFailed = ini.GetFailedLinqueHigh();    //液面探测失败下降高度
    int suckmiter = ini.GetLearnSamplevolume();              //吸贫血样本量
    double  proportionalValue = ini.GetPPPConversionScale(); //转换比例 样本系数
    int testTubeDownHigh = ini.GetEmptyTubeDownHigh();      /*血样针在空试管区下降高度*/

    outdataList.clear();
    outdataList.reserve(9);
    quint8 num = 0;
    auto *pActive = Testing::m_TaskDll;

    outdataList.push_back(pActive->DLL_XYMoveSpecifiedPosition(suckSampleAxis,0,0,num));
    outdataList.push_back(pActive->BigBenActive(true,BIG_BEN_INHALE_ARI,num,DIS_WASHES_PUMPS,0));
    outdataList.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,LeveldetectionFailed,0,num,
                                                                true,LeveldetectionFailed,false,GRIPPERNORMAL));
    //吸贫血量 + 空气量
    int suck_ul = suckmiter * proportionalValue + BIG_BEN_INHALE_ARI;
    outdataList.push_back(pActive->BigBenActive(true,suck_ul,num,DIS_WASHES_PUMPS,0)); //吸样量
    outdataList.push_back(pActive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num,false)); //血样针复位


    outdataList.push_back(pActive->DLL_XYMoveSpecifiedPosition(splitSampleAxis,0,0,num));
    outdataList.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,testTubeDownHigh,0,num,false,
                                                                testTubeDownHigh,false,GRIPPERNORMAL));
    int spit_ul =  BIG_BEN_INHALE_ARI/2; //吐出贫血剩一半空气
    outdataList.push_back(pActive->BigBenActive(false,spit_ul,num,DIS_WASHES_PUMPS,0));
    outdataList.push_back(pActive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num,false)); //血样针复位
}


void QUIUtils::_mapConsumablesName(quint8 index_, QString& out_)
{
    // 定义静态映射表，存储索引与名称的对应关系
    static const QHash<quint8, QString> consumableMap = {
        {INDEX_AA_CONSUMABLE,     "消耗试剂AA(L)"},
        {INDEX_AA_1_CONSUMABLE,   "消耗试剂AA(R)"},
        {INDEX_ADP_CONSUMABLE,    "消耗试剂ADP(L)"},
        {INDEX_ADP_1_CONSUMABLE,  "消耗试剂ADP(R)"},
        {INDEX_EPI_CONSUMABLE,    "消耗试剂EPI(L)"},
        {INDEX_EPI_1_CONSUMABLE,  "消耗试剂EPI(R)"},
        {INDEX_COL_CONSUMABLE,    "消耗试剂COL(L)"},
        {INDEX_COL_1_CONSUMABLE,  "消耗试剂COL(R)"},
        {INDEX_RIS_CONSUMABLE,    "消耗试剂RIS(L)"},
        {INDEX_RIS_1_CONSUMABLE,  "消耗试剂RIS(R)"},
        {INDEX_CLEANLINQUE_CONSUMABLE, "消耗S1清洗液"},
        {INDEX_TESTTUBE_CONSUMABLE,    "测试管盘"},
        {OUTSIDE_CLEANLINQUE_S2,       "S2外部清洗液"}
    };

    // 查找映射表，未找到则返回默认值
    auto it = consumableMap.constFind(index_);
    out_ = (it != consumableMap.constEnd()) ? *it : "无效试剂下标";
}


QByteArrayList QUIUtils::_controltestbloodpindownheigh(int  index_,int down_mm)
{
    QByteArrayList out_directives;
    out_directives.clear();
    allZAxisBackOrigin(out_directives,out_directives.size());     //先Z轴复位
    quint8  num_ = out_directives.count()%255;                           /*命令下标*/
    if(index_ == BLOODPINDOWNHEIGH)
    {
        //血样针下降高度
        QPoint topleft_(0,0);
        QPoint topright_(0,0);
        QPoint bottomleft_(0,0);
        QPoint bottmright_(0,0);
        SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,0,MOTOR_BLOOD_INDEX,topleft_);
        SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,5,MOTOR_BLOOD_INDEX,topright_);
        SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,54,MOTOR_BLOOD_INDEX,bottomleft_);
        SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,59,MOTOR_BLOOD_INDEX,bottmright_);
        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(topleft_,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,down_mm,0,
                                                                                num_,false,down_mm,false,GRIPPERNORMAL));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num_,false));

        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(topright_,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,down_mm,0,num_,
                                                                                false,down_mm,false,GRIPPERNORMAL));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num_,false));

        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(bottomleft_,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,down_mm,0,num_,
                                                                                false,down_mm,false,GRIPPERNORMAL));

        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num_,false));

        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(bottmright_,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,down_mm,0,num_,
                                                                                false,down_mm,false,GRIPPERNORMAL));

        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num_,false));
    }
    else if(BLOODPINDOWNHEIGH_CLEANLINQUEFAILED == index_)
    {
        //血样针液面探测失败下降高度(清洗液)
        QPoint cleanzoneoffBloodNedl(0,0);
        SingletonAxis::GetInstance()->cleanZoneAxisPos(READ_OPERRAT,MOTOR_BLOOD_INDEX ,cleanzoneoffBloodNedl);
        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(cleanzoneoffBloodNedl,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,down_mm,0,num_,false,
                                                                                down_mm,false,GRIPPERNORMAL));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num_,false));
    }
    else if(BLOODPINDOWNHEIGH_SERUMMODEL == index_)
    {
        //血样针血浆模式下针高度
        QPoint bloodSamplePos(0,0);
        SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT,1,bloodSamplePos);
        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(bloodSamplePos,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,down_mm,0,num_,
                                                                                false,down_mm,false,GRIPPERNORMAL));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num_,false));

    }
    else if(BLOODPINDOWNHEIGH_ANEMIALINQUEFAILED == index_)
    {
        //血样针液面探测失败下降高度(贫血)
        QPoint bloodSamplePos(0,0);
        SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT,0,bloodSamplePos);
        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(bloodSamplePos,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,down_mm,0,num_,
                                                                                false,down_mm,false,GRIPPERNORMAL));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num_,false));

    }
    QPoint _getoriginpos(0,0);
    SingletonAxis::GetInstance()->originPos(READ_OPERRAT,_getoriginpos);
    out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(QPoint(0,0),0,0, num_));
    out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(_getoriginpos,0,0, num_));
    return out_directives;
}

QByteArrayList QUIUtils::_controlReagpindownheih(int index_,int down_mm)
{
    QByteArrayList out_directives;
    out_directives.clear();
    allZAxisBackOrigin(out_directives,out_directives.size());     //先Z轴复位
    quint8  num_ = out_directives.count()%255;                           /*命令下标*/
    if(index_ == REAGPIN_CLEANLINQUE_DOWN)
    {
        QPoint cleanzoneoffreagpin(0,0);
        SingletonAxis::GetInstance()->cleanZoneAxisPos(READ_OPERRAT,MOTOR_REAGNET_INDEX ,cleanzoneoffreagpin);
        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(cleanzoneoffreagpin,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_REAGNET_INDEX,down_mm,0,num_,
                                                                                false,down_mm,false,GRIPPERNORMAL));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_REAGNET_INDEX,0,0,num_,false));
    }
    else if(index_ == REAGPIN_REAGLINQUE_FAILEDDOWN)
    {
        QPoint reagpinoffsetreagloc(0,0);
        SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT,0 ,reagpinoffsetreagloc);
        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(reagpinoffsetreagloc,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_REAGNET_INDEX,down_mm,0,
                                                                                num_,false,down_mm,false,GRIPPERNORMAL));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_REAGNET_INDEX,0,0,num_,false));
    }
    else if(index_ >= REAGPIN_DOWNHEIGH_IN_AA && index_ <= REAGPIN_DOWNHEIGH_IN_RIS)
    {
        QPoint chnPos(0,0);
        SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT, 0 ,MOTOR_REAGNET_INDEX,chnPos);
        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(chnPos,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_REAGNET_INDEX,down_mm,0,num_,
                                                                                false,down_mm,false,GRIPPERNORMAL));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_REAGNET_INDEX,0,0,num_,false));
    }
    QPoint _getoriginpos(0,0);
    SingletonAxis::GetInstance()->originPos(READ_OPERRAT,_getoriginpos);
    out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(QPoint(0,0),0,0, num_));
    out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(_getoriginpos,0,0, num_));
    return out_directives;
}

QByteArrayList QUIUtils::_controlHandsdowntest(int index_,int down_mm)
{
    QByteArrayList out_directives;
    out_directives.clear();
    allZAxisBackOrigin(out_directives,out_directives.size());     //先Z轴复位
    quint8  num_ = out_directives.count()%255;                           /*命令下标*/
    QPoint throwcuphole(0,0);
    SingletonAxis::GetInstance()->throwTubeHolePos(READ_OPERRAT,throwcuphole);
    uint throwbak_y = std::abs(throwcuphole.y() - PUTTUBEMOVE);
    QPoint throwHolePos(throwcuphole.x(),throwbak_y);
    if(index_ == HANDSDOWN_THROWCUP)
    {
        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(throwcuphole,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,down_mm,0,num_,
                                                                                false,down_mm,false,GRIPPERNORMAL));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,0,0,num_,false));
    }
    else if(index_ >= HANDSDOWN_TRAYTUBE_1 && index_ <= HANDSDOWN_TRAYTUBE_4)
    {
        QPoint topleft_(0,0);
        QPoint topright_(0,0);
        QPoint bottomleft_(0,0);
        QPoint bottmright_(0,0);
        switch(index_)
        {
        case HANDSDOWN_TRAYTUBE_1:
        {
            SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,0,MOTOR_BLOOD_INDEX,topleft_);
            SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,5,MOTOR_BLOOD_INDEX,topright_);
            SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,54,MOTOR_BLOOD_INDEX,bottomleft_);
            SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,59,MOTOR_BLOOD_INDEX,bottmright_);
            break;
        }
        case HANDSDOWN_TRAYTUBE_2:
        {
            SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,60,MOTOR_BLOOD_INDEX,topleft_);
            SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,65,MOTOR_BLOOD_INDEX,topright_);
            SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,54 +60,MOTOR_BLOOD_INDEX,bottomleft_);
            SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,59 +60,MOTOR_BLOOD_INDEX,bottmright_);
            break;
        }
        case HANDSDOWN_TRAYTUBE_3:
        {
            SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,0+120,MOTOR_BLOOD_INDEX,topleft_);
            SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,5+120,MOTOR_BLOOD_INDEX,topright_);
            SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,54+120,MOTOR_BLOOD_INDEX,bottomleft_);
            SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,59+120,MOTOR_BLOOD_INDEX,bottmright_);
            break;
        }
        case HANDSDOWN_TRAYTUBE_4:
        {
            SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,0+180,MOTOR_BLOOD_INDEX,topleft_);
            SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,5+180,MOTOR_BLOOD_INDEX,topright_);
            SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,54+180,MOTOR_BLOOD_INDEX,bottomleft_);
            SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,59+180,MOTOR_BLOOD_INDEX,bottmright_);
            break;
        }
        default: break;
        }


        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(topleft_,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,down_mm,0,num_,
                                                                                false,down_mm,false,GRABCUPWITHGRIPPER));

        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,0,0,num_,false));
        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(throwcuphole,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,INI_File()._gethandsdownthrowcpus(),
                                                                                0,num_,false,INI_File()._gethandsdownthrowcpus(),false,GRIPPERNORMAL));

        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(throwHolePos,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,INI_File()._gethandsdownthrowcpus(),0, num_,false));

        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(topright_,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,down_mm,0,num_,
                                                                                false,down_mm,false,GRABCUPWITHGRIPPER));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,0,0,num_,false));
        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(throwcuphole,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,INI_File()._gethandsdownthrowcpus(),
                                                                                0,num_,false,INI_File()._gethandsdownthrowcpus(),false,GRIPPERNORMAL));
        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(throwHolePos,0,0, num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,INI_File()._gethandsdownthrowcpus(),0, num_,false));

        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(bottomleft_,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,down_mm,0,
                                                                                num_,false,down_mm,false,GRABCUPWITHGRIPPER));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,0,0,num_,false));
        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(throwcuphole,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,INI_File()._gethandsdownthrowcpus(),
                                                                                0,num_,false,INI_File()._gethandsdownthrowcpus(),false,GRIPPERNORMAL));
        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(throwHolePos,0,0, num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,INI_File()._gethandsdownthrowcpus(),0, num_,false));

        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(bottmright_,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,down_mm,0,num_,false,down_mm,false,GRABCUPWITHGRIPPER));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,0,0,num_,false));
        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(throwcuphole,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,INI_File()._gethandsdownthrowcpus(),
                                                                                0,num_,false,INI_File()._gethandsdownthrowcpus(),false,GRIPPERNORMAL));
        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(throwHolePos,0,0, num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,INI_File()._gethandsdownthrowcpus(),0, num_,false));
    }
    else if(index_ >= HANDSDOWN_CHN_1 && index_ <= HANDSDOWN_CHN_12)
    {
        QPoint _chnpos(0,0);
        int chn_ = index_ - HANDSDOWN_CHN_1;
        SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT,chn_,MOTOR_HANDS_INDEX,_chnpos);
        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(_chnpos,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,down_mm,0,num_,false,down_mm,false,GRABCUPWITHGRIPPER));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,0,0,num_,false));

        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(throwcuphole,0,0,num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,INI_File()._gethandsdownthrowcpus(),
                                                                                0,num_,false,INI_File()._gethandsdownthrowcpus(),false,GRIPPERNORMAL));
        out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(throwHolePos,0,0, num_));
        out_directives.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,INI_File()._gethandsdownthrowcpus(),0, num_,false));
    }

    QPoint _getoriginpos(0,0);
    SingletonAxis::GetInstance()->originPos(READ_OPERRAT,_getoriginpos);
    out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(QPoint(0,0),0,0, num_));
    out_directives.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(_getoriginpos,0,0, num_));
    return out_directives;
}



void QUIUtils::ReplaceSuckReagentLoc(QByteArrayList &SuckReagArry, quint8 indexReag,double LastRatio)
{
    quint8 value_limit = 0;
    ConsumablesOper::GetpInstance()->updateReagentLimit(READ_OPERRAT, indexReag ,value_limit);
    QLOG_DEBUG()<<"试剂"<<indexReag<<"余比例"<<LastRatio<<"报警线"<<value_limit<<endl;
    if(LastRatio <= value_limit)
    {
          QPoint AbsorbReaentAxis(0, 0);
          quint8 _num = 0%255;
          QByteArray  getSuckReagArry = SuckReagArry.takeFirst();
          QLOG_DEBUG()<<getSuckReagArry.toHex(' ').trimmed().toUpper()<<"=替换试剂="<<getSuckReagArry.size()<<endl;
          //试剂坐标从0下标开始
          SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT, indexReag - 1 + 5, AbsorbReaentAxis);
          QByteArray  ReplaceArrySuck = Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(AbsorbReaentAxis, 0, 0, _num);
          SuckReagArry.push_front(ReplaceArrySuck);
    }
    return;
}


//控制PE 加标准物质 高 中 低 4个试杯
void QUIUtils::controlPEAddBasicLinque(QByteArrayList &directives,const QPoint &suckAxis,
                                         const QPoint &standardSolutions,
                                         const QPoint &PEHeighValAxis,
                                         const QPoint &PEMidValAxis,
                                         const QPoint &PELowValAxis){


    auto &ini = INI_File();
    auto *pActive = Testing::m_TaskDll;
    directives.clear();
    directives.reserve(39);

    const double pppRatio = ini.GetPPPConversionScale(); //PPP样本系数
    const int    failRetractHeight = ini.GetFailedLinqueHigh();//液面探测失败下降高度
    const int    tubeDownHeight = static_cast<int>(ini.GetEmptyTubeDownHigh()); //试管上向下吐出的高度


    //先Z轴复位
    allZAxisBackOrigin(directives,directives.size());
    quint8  num = directives.count()%255;

    auto addLinqueStep = [&](int volume, const QPoint &targetAxis) {
         directives.append(pActive->DLL_XYMoveSpecifiedPosition(suckAxis, 0, 0, num));
         directives.push_back(pActive->BigBenActive(true,BIG_BEN_INHALE_ARI,num,DIS_WASHES_PUMPS,0));
         directives.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,failRetractHeight,0,num,
                                                                  true,failRetractHeight,false,GRIPPERNORMAL));
         directives.push_back(pActive->BigBenActive(true,volume,num,DIS_WASHES_PUMPS,0));
         directives.push_back(pActive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num,false));


         directives.push_back(pActive->DLL_XYMoveSpecifiedPosition(targetAxis,0,0,num));
         directives.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,
                                                                  tubeDownHeight,0,
                                                                  num,false,tubeDownHeight,
                                                                  false,GRIPPERNORMAL));
         directives.push_back(pActive->BigBenActive(false,BIG_BEN_INHALE_ARI/2,num,DIS_WASHES_PUMPS,0));
         directives.push_back(pActive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num,false));
    };

    addLinqueStep(200* pppRatio + BIG_BEN_INHALE_ARI,  standardSolutions);
    //QLOG_DEBUG()<<"第一个PE量= (200"<<"*"<<pppRatio<<"+"<<BIG_BEN_INHALE_ARI<<")="<<200* pppRatio + BIG_BEN_INHALE_ARI;
    addLinqueStep(50 * pppRatio + BIG_BEN_INHALE_ARI,  PEHeighValAxis);
    //QLOG_DEBUG()<<"第二个PE量= (50"<<"*"<<pppRatio<<"+"<<BIG_BEN_INHALE_ARI<<")="<<200* pppRatio + BIG_BEN_INHALE_ARI;
    addLinqueStep(100* pppRatio + BIG_BEN_INHALE_ARI,  PEMidValAxis);
    //QLOG_DEBUG()<<"第三个PE量= (50"<<"*"<<pppRatio<<"+"<<BIG_BEN_INHALE_ARI<<")="<<200* pppRatio + BIG_BEN_INHALE_ARI;
    addLinqueStep(150* pppRatio + BIG_BEN_INHALE_ARI,  PELowValAxis);
    //QLOG_DEBUG()<<"第四个PE量= (50"<<"*"<<pppRatio<<"+"<<BIG_BEN_INHALE_ARI<<")="<<200* pppRatio + BIG_BEN_INHALE_ARI;
    return;
}

void QUIUtils::testPEWaterSuckSplit(QByteArrayList &outDirectives,const QPoint &suckWaterAxis,
                                    const QPoint &splitWaterAxis,
                                    const QPoint &PEHeighValAxis,
                                    const QPoint &PEMidValAxis,
                                    const QPoint &PELowValAxis)
{
    outDirectives.reserve(39);
    auto &ini = INI_File();
    auto *pActive = Testing::m_TaskDll;

    const double ppp_ratio = ini.GetPPPConversionScale(); //PPP样本系数
    const int failTestDownHeight = ini.GetFailedLinqueHigh();//液面探测失败下降高度

    const int suckWaterBasic = 200.00 * ppp_ratio + BIG_BEN_INHALE_ARI;
    const int suckWaterAddHeighVal = 150.00* ppp_ratio + BIG_BEN_INHALE_ARI;
    const int suckWaterAddMidVal = 100.00* ppp_ratio + BIG_BEN_INHALE_ARI;
    const int suckWaterAddLowVal = 50.00* ppp_ratio + BIG_BEN_INHALE_ARI;

    //QLOG_DEBUG()<<"第1个水量= (200"<<"*"<<ppp_ratio<<"+"<<BIG_BEN_INHALE_ARI<<")="<<200* ppp_ratio + BIG_BEN_INHALE_ARI;
    //QLOG_DEBUG()<<"第2个水量= (150"<<"*"<<ppp_ratio<<"+"<<BIG_BEN_INHALE_ARI<<")="<<150* ppp_ratio + BIG_BEN_INHALE_ARI;
    //QLOG_DEBUG()<<"第3个水量= (100"<<"*"<<ppp_ratio<<"+"<<BIG_BEN_INHALE_ARI<<")="<<100* ppp_ratio + BIG_BEN_INHALE_ARI;
    //QLOG_DEBUG()<<"第4个水量= (50"<<"*"<<ppp_ratio<<"+"<<BIG_BEN_INHALE_ARI<<")="<<50* ppp_ratio + BIG_BEN_INHALE_ARI;


    const int  tubeDownHeight = static_cast<int>(ini.GetEmptyTubeDownHigh());

    allZAxisBackOrigin(outDirectives,outDirectives.size());//先Z轴复位
    quint8  num = outDirectives.count()%255;

    outDirectives.push_back(pActive->DLL_XYMoveSpecifiedPosition(suckWaterAxis,0,0,num));
    outDirectives.push_back(pActive->BigBenActive(true,BIG_BEN_INHALE_ARI,num,DIS_WASHES_PUMPS,0));
    outDirectives.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,failTestDownHeight,0,num,
                                                                 true,failTestDownHeight,false,GRIPPERNORMAL));
    outDirectives.push_back(pActive->BigBenActive(true,suckWaterBasic,num,DIS_WASHES_PUMPS,0));
    outDirectives.push_back(pActive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num,false)); //血样针复位

    outDirectives.push_back(pActive->DLL_XYMoveSpecifiedPosition(splitWaterAxis,0,0,num));
    outDirectives.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,tubeDownHeight,0,
                                                                 num,false,tubeDownHeight,false,GRIPPERNORMAL));
    outDirectives.push_back(pActive->BigBenActive(false,BIG_BEN_INHALE_ARI/2,num,DIS_WASHES_PUMPS,0));
    outDirectives.push_back(pActive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num,false)); //血样针复位

    auto addLinqueStep = [&](int volume, const QPoint &targetAxis) {
         outDirectives.append(pActive->DLL_XYMoveSpecifiedPosition(suckWaterAxis, 0, 0, num));
         outDirectives.push_back(pActive->BigBenActive(true,BIG_BEN_INHALE_ARI,num,DIS_WASHES_PUMPS,0));
         outDirectives.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,failTestDownHeight,0,num,
                                                                  true,failTestDownHeight,false,GRIPPERNORMAL));
         outDirectives.push_back(pActive->BigBenActive(true,volume,num,DIS_WASHES_PUMPS,0));
         outDirectives.push_back(pActive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num,false));


         outDirectives.push_back(pActive->DLL_XYMoveSpecifiedPosition(targetAxis,0,0,num));
         outDirectives.push_back(pActive->DLL_ZMoveSpecifiedPosition(MOTOR_BLOOD_INDEX,
                                                                  tubeDownHeight,0,
                                                                  num,false,tubeDownHeight,
                                                                  false,GRIPPERNORMAL));
         outDirectives.push_back(pActive->BigBenActive(false,BIG_BEN_INHALE_ARI/2,num,DIS_WASHES_PUMPS,0));
         outDirectives.push_back(pActive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0,num,false));
    };
    addLinqueStep(suckWaterAddHeighVal,  PEHeighValAxis);
    addLinqueStep(suckWaterAddMidVal, PEMidValAxis);
    addLinqueStep(suckWaterAddLowVal, PELowValAxis);
    return;
}
