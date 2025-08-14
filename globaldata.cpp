#pragma execution_character_set("utf-8")
#include "globaldata.h"
#include "QsLog/include/QsLog.h"
#include <QCoreApplication>
#include <loadequipmentpos.h>
#include <QDir>
#include <cmath>
#include <QTextStream>
#include <QLineEdit>
#include <QDateTime>
#include "cglobal.h"
#include "ini_file.h"
#include "quiutils.h"
#include "transformeddata.h"
#include <QDateTime>
#include <QHeaderView>
#include <QScrollBar>

GlobalData::GlobalData()
{

}




void  GlobalData::QCommboxSheet(QComboBox *pcommbox)
{
    QString css_ = "QComboBox{border-radius:5px; padding:2px 4px; font: 19 17pt '楷体'; border: 2px solid rgba(220,220,220);background-color:rgba(188,187,183);}"
                           "QComboBox::drop-down {subcontrol-origin: padding;subcontrol-position: top right;width: 15px; border:none;}"
                           "QComboBox::down-arrow {height:35px; width:30px; image: url(:/Picture/minus.png);}"
                           "QComboBox QAbstractItemView{background:rgba(255,255,255,1);border:1px solid rgba(150,150,150,1);border-radius:0px 0px 5px 5px;font: 24px; font-family: 楷体;outline: 0px;}"
                           "QComboBox QAbstractItemView::item{ height:35px; color:#666666; padding-left:9px; background-color:#FFFFFF;}"
                           "QComboBox QAbstractItemView::item:hover{background-color:#409CE1;color:#ffffff;}"
                           "QComboBox QAbstractItemView::item:selected{background-color:#409CE1;color:#ffffff;}"
                           "QComboBox:on { padding-top: 3px;padding-left: 3px;}"
                           "QComboBox::down-arrow:on {top: 1px;left:3px;}";
    pcommbox->setStyleSheet(css_);
    pcommbox->update();
    return;
}

void GlobalData::QLableSheet(QLabel *plable){
    plable->setStyleSheet(
        "QLabel {"
        "   background-color: qradialgradient(cx:0.5, cy:0.5, radius: 0.6,"
        "       fx:0.5, fy:0.5, stop:0 #FFFFFF, stop:1 #F5F5F5);"
        "   border: 3px solid qconicalgradient(cx:0.5, cy:0.5, angle:90,"
        "       stop:0 #4CAF50, stop:0.5 #2196F3, stop:1 #2196F3);"
        "   border-radius: 15px;"
        "   padding: 15px;"
        "   color: #212121;"
        "   font: bold 14px '楷体';"
        "   min-width: 100px;"
        "   min-height: 20px;"
        "}"
        "QLabel:hover {"
        "   border: 3px solid #2196F3;"
        "   box-shadow: 0 0 15px rgba(33,150,243,0.3);"
        "}"
        "QLabel:pressed {"
        "   background-color: #E0E0E0;"
        "}"
    );
}

void GlobalData::QLineEditSheet(QLineEdit *PLineEdit)
{
    QString  css_ = "QLineEdit:hover {border: 1px solid #298DFF;border-radius: 3px;background-color: #F2F2F2;color: "
                    "#298DFF;selection-background-color: #298DFF;selection-color: #F2F2F2;}"
                    "QLineEdit[echoMode = '2'] { lineedit-password-character: 9679;lineedit-password-mask-delay: 2000;}"
                    "QLineEdit{border: 1px solid #298DFF;background-color: #F2F2F2;}"
                    "QLineEdit:disabled {border: 1px solid #CDCDCD;background-color: #CDCDCD;color: #B4B4B4; }"
                    "QLineEdit:read-only { background-color: #CDCDCD;color: #F2F2F2;}";
    PLineEdit->setStyleSheet(css_);
    PLineEdit->update();                                                                                                                                                                                                                                                                                                                                                   "QLineEdit:disabled {border: 1px solid #CDCDCD;background-color: #CDCDCD;color: #B4B4B4;}QLineEdit:read-only { /* QLineEdit在只读时的状态 */background-color: #CDCDCD;color: #F2F2F2;}";
}


void GlobalData::QCheckboxSheet(QCheckBox *pbox,const QString name)
{
   QString css_ = "QCheckBox::indicator {width:32px; height: 32px;}"
                  "QCheckBox::indicator::unchecked {image: url(:/Picture/check_box_unchecked.png);width:32px;height: 32px;}"
                  "QCheckBox::indicator::checked {image: url(:/Picture/check-box-checked.png);width:32px;height: 32px;}"
                  "QCheckBox::indicator:enabled:unchecked:hover {image: url(:/Picture/check_box_hove.png);width:32px;height: 32px;}";
   pbox->setStyleSheet(css_);
   pbox->setText(name);
   pbox->update();
   return;
}

void GlobalData::QCheckboxSheetChnstate(QCheckBox *pbox,const QString name)
{
    QString css_ ="QCheckBox::indicator {width: 64px; height: 64px;color: rgb(0, 0, 0);}"
                  "QCheckBox::indicator:unchecked {image: url(:/Picture/SetPng/closechannel.png);width: 64px; height: 64px;}"
                  "QCheckBox::indicator:checked {image: url(:/Picture/SetPng/openchannel.png);width: 64px; height: 64px;}";

    pbox->setStyleSheet(css_);
    pbox->setText(name);
    pbox->update();
    return;
}

void GlobalData::mainseledview(QToolButton *ptoolbtn,bool checked)
{
    QString Css_;
    if(checked)
    {
        Css_ = "QToolButton{border: 2px solid rgb(0, 250, 0);background-color:rgba(205, 201, 201);border-radius: 10px; width: 80px;  height:60px;  padding:0 0px;  font-family:'楷体';font-size:22px; background: transparent;}"
               "QToolButton:hover{border: 2px solid rgb(0, 250, 0);background-color: rgb(250, 250, 250); border-style: solid;border-radius:10px;  width: 80px; height:30px;  padding:0 0px;  font-family:'楷体';font-size:20px;}"
               "QToolButton#toolButton_resultView_2:pressed{border: 1px solid  rgb(0, 250, 0); background-color: rgb(250, 250, 250); border-style: solid;  border-radius:10px; width: 80px;  height:30px;  padding:0 0px; font-family:'楷体';font-size:20px;} ";
    }
    else
    {
        Css_ = "QToolButton{border: 0px solid rgb(225, 225, 225);background-color:rgba(0,0,0,0);border-radius: 10px; width: 80px;  height:60px;  padding:0 0px;  font-family:'楷体';font-size:22px; background: transparent;}"
               "QToolButton:hover{border: 2px solid rgb(205, 201, 201);background-color: rgb(250, 250, 250); border-style: solid;border-radius:10px;  width: 80px; height:30px;  padding:0 0px; font-family:'楷体';font-size:20px;}"
               "QToolButton#toolButton_resultView_2:pressed{border: 1px solid  rgb(220, 220, 220); background-color: rgb(250, 250, 250); border-style: solid;  border-radius:10px; width: 80px;  height:30px;  padding:0 0px;  font-family:'楷体';font-size:20px;} ";
    }
    ptoolbtn->setStyleSheet(Css_);
    ptoolbtn->update();
    return;
}

QColor GlobalData::customCurveColor(quint8 indexReag)
{
    QColor customColor;
    switch(indexReag)
    {
        case AA_REAGENT:  
            customColor.setRgb(86,147,198,255);
		break;
        case ADP_REAGENT: 
            customColor.setRgb(114,106,176,255);
		break;
        case EPI_REAGENT: 
            customColor.setRgb(234,42,42,255);
		break;
        case COL_REAGENT: 
            customColor.setRgb(147,153,156,255);
		break;
        case RIS_REAGENT: 
            customColor.setRgb(188, 143, 143,255);
		break;
        default:
            customColor.setRgb(0,0,0,0);
        break;
    }
    return customColor;
}


void GlobalData::QTableWidgetinitSheet(QTableWidget *CustomTableWidget,const QString TableName,
                                        const QStringList itemHeand,const QStringList itemRows,bool binsertRows)
{
    CustomTableWidget->setMouseTracking(true);  //设置鼠标追踪
    CustomTableWidget->setSelectionBehavior(QAbstractItemView::SelectItems); //整行选中的方式
    CustomTableWidget->setEditTriggers(QAbstractItemView::CurrentChanged); //禁止编辑
    CustomTableWidget->horizontalHeader()->setFont(QFont("楷体", 16));
    //表列随着表格变化而自适应变化++
    CustomTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    CustomTableWidget->setAlternatingRowColors(true); //隔行换色

    //点击表时不对表头行光亮（获取焦点)
    CustomTableWidget->horizontalHeader()->setHighlightSections(false);

    //设置表头字体加粗
    QFont font = CustomTableWidget->horizontalHeader()->font();
    font.setBold(true);
    CustomTableWidget->horizontalHeader()->setFont(font);
    CustomTableWidget->verticalHeader()->setHidden(true);       //隐藏行号列
    CustomTableWidget->setWindowTitle(TableName);

    //表头
    int leng_ = itemHeand.size();
    CustomTableWidget->setColumnCount(leng_);
    CustomTableWidget->setHorizontalHeaderLabels(itemHeand);
    CustomTableWidget->verticalHeader()->setVisible(false);
    CustomTableWidget->verticalHeader()->setDefaultSectionSize(35); //默认行高

    //表行
    if(binsertRows)
    {
        int totalRows = itemRows.size();
        CustomTableWidget->setRowCount(totalRows);
        CustomTableWidget->verticalHeader()->setVisible(false);
        CustomTableWidget->verticalHeader()->setDefaultSectionSize(35); //默认行高
    }

    QString cssTable = "QTableWidget::item:hover{background-color:rgb(70 ,130 ,180)}"
                       "QTableWidget::item:selected{background-color:rgb(139, 139, 122)}"

                       "QTableView QTableCornerButton::section{color: white; background-color: rgb(188, 187, 186); "
                       "border: 1px solid rgb(188, 187, 186);border-radius:0px; border-color: rgb(0, 0, 0);"
                       "font: bold 1pt; padding:12px 0 0 10px}"

                       "QHeaderView::section,QTableCornerButton:section{padding:3px; margin:0px; color:rgba(188, 187, 186, 255); "
                       "border:1px solid rgba(188, 187, 186, 255); border-left-width:0px; border-right-width:1px; border-top-width:0px; "
                       "border-bottom-width:1px; background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252);}"

                       "QTableWidget{background-color: white; border: 1px; border-color: rgb(0, 0, 0);}"

                       "QHeaderView::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 rgba(188, 187, 186, 255), "
                       "stop: 0.5 rgba(188, 187, 186, 255),stop: 0.6 rgba(188, 187, 186, 255), stop:1 rgba(188, 187, 186, 255)); color: white;}"

                       "QTableView QTableCornerButton::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 rgba(188, 187, 186, 255), "
                       "stop: 0.5 rgba(188, 187, 186, 255),stop: 0.6 rgba(188, 187, 186, 255), stop:1 rgba(188, 187, 186, 255)); color: white;}";
    CustomTableWidget->setStyleSheet(cssTable);
    CustomTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(188, 187, 186); font: 18pt '楷体';color: black; border:2px;};");

    //设置水平、垂直滚动条样式
    CustomTableWidget->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:10px;}"
    "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
    "QScrollBar::handle:hover{background:gray;}"
    "QScrollBar::sub-line{background:transparent;}"
    "QScrollBar::add-line{background:transparent;}");
    CustomTableWidget->update();
    return;
}


// 移动平均滤波器（处理边界）
QVector<double> GlobalData::smoothData(const QVector<double>& input, int windowSize) {
    QVector<double> smoothed;
    if (windowSize < 1 || input.isEmpty()) return input;

    const int halfWindow = windowSize / 2;
    const int n = input.size();

    for (int i = 0; i < n; ++i) {
        double sum = 0.0;
        int count = 0;

        int start = std::max(0, i - halfWindow);
        int end = std::min(n - 1, i + halfWindow);

        for (int j = start; j <= end; ++j) {
            sum += input[j];
            ++count;
        }
        smoothed.append(sum / count);
    }
    return smoothed;
}

QString GlobalData::mapReagentNames(const quint8 KindConsumables){

    static const QHash<quint8, QString> consumablesMap = {
        {INDEX_AA_CONSUMABLE,      "AA"},
        {INDEX_ADP_CONSUMABLE,     "ADP"},
        {INDEX_EPI_CONSUMABLE,     "EPI"},
        {INDEX_COL_CONSUMABLE,     "COL"},
        {INDEX_RIS_CONSUMABLE,     "RIS"},
        {INDEX_CLEANLINQUE_CONSUMABLE, "清洗剂S1(内)"},
        {INDEX_TESTTUBE_CONSUMABLE,    "测试试管"},
        {OUTSIDE_CLEANLINQUE_S2,   "清洗剂S2(外)"},
        {INDEX_AA_1_CONSUMABLE,      "AA(R)"},
        {INDEX_ADP_1_CONSUMABLE,     "ADP(R)"},
        {INDEX_EPI_1_CONSUMABLE,     "EPI(R)"},
        {INDEX_COL_1_CONSUMABLE,     "COL(R)"},
        {INDEX_RIS_1_CONSUMABLE,     "RIS(R)"}
    };

    // 使用value()方法并设置默认返回值
    return consumablesMap.value(KindConsumables, "");
}

QString GlobalData::mapIndexReagentnames(quint8 index){
    static const QHash<quint8, QString> consumablesMap = {
        {ANEMIA,      "PPP"},
        {AA_REAGENT,      "AA"},
        {ADP_REAGENT,     "ADP"},
        {EPI_REAGENT,     "EPI"},
        {COL_REAGENT,     "COL"},
        {RIS_REAGENT,     "RIS"}
    };

    // 使用value()方法并设置默认返回值
    return consumablesMap.value(index, "");
}
quint8 GlobalData::indexReagentMapReagnetNmaes(const QString testReagName)
{
    static const QHash<QString, quint8> reagentMap = {
            {"AA",   AA_REAGENT},
            {"ADP",  ADP_REAGENT},
            {"EPI",  EPI_REAGENT},
            {"COL",  COL_REAGENT},
            {"RIS",  RIS_REAGENT}
    };
    return reagentMap.value(testReagName, 0);
}

void  GlobalData::sycnTestingReagnetNum(QList<quint8> &testIndex, QString reagentName)
{
    testIndex.clear();
    QStringList output = reagentName.split("|");
    testIndex.reserve(output.size());
    foreach (QString reag, output)
    {
        quint8 index = indexReagentMapReagnetNmaes(reag);
        testIndex.push_back(index);
    }
    return;
}

bool GlobalData::commandStatus(const int &run_status,QString &outtext){
    bool isfinished = false;
    switch(run_status){
        case MOTOR_WAITING:
            outtext = "指令返回状态:等待运行";
        break;
        case MOTOR_RUNNING:
            outtext = "指令返回状态:运行中";
        break;
        case MOTOR_ERROR:
            outtext = "指令返回状态:执行异常";
        break;
        case MOTOR_FINISH:
            outtext = "指令返回状态:执行完成";
            isfinished = true;
        break;
        default : break;
    }
    return isfinished;
}

QString GlobalData::sycnBottleCapacitykey(quint8 index_reag)
{
    static const QHash<quint8, QString> CapacitykeyList = {
        {INDEX_AA_CONSUMABLE,      "AACapacity"},
        {INDEX_ADP_CONSUMABLE,     "ADPCapacity"},
        {INDEX_EPI_CONSUMABLE,     "EPICapacity"},
        {INDEX_COL_CONSUMABLE,     "COLCapacity"},
        {INDEX_RIS_CONSUMABLE,     "RISCapacity"},
        {INDEX_CLEANLINQUE_CONSUMABLE,  "S1CleanCapacity"}

    };
    return CapacitykeyList.value(index_reag,"");
}

QString GlobalData::sycnBottleLimitskey(quint8 index_reag)
{
    static const QHash<quint8, QString> LimitskeyList = {
        {INDEX_AA_CONSUMABLE,      "AALimits"},
        {INDEX_ADP_CONSUMABLE,     "ADPLimits"},
        {INDEX_EPI_CONSUMABLE,     "EPILimits"},
        {INDEX_COL_CONSUMABLE,     "COLLimits"},
        {INDEX_RIS_CONSUMABLE,     "RISLimits"},
        {INDEX_CLEANLINQUE_CONSUMABLE,  "S1CleanLimits"}

    };
    return LimitskeyList.value(index_reag,"");
}

//写入缓存区主板耗材
QByteArray GlobalData::writeBuffer2Equipment()
{
    QByteArray buffer;
    QUIUtils::writeSuppliesBottleControlOrder(buffer,0,0,0,0,false);
    return buffer;
}

//写入试剂限位瓶数限位
QByteArray GlobalData::writeBottleLimit2Equipment()
{
    QMap<int, quint8> writeBoardValList;
    INI_File ini; // 复用实例

    // 使用auto简化类型转换
    auto loadParam = [&ini](const char* key) {
        return static_cast<quint8>(ini.rConfigPara(key).toUInt());
    };

    writeBoardValList.insert(INDEX_AA_CONSUMABLE, loadParam("AAbottleLimit"));
    writeBoardValList.insert(INDEX_ADP_CONSUMABLE, loadParam("ADPbottleLimit"));
    writeBoardValList.insert(INDEX_EPI_CONSUMABLE, loadParam("EPIbottleLimit"));
    writeBoardValList.insert(INDEX_COL_CONSUMABLE, loadParam("COLbottleLimit"));
    writeBoardValList.insert(INDEX_RIS_CONSUMABLE, loadParam("RISbottleLimit"));
    writeBoardValList.insert(INDEX_CLEANLINQUE_CONSUMABLE, loadParam("S1bottleLimit"));
    writeBoardValList.insert(INDEX_TESTTUBE_CONSUMABLE, loadParam("tubeTarybottleLimit"));
    writeBoardValList.insert(OUTSIDE_CLEANLINQUE_S2, loadParam("S2bottleLimit"));

    // 明确未使用字段的语义
    constexpr int UNUSED_SLOT = 0;
    writeBoardValList.insert(OUTSIDE_CLEANLINQUE_S2 + 1, UNUSED_SLOT);
    writeBoardValList.insert(OUTSIDE_CLEANLINQUE_S2 + 2, UNUSED_SLOT);
    return QUIUtils::WriteBottleLimitArry(writeBoardValList, true);
}



void GlobalData::splitDouble(double num, int &integer, double &fraction/*, QChar &sign*/) {
    //sign = num < 0 ? '-' : (num > 0 ? '+' : ' '); // 确定符号
    num = std::abs(num); // 转为正数处理
    double intPart;
    fraction = modf(num, &intPart);
    integer = static_cast<int>(intPart);
}

bool GlobalData::CreadFolder(const QString Foldername)
{
    bool bcreat = false;
    QString folderPath = QString(QDir(QCoreApplication::applicationDirPath()).filePath(Foldername));
    QDir dir(folderPath);
    if(!dir.exists()){
        bool ok_ = dir.mkpath(folderPath);
        bcreat = ok_;
    }
    else
        bcreat = true;
    return bcreat;
}


void GlobalData::CreatFloadfileAppend(const QString& foldername,const QString& filename,const QString& data)
{
    QString folderPath = QString(QDir(QCoreApplication::applicationDirPath()).filePath(foldername));
    QString dirName = folderPath + "/" + QDateTime::currentDateTime().toString("yyyyMMdd");
    QDir dir(dirName);
    if(!dir.exists()){
        dir.mkdir(dirName);
    }

    QString fileName = dirName + "/" + filename + ".txt" ;
    QFile file(fileName);
    bool bRet = file.open(QIODevice::WriteOnly |QIODevice::Append | QIODevice::Text);
    if(bRet)
    {
        QTextStream aStream(&file); //用文本流读取文件
        QString str = data;//叠加调用并不会换行
        aStream<< str << endl; //endl起到换行的作用，在字符后面有换行
    }
    file.close();

}


quint8 GlobalData::BytesistakenOutoftheprotocol(const quint8 seq,const QByteArray outcode)
{
    // 检查长度并取索引为2的字节
    int leng = seq +  1;
    if (outcode.size() >= leng) {
        unsigned char byteValue = static_cast<unsigned char>(outcode[seq]);
        int decimalValue = static_cast<int>(byteValue);
        QLOG_DEBUG() << "索引"<<seq<<"的十进制值:" << decimalValue;  // 输出:
    } else {
        QLOG_DEBUG() << "数据长度不足！";
    }
    return 0;
}



quint32 GlobalData::backHandsSuckAirValue(const QStringList recvdata)
{
    bool ok;
    QString airval = recvdata[13] + recvdata[12] + recvdata[11] + recvdata[10];
    quint32 airdata = airval.toInt(&ok,16);
    return airdata;
}


QString GlobalData::analyzeWhetherGripperSuction(const QStringList byteArray,bool &isSuction,quint32 &airvalue)
{
	bool ok;
    QString valueArry = byteArray[5];
    quint8 controlbyte5th =  valueArry.toInt(&ok,16);
    QString statusAxis = QString("%1").arg(QString::number(controlbyte5th, 2), 8, QChar('0'));
    quint8 HandsStatus = statusAxis.mid(3, 1).toUInt(&ok, 2);
    isSuction = (HandsStatus == 1)? true : false;

    airvalue = backHandsSuckAirValue(byteArray);
    QString statushands = (isSuction)? "吸":"吐";
    QLOG_DEBUG()<<"解析是否抓手控制"<< byteArray <<"电机字节4th"<< statusAxis <<"抓手状态:"<<statushands
                   <<"负压值:"<<airvalue<<endl;
    return statushands+"负压值:"+ QString::number(airvalue);
}




bool GlobalData::isHandsSuckFinish(quint32 airvale){
    quint32 Maxsuck = static_cast<quint32>(INI_File().rConfigPara(GRIPPERLESSTHANMAX).toInt());
    return (airvale <= Maxsuck);
}

bool GlobalData::isHandsThrowCupFinish(quint32 airvale){
    quint32 Minsplit = static_cast<quint32>(INI_File().rConfigPara(GRIPPERBIGTHANMIM).toInt());
    return (airvale >= Minsplit);
}


QByteArray GlobalData::Grabthehandtotryagain(const quint8 num,const int times,quint8 catchChannel)
{
    quint8 commandnum = num;
    int chndownhands = INI_File()._gethandsdownheiht(catchChannel) + times;
    bool bRepeattheGrabCup = INI_File().rConfigPara(REPEATGRABCUP).toBool();
    QByteArray poutdata = Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,
                                                                         chndownhands,
                                                                         0,
                                                                         commandnum,
                                                                         false,
                                                                         chndownhands,
                                                                         bRepeattheGrabCup,
                                                                         GRABCUPWITHGRIPPER);


    return poutdata;
}

QByteArray GlobalData::ThrowCupstotryagain(const quint8 num,const int times)
{
    quint8 commandnum = num;
    int _throwcupheigh = INI_File()._gethandsdownthrowcpus() - times;
    QByteArray poutdata= Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,
                                                                        _throwcupheigh,0,
                                                                        commandnum,
                                                                        false,
                                                                        _throwcupheigh,
                                                                        false,GRIPPERNORMAL);


    return poutdata;
}

QByteArray GlobalData::GrabtheCupAttheTestTubeTray(const quint8 num , const int times, quint8 testTubeHole) {

    quint8 commandnum = num;

    const quint8 maxIndex = 3; // 最大允许的托盘索引
    quint8 index_tray = testTubeHole / ONETRAY_TOTALTUBE; // 直接计算区间

    // 边界检查：若超过最大值则归零（或按需求处理）
    if (index_tray > maxIndex) {
        index_tray = 0; // 可根据实际需求调整为其他值或断言
    }
    quint8 indextraydown = INI_File()._gethandsdownheightinTesttray(index_tray) + times;

    QByteArray SuckCupArry = Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,
																		   indextraydown,0,
                                                                           commandnum,false,
                                                                           indextraydown,
                                                                           false,GRABCUPWITHGRIPPER);
    return SuckCupArry;

}

QByteArray GlobalData::putoutCupAtTestChannel(const quint8 num,const int times,quint8 indexChn)
{
    int HandsGripDownHigh = (int)INI_File()._gethandsdownheiht(indexChn) - times;
    quint8 commandnum = num;
    QByteArray outdata = Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,
                                                                        HandsGripDownHigh,
                                                                        0,
                                                                        commandnum,
                                                                        false,
                                                                        HandsGripDownHigh,
                                                                        false,GRIPPERNORMAL);
    return outdata;
}


quint8 GlobalData::analyzeSlave_addr(const QStringList recvData, bool outlogs) {
    // 1. 添加转换校验逻辑
    bool ok;
    QString valueArry = recvData.value(SLAVE_ADDRESS);  // 使用value()避免越界
    quint8 slaveaddr = valueArry.toInt(&ok, HEX_SWITCH);
    if (!ok) {
        QLOG_WARN() << "无效的从机地址：" << valueArry;
        return 0xFF; // 返回错误码
    }

    if (!outlogs) return slaveaddr;


    // 2. 使用静态查找表替代switch-case
    static const QHash<quint8, QString> addrMap = {
        {MODULE_1,      "模组I"},
        {MODULE_2,      "模组II"},
        {MODULE_3,      "模组III"},
        {Z_AXIS,        "Z轴电机"},
        {MAIN_CONTROL,  "主板控制"}
    };

    // 3. 通过哈希表查找日志信息
    auto it = addrMap.find(slaveaddr);
    if (it != addrMap.end()) {
        QLOG_INFO() << it.value();
    } else {
        QLOG_WARN() << "未知从机地址：" << slaveaddr;
    }

    return slaveaddr;
}

quint8 GlobalData::analyzeCommandIndex(const QStringList recvData) {

    bool ok;
    QString valueArry = recvData.value(COMMANDNUMBER);  // 使用value()避免越界
    quint8  commandIndex = valueArry.toInt(&ok, HEX_SWITCH);
    if (!ok) {
        QLOG_WARN() << "无效的命令编号：" << valueArry;
        return 0xFF; // 返回错误码
    }
    return commandIndex;
}



void GlobalData::Parse4thBytecommandstatus(const QStringList recvData,
                                           int &commandMode,
                                           int &sport_status,
                                           int &speedMode ,
                                           int &which_motor) {
	bool ok;
    QString valueArry = recvData[4];
    quint8  controlbyte4th =  valueArry.toInt(&ok,16);

    QString binaryStr = QString::number(controlbyte4th, 2).rightJustified(8, '0', true);
    if (binaryStr.length() != 8) {
            QLOG_WARN() << "无效的二进制数据：" << controlbyte4th;
            return ;
    }

    //使用辅助函数提取二进制位
    auto extractBits = [&](int pos, int len) {
        return binaryStr.mid(pos, len).toInt(nullptr, 2);
    };

    //报告状态解析
    commandMode =  extractBits(7, 1);

    which_motor =  extractBits(0, 3);

    sport_status = extractBits(5, 2);

    speedMode =    extractBits(3, 2);

    return;

}


QByteArray GlobalData::intToQByteArray(int input, bool EndianTyped)
{
    QByteArray output;
    if (EndianTyped)
    {
        output[3] = (uchar)(0x000000ff & input);
        output[2] = (uchar)((0x0000ff00 & input) >> 8);
        output[1] = (uchar)((0x00ff0000 & input) >> 16);
        output[0] = (uchar)((0xff000000 & input) >> 24);
    }
    else
    {
        output[0] = (uchar)(0x000000ff & input);
        output[1] = (uchar)((0x0000ff00 & input) >> 8);
        output[2] = (uchar)((0x00ff0000 & input) >> 16);
        output[3] = (uchar)((0xff000000 & input) >> 24);
    }
    return output;
}
QByteArray GlobalData::GrabTheCupUnderTheGripper(double downHeight,int &indexNum,bool backOriginLoc,bool bGrab)
{
    QByteArray driverarry;
    driverarry = QByteArray::fromHex(QString::number(4).toUtf8());
    driverarry.push_back(0x17);

    QString strHex = QString::number(indexNum);
    driverarry.push_back(QByteArray::fromHex(strHex.toUtf8())); //命令编号
    indexNum++;

    driverarry.push_back(QByteArray::fromHex(QString::number(0,HEX_SWITCH).toUtf8())); //缓存容量

    //高位在前 低位在后
    QString operationMode;
    QString axis =          QString("%1").arg(0, 3, 2,QLatin1Char('0')); //指定的点机
    if(!backOriginLoc)
        operationMode = QString("%1").arg(0, 2, 2,QLatin1Char('0')); //位置模式
    else
       operationMode = QString("%1").arg(0, 2, 2,QLatin1Char('0')); //负速度模式复位
    QString cmd_state =     QString("%1").arg(0, 2, 2,QLatin1Char('0'));
    QString type =          QString("%1").arg(0, 1, 2,QLatin1Char('0')); //命令

    QString hexstr = QString("%1%2%3%4").arg(axis).arg(operationMode).arg(cmd_state).arg(type);
    quint8 numtmp = hexstr.toInt(nullptr,BINARY_SWITCH);
    QString HexParastr = QString::number(numtmp,HEX_SWITCH);
    driverarry.push_back(QByteArray::fromHex(HexParastr.toUtf8()));

    QString LinqueTest = QString("%1").arg(0, 1, 2,QLatin1Char('0'));
    QString OptocouplerBack = QString("%1").arg(0, 1, 2,QLatin1Char('0'));
    QString DownModel =  QString("%1").arg(0, 1, 2,QLatin1Char('0'));
    QString catchState;
    if(bGrab)
        catchState = QString("%1").arg(1, 1, 2,QLatin1Char('0'));
    else
        catchState = QString("%1").arg(0, 1, 2,QLatin1Char('0'));
    QString zonespos   = QString("%1").arg(0, 2, 2,QLatin1Char('0'));
    QString offsetAxis = QString("%1").arg(0, 2, 2,QLatin1Char('0'));
    hexstr = QString("%1%2%3%4%5").arg(LinqueTest).arg(OptocouplerBack).arg(DownModel).arg(catchState)
            .arg(zonespos).arg(offsetAxis);
    numtmp = hexstr.toInt(nullptr,BINARY_SWITCH);
    HexParastr = QString::number(numtmp,HEX_SWITCH);
    driverarry.push_back(QByteArray::fromHex(HexParastr.toUtf8()));

    QByteArray downHeightVal;
    downHeightVal = intToQByteArray(downHeight,false);

    driverarry.push_back(downHeightVal); //4

    QByteArray nullByte = intToQByteArray(0,false);
    driverarry.push_back(nullByte);

    driverarry.push_back(QByteArray::fromHex(QString::number(0,HEX_SWITCH).toUtf8()));
    QLOG_DEBUG()<<"抓手下降抓杯命令"<< driverarry.toHex(' ').trimmed().toUpper()<<endl;
    return driverarry;
}

QByteArray GlobalData::ConfigEachChannelSpeed(quint8 indexModule, quint8 indexChn,quint16 setspeed)
{
    QByteArray driverarry;
    driverarry = QByteArray::fromHex(QString::number(indexModule).toUtf8());

    driverarry.push_back(0x17);

    QString strHex = QString::number(W_MODULE_SPEED,HEX_SWITCH);
    driverarry.push_back(QByteArray::fromHex(strHex.toUtf8())); //命令编号

    driverarry.push_back(QByteArray::fromHex(QString::number(0,HEX_SWITCH).toUtf8())); //缓存容量

    //高位在前 低位在后
    QString loc_end = QString("%1").arg(2, 5, 2,QLatin1Char('0')); //模组转速
    QString loc_min = QString("%1").arg(0, 2, 2,QLatin1Char('0'));
    QString loc_top = QString("%1").arg(0, 1, 2,QLatin1Char('0')); //写
    QString hexstr = QString("%1%2%3").arg(loc_end).arg(loc_min).arg(loc_top);
    quint8 numtmp = hexstr.toInt(nullptr,2);
    QLOG_DEBUG()<<"写模组速度"<<indexModule<<"4thbit="<<hexstr<<"16进制="<<numtmp;
    QString HexParastr = QString::number(numtmp,HEX_SWITCH);
    driverarry.push_back(QByteArray::fromHex(HexParastr.toUtf8()));

    //设置目标模组温度
    auto &ini = INI_File();
    double inimoduleTemp = 0.00;
    switch(indexModule){
    case MODULE_1: inimoduleTemp = ini.rConfigPara(MODULETEMPA).toDouble();break;
    case MODULE_2: inimoduleTemp = ini.rConfigPara(MODULETEMPB).toDouble();break;
    case MODULE_3: inimoduleTemp = ini.rConfigPara(MODULETEMPC).toDouble();break;
    default: break;
    }
    quint16 moduleTemp  =  inimoduleTemp/TEMP_CONVERSION_RATIO;
    QByteArray dataTemp =  QUIUtils::qint16ToQByteArray(moduleTemp, false);
    driverarry.push_back(dataTemp);  //5 - 6 th

    //设置模组通道转速
    quint16 *chnspeedarry = new quint16[4]();
    switch(indexChn)
    {
        case TEST_CHANNEL_1:
        case TEST_CHANNEL_5:
        case TEST_CHANNEL_9:
            chnspeedarry[0] =  setspeed;
        break;
        case TEST_CHANNEL_2:
        case TEST_CHANNEL_6:
        case TEST_CHANNEL_10:
            chnspeedarry[1] =  setspeed;
        break;
        case TEST_CHANNEL_3:
        case TEST_CHANNEL_7:
        case TEST_CHANNEL_11:
            chnspeedarry[2] =  setspeed;
        break;
        case TEST_CHANNEL_4:
        case TEST_CHANNEL_8:
        case TEST_CHANNEL_12:
            chnspeedarry[3] =  setspeed;
        break;
    }
	QByteArray datachn =  QUIUtils::qint16ToQByteArray(chnspeedarry[0], false);
    driverarry.push_back(datachn);
	datachn =  QUIUtils::qint16ToQByteArray(chnspeedarry[1], false);
    driverarry.push_back(datachn);
	datachn =  QUIUtils::qint16ToQByteArray(chnspeedarry[2], false);
    driverarry.push_back(datachn);
	datachn =  QUIUtils::qint16ToQByteArray(chnspeedarry[3], false);
    driverarry.push_back(datachn);
    delete []chnspeedarry;
    QLOG_DEBUG()<<"设置模组"<<indexModule<<"通道"<<indexChn<<"转速命令"<< driverarry.toHex(' ').trimmed().toUpper()<<endl;
    return driverarry;
}


QByteArray GlobalData::ReadEachChannelSpeed(quint8 indexModule)
{
    QByteArray driverarry;
    driverarry = QByteArray::fromHex(QString::number(indexModule).toUtf8());

    driverarry.push_back(0x17);

    QString strHex = QString::number(R_MODULE_SPEED,HEX_SWITCH);
    driverarry.push_back(QByteArray::fromHex(strHex.toUtf8())); //命令编号

    driverarry.push_back(QByteArray::fromHex(QString::number(0,HEX_SWITCH).toUtf8())); //缓存容量

    //高位在前 低位在后
    QString loc_end = QString("%1").arg(2, 5, 2,QLatin1Char('0')); // 模组转速
    QString loc_min = QString("%1").arg(0, 2, 2,QLatin1Char('0'));
    QString loc_top = QString("%1").arg(1, 1, 2,QLatin1Char('0')); //读
    QString hexstr = QString("%1%2%3").arg(loc_end).arg(loc_min).arg(loc_top);
    quint8 numtmp = hexstr.toInt(nullptr,2);
    QLOG_DEBUG()<<"读模组速度"<<indexModule<<"4thbit="<<hexstr<<"16进制="<<numtmp;
    QString HexParastr = QString::number(numtmp,HEX_SWITCH);
    driverarry.push_back(QByteArray::fromHex(HexParastr.toUtf8()));

    //读取目标模组温度
    quint16 moduleTemp  =  0;
    QByteArray dataTemp =  QUIUtils::qint16ToQByteArray(moduleTemp, false);
    driverarry.push_back(dataTemp);  //5 - 6 th

    QByteArray modulechn =  QUIUtils::qint16ToQByteArray(0, false);
    driverarry.push_back(modulechn);

    modulechn =  QUIUtils::qint16ToQByteArray(0, false);
    driverarry.push_back(modulechn);

    modulechn =  QUIUtils::qint16ToQByteArray(0, false);
    driverarry.push_back(modulechn);

    modulechn =  QUIUtils::qint16ToQByteArray(0, false);
    driverarry.push_back(modulechn);
    QLOG_DEBUG()<<"读模组通道转速"<<indexModule<<"命令"<< driverarry.toHex(' ').trimmed().toUpper()<<endl;
    return driverarry;
}


QByteArray GlobalData::WriteModuleLedData(quint8 indexModule, quint8 FcunNum, quint16 Ledval)
{
    QByteArray driverarry;
    driverarry = QByteArray::fromHex(QString::number(indexModule).toUtf8());

    driverarry.push_back(0x17);

    QString strHex = QString::number(FcunNum,HEX_SWITCH);
    driverarry.push_back(QByteArray::fromHex(strHex.toUtf8())); //命令编号

    driverarry.push_back(QByteArray::fromHex(QString::number(0,HEX_SWITCH).toUtf8())); //缓存容量

    QString loc_end = QString("%1").arg(1, 5, 2,QLatin1Char('0')); //模组LED
    QString loc_min = QString("%1").arg(0, 2, 2,QLatin1Char('0'));
    QString loc_top = QString("%1").arg(0, 1, 2,QLatin1Char('0')); //写
    QString hexstr = QString("%1%2%3").arg(loc_end).arg(loc_min).arg(loc_top);
    quint8 numtmp = hexstr.toInt(nullptr,2);
    QLOG_DEBUG()<<"写模组LED"<<indexModule<<"4thbit="<<hexstr<<"16进制="<<numtmp;
    QString HexParastr = QString::number(numtmp,HEX_SWITCH);
    driverarry.push_back(QByteArray::fromHex(HexParastr.toUtf8()));

    //设置目标模组温度
    auto &ini = INI_File();
    double inimoduleTemp = 0.00;
    switch(indexModule){
    case MODULE_1: inimoduleTemp = ini.rConfigPara(MODULETEMPA).toDouble();break;
    case MODULE_2: inimoduleTemp = ini.rConfigPara(MODULETEMPB).toDouble();break;
    case MODULE_3: inimoduleTemp = ini.rConfigPara(MODULETEMPC).toDouble();break;
    default: break;
    }
    quint16 moduleTemp  =   inimoduleTemp/TEMP_CONVERSION_RATIO;
    QByteArray dataTemp =   QUIUtils::qint16ToQByteArray(moduleTemp, false);
    driverarry.push_back(dataTemp);  //5 - 6 th

    QByteArray datachn =  QUIUtils::qint16ToQByteArray(Ledval, false);
    driverarry.push_back(datachn);

    datachn =  QUIUtils::qint16ToQByteArray(Ledval, false);
    driverarry.push_back(datachn);

    datachn =  QUIUtils::qint16ToQByteArray(Ledval, false);
    driverarry.push_back(datachn);

    datachn =  QUIUtils::qint16ToQByteArray(Ledval, false);
    driverarry.push_back(datachn);
    QLOG_DEBUG()<<"写模组LED"<<indexModule<<"命令"<< driverarry.toHex(' ').trimmed().toUpper()<<endl;
    return driverarry;
}


QByteArray GlobalData::ReadModuleLedData(quint8 indexModule)
{
    QByteArray driverarry;
    driverarry = QByteArray::fromHex(QString::number(indexModule).toUtf8());

    driverarry.push_back(0x17);

    QString strHex = QString::number(R_MODULE_LED,HEX_SWITCH);
    driverarry.push_back(QByteArray::fromHex(strHex.toUtf8())); //命令编号

    driverarry.push_back(QByteArray::fromHex(QString::number(0,HEX_SWITCH).toUtf8())); //缓存容量

    //高位在前 低位在后
    QString loc_end = QString("%1").arg(1, 5, 2,QLatin1Char('0')); // 模组LED
    QString loc_min = QString("%1").arg(0, 2, 2,QLatin1Char('0'));
    QString loc_top = QString("%1").arg(1, 1, 2,QLatin1Char('0')); //读
    QString hexstr = QString("%1%2%3").arg(loc_end).arg(loc_min).arg(loc_top);
    quint8 numtmp = hexstr.toInt(nullptr,2);
    QLOG_DEBUG()<<"读模组LED"<<indexModule<<"4thbit="<<hexstr<<"16进制="<<numtmp;
    QString HexParastr = QString::number(numtmp,HEX_SWITCH);
    driverarry.push_back(QByteArray::fromHex(HexParastr.toUtf8()));

    //读取目标模组温度
    quint16 moduleTemp  =  0;
    QByteArray dataTemp =  QUIUtils::qint16ToQByteArray(moduleTemp, false);
    driverarry.push_back(dataTemp);  //5 - 6 th

    QByteArray modulechn =  QUIUtils::qint16ToQByteArray(0, false);
    driverarry.push_back(modulechn);

    modulechn =  QUIUtils::qint16ToQByteArray(0, false);
    driverarry.push_back(modulechn);

    modulechn =  QUIUtils::qint16ToQByteArray(0, false);
    driverarry.push_back(modulechn);

    modulechn =  QUIUtils::qint16ToQByteArray(0, false);
    driverarry.push_back(modulechn);
    QLOG_DEBUG()<<"读模组led"<<indexModule<<"命令"<< driverarry.toHex(' ').trimmed().toUpper()<<endl;
    return driverarry;
}


QByteArray GlobalData::SaveConfigModuleData(quint8 indexModule,quint8 fucnNum)
{
    QByteArray driverarry;
    driverarry = QByteArray::fromHex(QString::number(indexModule).toUtf8());

    driverarry.push_back(0x17);

    QString strHex = QString::number(fucnNum,HEX_SWITCH);
    driverarry.push_back(QByteArray::fromHex(strHex.toUtf8())); //命令编号

    driverarry.push_back(QByteArray::fromHex(QString::number(0,HEX_SWITCH).toUtf8())); //缓存容量

    QString loc_end = QString("%1").arg(3, 5, 2,QLatin1Char('0')); //保存
    QString loc_min = QString("%1").arg(0, 2, 2,QLatin1Char('0'));
    QString loc_top = QString("%1").arg(0, 1, 2,QLatin1Char('0')); //写
    QString hexstr = QString("%1%2%3").arg(loc_end).arg(loc_min).arg(loc_top);
    quint8 numtmp = hexstr.toInt(nullptr,2);
    QLOG_DEBUG()<<"保存模组调整"<<indexModule<<"4thbit="<<hexstr<<"16进制="<<numtmp;
    QString HexParastr = QString::number(numtmp,HEX_SWITCH);
    driverarry.push_back(QByteArray::fromHex(HexParastr.toUtf8()));

    //设置目标模组温度
    auto &ini = INI_File();
    double inimoduleTemp = 0.00;
    switch(indexModule){
    case MODULE_1: inimoduleTemp = ini.rConfigPara(MODULETEMPA).toDouble();break;
    case MODULE_2: inimoduleTemp = ini.rConfigPara(MODULETEMPB).toDouble();break;
    case MODULE_3: inimoduleTemp = ini.rConfigPara(MODULETEMPC).toDouble();break;
    default: break;
    }
    quint16 moduleTemp  =   inimoduleTemp/TEMP_CONVERSION_RATIO;
    QByteArray dataTemp =   QUIUtils::qint16ToQByteArray(moduleTemp, false);
    driverarry.push_back(dataTemp);  //5 - 6 th

    QByteArray data_ =  QUIUtils::qint16ToQByteArray(0, false);
    driverarry.push_back(data_);

    data_ =  QUIUtils::qint16ToQByteArray(0, false);
    driverarry.push_back(data_);

    data_ =  QUIUtils::qint16ToQByteArray(0, false);
    driverarry.push_back(data_);

    data_ =  QUIUtils::qint16ToQByteArray(0, false);
    driverarry.push_back(data_);
    QLOG_DEBUG()<<"保存模组设置"<<indexModule<<"命令"<< driverarry.toHex(' ').trimmed().toUpper()<<endl;
    return driverarry;
}




QString GlobalData::ObatinCreatSampleTime()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString sampleCreatTime = QString("%1%2%3").arg(current_date_time.toString("yyyy")).arg(current_date_time.toString("MM"))
            .arg(current_date_time.toString("dd"));
    return sampleCreatTime;
}

void GlobalData::apartSampleId(const QString sampleId, QString &date, int &id)
{
    QString clonedata = sampleId;
    int leng = clonedata.length();
    id = clonedata.right(4).toInt();
    date = clonedata.left(leng - 4);
    return;
}

// 组合样本号
QString GlobalData::groupDateAndID(const QString date, const int idnum)
{
    QString outputSampleid;
    QString sampleid = QString("%1").arg(idnum, 4, 10,QLatin1Char('0'));
    outputSampleid = date + sampleid;
    return outputSampleid;
}

void GlobalData::vectorconversionString(QVector<double> curvepoint,QString &curvepos)
{
    int size_len = curvepoint.size();
    for(int n = 0; n < size_len; n++)
    {
        double _data = curvepoint.at(n);
        QString _data_precision = QString::number(_data, 'f', 2);
        if(n + 1 != size_len)
            curvepos = curvepos + QString("%1,").arg(_data_precision);
        else
           curvepos = curvepos + QString("%1").arg(_data_precision);
    }
    return;
}


void GlobalData::outputtheAbsorbentIndex(const quint8 focuinActive,
                                          quint8 &index_reagent,
                                          quint8 &consumable_index) {
    bool reatentLocLeft = true;
    switch(focuinActive)
    {
        case FOCUS_SPIT_AA_TESTING:
            index_reagent = AA_REAGENT;
            consumable_index = INDEX_AA_CONSUMABLE;
            //consumable_index = INDEX_AA_1_CONSUMABLE;
        break;
        case FOCUS_SPIT_ADP_TESTING:
            index_reagent = ADP_REAGENT;
            consumable_index = INDEX_ADP_CONSUMABLE;
            // consumable_index = INDEX_ADP_1_CONSUMABLE;
        break;
        case FOCUS_SPIT_EPI_TESTING:
            index_reagent = EPI_REAGENT ;
            consumable_index = INDEX_EPI_CONSUMABLE; //consumable_index = INDEX_EPI_1_CONSUMABLE;
        break;
        case FOCUS_SPIT_COL_TESTING:
            index_reagent = COL_REAGENT ;
            consumable_index = INDEX_COL_CONSUMABLE;//consumable_index = INDEX_COL_1_CONSUMABLE;
        break;
        case FOCUS_SPIT_RIS_TESTING:
            index_reagent = RIS_REAGENT ;
            consumable_index = INDEX_RIS_CONSUMABLE;// consumable_index = INDEX_RIS_1_CONSUMABLE;
        break;
        default:break;
    }
}

bool GlobalData::determineReagentontheleftIsSufficient(quint8 consumableIndex,quint8 reagentIndex)
{
    // 获取试剂消耗量
    const unsigned char usedVolume = INI_File().getTypesReagentSuckVolume(reagentIndex);
    if (usedVolume == 0) {
        QLOG_WARN() << "试剂消耗量为0，跳过更新流程，类型:" << reagentIndex;
        return false;
    }

    double remainingRatio  = 0.0; //输出剩余比
    quint16 updatedRatio  = 0;   //剩余比例
    QUIUtils::CalculateTheConsumptionOfReagent(consumableIndex,
                                               usedVolume,
                                               remainingRatio,
                                               updatedRatio );

    quint8 limitArlm;
    ConsumablesOper::GetpInstance()->updateReagentLimit(READ_OPERRAT,
                                                        consumableIndex,
                                                        limitArlm);
    const double remainingPercentage = static_cast<double>(remainingRatio * 100);

    return (remainingPercentage >= limitArlm);
}

QByteArray GlobalData::replactSuckReagentLoc(quint8 num,quint8 indexReagnet){

    quint8 testingReag = 0;
    switch (indexReagnet)
	{
        case AA_REAGENT:   testingReag = 5; break;
        case ADP_REAGENT:  testingReag = 6; break;
        case EPI_REAGENT:  testingReag = 7; break;
        case COL_REAGENT:  testingReag = 8; break;
        case RIS_REAGENT:  testingReag = 9; break;
	default:
		break;
	}
    QPoint AbsorbReaentAxis(0,0);
    SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT,testingReag,AbsorbReaentAxis);
    return Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(AbsorbReaentAxis,
                                                            0,
                                                            0,
                                                            num);
}

QByteArrayList GlobalData::errMachineBackOrigin(const quint8 num,const int indexZAxis)
{
    QByteArrayList allBackOrigin;
    allBackOrigin.reserve(4);
    quint8 indexcode = num;
    QPoint OriginAxis(0,0);
    const QByteArray dataArry = Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(indexZAxis,10,0,indexcode,
                                                                               false,10,false,GRIPPERNORMAL);
    allBackOrigin.push_back(dataArry);
    allBackOrigin.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(indexZAxis,0,TRAY_TESTCHANNEL,
                                                                indexcode,false));
    allBackOrigin.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(OriginAxis,0,0,indexcode));
    SingletonAxis::GetInstance()->originPos(READ_OPERRAT,OriginAxis);
    allBackOrigin.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(OriginAxis,0,0,indexcode));
    return allBackOrigin;
}


QByteArrayList GlobalData::outCommandSuckReagent(const quint8 indexReag){

    QPoint suckReagentAxis(0,0);
    const quint8 testingReag = indexReag - 1;
    //试剂坐标从0-9下标开始 indexReag = AA:RIS 0 - 4
    SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT,testingReag,suckReagentAxis);
    const bool issuckAir = INI_File().rConfigPara(FIRSTSUCKAIRS).toBool();    //吸空气校准
    const int  linqueFailedHeigh = INI_File().GetFailedReagentsLinqueHigh();
    auto *pActiveStu = Testing::m_TaskDll;
    int washTime = 0;
    quint8 num = 0;

    QByteArrayList outdata;
    outdata.push_back(pActiveStu->DLL_XYMoveSpecifiedPosition(suckReagentAxis,0,0,num));
    if(issuckAir){
        outdata.push_back(pActiveStu->SmallBenActive(true,SMALL_BEN_INHALE_ARI,DIS_WASHES_PUMPS,num, washTime));
    }

    outdata.push_back(pActiveStu->DLL_ZMoveSpecifiedPosition(MOTOR_REAGNET_INDEX,
                                                             linqueFailedHeigh,
                                                             0,
                                                             num,
                                                             true,
                                                             linqueFailedHeigh,
                                                             false,
                                                             GRIPPERNORMAL));
    /*总步数吸试剂*/
    int TotalSuckStep = QUIUtils::SuckReagentNumberofSteps(indexReag);

    /*动小泵吸试剂*/
    outdata.push_back(pActiveStu->SmallBenActive(true,
                                                TotalSuckStep,
                                                DIS_WASHES_PUMPS,
                                                num,
                                                washTime)
                      );
    //试剂针复位
    outdata.push_back(pActiveStu->DLL_ZAxis_Reset(MOTOR_REAGNET_INDEX,0,0,num,false));
    return outdata;
}

QByteArrayList GlobalData::outSuckPRPtestReagentCup(int testChannel,
                                                    quint8 suckHole,
                                                    QPoint &PRPHoleAxis){


    QByteArrayList outdataCommand;
    outdataCommand.clear();
    quint8 num = outdataCommand.size()%255;
    int putDownHeigh  = INI_File()._gethandsdownheiht(testChannel); //抓手在通道放杯高度
    quint8 suckCupHeigh =  QUIUtils::_hansdownheightinnertubetray(suckHole); //在试管盘吸杯高度
    const bool bRepeattheGrabCup = INI_File().rConfigPara(REPEATGRABCUP).toBool();


    QPoint channelPutAxis(0,0);
    SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,suckHole,MOTOR_HANDS_INDEX,PRPHoleAxis);

    SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT,testChannel,MOTOR_HANDS_INDEX,channelPutAxis);
    auto *pActivestu = Testing::m_TaskDll;
    outdataCommand.push_back(pActivestu->DLL_XYMoveSpecifiedPosition(PRPHoleAxis,0,0,num));

    outdataCommand.push_back(pActivestu->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,suckCupHeigh,
                                                                    0,num,false,
                                                                    suckCupHeigh,
                                                                    bRepeattheGrabCup,
                                                                    GRABCUPWITHGRIPPER));

    outdataCommand.push_back(pActivestu->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,
                                                        suckCupHeigh,
                                                        0,num,
                                                        bRepeattheGrabCup));

    outdataCommand.push_back(pActivestu->DLL_XYMoveSpecifiedPosition(channelPutAxis,0,0,num));



    outdataCommand.push_back(pActivestu->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX,putDownHeigh,
                                                                    0,num,false,
                                                                    putDownHeigh,false,GRIPPERNORMAL));


    outdataCommand.push_back(pActivestu->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,
                                                         putDownHeigh,0,
                                                         num,false));

    return outdataCommand;
}


QByteArrayList GlobalData::letallAxisBackOrigin(QList<quint8> &backnumList){
     auto *pActivestu = Testing::m_TaskDll;
     QByteArrayList outdataCommand;
     outdataCommand.clear();
     backnumList.clear();
     outdataCommand.reserve(5);
     backnumList.reserve(5);
     QPoint nativeOrigin(0,0);
     quint8 num = 0;
     backnumList<<0<<1<<2<<3<<4;
     outdataCommand.push_back(pActivestu->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX,0,0,num,false));
     outdataCommand.push_back(pActivestu->DLL_ZAxis_Reset(MOTOR_REAGNET_INDEX,0,0,num,false));
     outdataCommand.push_back(pActivestu->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX,0,0, num,false));
     outdataCommand.push_back(pActivestu->DLL_XYMoveSpecifiedPosition(nativeOrigin,0,0,num));
     SingletonAxis::GetInstance()->originPos(READ_OPERRAT,nativeOrigin);
     outdataCommand.push_back(pActivestu->DLL_XYMoveSpecifiedPosition(nativeOrigin,0,0,num));
     return outdataCommand;
}
