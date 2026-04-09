#pragma execution_character_set("utf-8")

#include "verifycoordinates.h"
#include "loadequipmentpos.h"
#include "quiutils.h"
#include "cglobal.h"
#include <QCoreApplication>
#include "QsLog/include/QsLog.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QMessageBox>
#include <QDialog>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDateTime>
#include <QPointer>
#include <QTimer>

VerifyCoordinates* VerifyCoordinates::GetInstance()
{
    // 使用 QPointer 或确保在 QApplication 销毁前有效
   static QPointer<VerifyCoordinates> instance;
   if (instance.isNull()) {
       instance = new VerifyCoordinates();
       // 可选：设置属性，确保在 QApplication 销毁时一起销毁
       instance->setAttribute(Qt::WA_DeleteOnClose);
   }
   return instance;
}

VerifyCoordinates::VerifyCoordinates(QWidget *parent)
    : QWidget(parent)
    , m_originAxis(0, 0)
    , m_throwHoleAxis(0, 0)
    , m_reagentPin(0, 0)
    , m_bloodPin(0, 0)
    , m_cleanZoneBloodPin(0, 0)
    , m_cleanZoneReagentPin(0, 0)
    , m_loadedOriginAxis(0, 0)
    , m_loadedThrowHoleAxis(0, 0)
    , m_loadedReagentPin(0, 0)
    , m_loadedBloodPin(0, 0)
    , m_loadedCleanZoneBloodPin(0, 0)
    , m_loadedCleanZoneReagentPin(0, 0)
{
    initParsers();
    QLOG_DEBUG() << "VerifyCoordinates initialized";
}

void VerifyCoordinates::initParsers()
{
    // 固定坐标解析器
    m_parsers["OriginAxis"] = [this](int x, int y) { m_originAxis = QPoint(x, y); };
    m_parsers["CleanZone(Blood)"] = [this](int x, int y) { m_cleanZoneBloodPin = QPoint(x, y); };
    m_parsers["CleanZone(Reagent)"] = [this](int x, int y) { m_cleanZoneReagentPin = QPoint(x, y); };
    m_parsers["ReagentHole"] = [this](int x, int y) { m_reagentPin = QPoint(x, y); };
    m_parsers["ThrowCups"] = [this](int x, int y) { m_throwHoleAxis = QPoint(x, y); };
    m_parsers["BloodZone"] = [this](int x, int y) { m_bloodPin = QPoint(x, y); };
}

bool VerifyCoordinates::parseKeyValue(const QString& line, QString& key, QString& value) const
{
    int equalPos = line.indexOf('=');
    if (equalPos == -1) {
        return false;
    }

    key = line.left(equalPos).trimmed();
    value = line.mid(equalPos + 1).trimmed();
    return true;
}

bool VerifyCoordinates::parsePoint(const QString& value, int& x, int& y) const
{
    int commaPos = value.indexOf(',');
    if (commaPos == -1) {
        return false;
    }

    bool xOk, yOk;
    x = value.left(commaPos).trimmed().toInt(&xOk);
    y = value.mid(commaPos + 1).trimmed().toInt(&yOk);

    return xOk && yOk;
}

bool VerifyCoordinates::validateCoordinateRange(int value, int min, int max, const QString& name) const
{
    if (value < min || value > max) {
        qDebug() << QString("Warning: %1 value %2 is out of range [%3, %4]")
                        .arg(name).arg(value).arg(min).arg(max);
        return false;
    }
    return true;
}

bool VerifyCoordinates::parseFixedCoordinate(const QString& key, int x, int y)
{
    if (m_parsers.contains(key)) {
        m_parsers[key](x, y);
        return true;
    }
    return false;
}

bool VerifyCoordinates::parseDynamicCoordinate(const QString& key, int x, int y)
{
    // 解析试剂通道
    if (key.startsWith("ReagentChn_")) {
        bool indexOk;
        int index = key.mid(11).toInt(&indexOk);
        if (indexOk && index >= 0 && index < 12) {
            m_channelOffReagentPin.setChannel(index, x, y);
            return true;
        }
    }
    // 解析抓手通道
    else if (key.startsWith("HandsChn_")) {
        bool indexOk;
        int index = key.mid(9).toInt(&indexOk);
        if (indexOk && index >= 0 && index < 12) {
            m_channels.setChannel(index, x, y);
            return true;
        }
    }
    // 解析托盘抓手
    else if (key.startsWith("TrayHands_")) {
        bool indexOk;
        int index = key.mid(10).toInt(&indexOk);
        if (indexOk && index >= 0 && index < 4) {
            m_handsTaryTube.setTray(index, x, y);
            return true;
        }
    }
    // 解析托盘血针
    else if (key.startsWith("TrayBloodPin_")) {
        bool indexOk;
        int index = key.mid(13).toInt(&indexOk);
        if (indexOk && index >= 0 && index < 4) {
            m_bloodPinTaryTube.setTray(index, x, y);
            return true;
        }
    }

    return false;
}

bool VerifyCoordinates::loadCoordinateVerification(quint8 equipmentType)
{
    QString equipmentName;
    switch (equipmentType) {
        case 0: equipmentName = "KS600"; break;
        case 1: equipmentName = "KS800"; break;
        case 2: equipmentName = "KS1200"; break;
        default:
            QLOG_DEBUG() << "无效设备类型:" << equipmentType;
            equipmentName = "Unknown";
            break;
    }
    return loadCoordinateVerification(equipmentName);
}

bool VerifyCoordinates::loadCoordinateVerification(const QString& equipmentName)
{
    QString configDir = QCoreApplication::applicationDirPath();
    QString fileName = QString("%1coordinateVerification.txt").arg(equipmentName);
    QString filePath = configDir + "/" + fileName;

    QFile file(filePath);
    if (!file.exists()) {
        QLOG_DEBUG() << "坐标验证文件不存在，将创建默认模板:" << filePath;
        if (!createDefaultCoordinateFile(filePath, equipmentName)) {
                QLOG_DEBUG() << "创建默认坐标文件失败";
                return false;
            }
            // 重新打开文件
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QLOG_DEBUG() << "无法打开新创建的坐标文件:" << filePath;
                return false;
            }
    } else {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QLOG_DEBUG() << "无法打开坐标验证文件:" << filePath
                     << "- Error:" << file.errorString();
            return false;
        }
    }

    // 清空现有数据
    m_originAxis = QPoint(0, 0);
    m_throwHoleAxis = QPoint(0, 0);
    m_reagentPin = QPoint(0, 0);
    m_bloodPin = QPoint(0, 0);
    m_cleanZoneBloodPin = QPoint(0, 0);
    m_cleanZoneReagentPin = QPoint(0, 0);
    m_channels.clear();
    m_channelOffReagentPin.clear();
    m_handsTaryTube.clear();
    m_bloodPinTaryTube.clear();

    QTextStream in(&file);
    in.setCodec("UTF-8");

    int lineNumber = 0;
    int parsedCount = 0;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lineNumber++;

        // 跳过空行和注释
        if (line.isEmpty() || line.startsWith("#")) {
            continue;
        }

        QString key, value;
        if (!parseKeyValue(line, key, value)) {
            QLOG_DEBUG() << "警告：第几行格式无效" << lineNumber << ":" << line;
            continue;
        }

        int x, y;
        if (!parsePoint(value, x, y)) {
            QLOG_DEBUG() << "警告：第几行的点格式无效" << lineNumber << ":" << value;
            continue;
        }

        // 验证坐标范围
        validateCoordinateRange(x, DEFAULT_MIN_COORDINATE, DEFAULT_MAX_COORDINATE,
                               QString("%1.X").arg(key));
        validateCoordinateRange(y, DEFAULT_MIN_COORDINATE, DEFAULT_MAX_COORDINATE,
                               QString("%1.Y").arg(key));

        // 解析坐标
        if (parseFixedCoordinate(key, x, y) || parseDynamicCoordinate(key, x, y)) {
            parsedCount++;
        } else {
            QLOG_DEBUG() << "警告：在第几行有未知键" << lineNumber << ":" << key;
        }
    }

    file.close();

    QLOG_DEBUG() << "坐标验证文件加载成功:" << filePath;
    QLOG_DEBUG() << "已解析" << parsedCount << "坐标条目";

    // 验证关键坐标
    if (m_originAxis.isNull()) {
        QLOG_DEBUG() << "警告：坐标文件中未设置 OriginAxis";
    }

    printLoadedCoordinates();

    return true;
}


void VerifyCoordinates::loadMachineAxis()
{
    const auto *pinstance = SingletonAxis::GetInstance();
    if (!pinstance) {
        qDebug() << "Error: Cannot get Axis instance";
        return;
    }

    // 定义常量（需要根据实际项目调整）
    const bool READ_OPERATION = false;  // 请根据实际定义修改


    // 加载原点坐标
    QPoint originAxis(0, 0);
    pinstance->originPos(READ_OPERATION, originAxis);
    m_loadedOriginAxis = originAxis;

    // 加载丢杯孔坐标
    QPoint throwAxis(0, 0);
    pinstance->throwTubeHolePos(READ_OPERATION, throwAxis);
    m_loadedThrowHoleAxis = throwAxis;

    // 加载清洗区坐标
    QPoint cleanOffReagent(0, 0);
    QPoint cleanOffBloodpin(0, 0);
    pinstance->cleanZoneAxisPos(READ_OPERATION, MOTOR_REAGNET_INDEX, cleanOffReagent);
    m_loadedCleanZoneReagentPin = cleanOffReagent;
    pinstance->cleanZoneAxisPos(READ_OPERATION, MOTOR_BLOOD_INDEX, cleanOffBloodpin);
    m_loadedCleanZoneBloodPin = cleanOffBloodpin;

    // 加载试剂区坐标
    QPoint reagentAxis(0, 0);
    pinstance->reagetZoneAxisPos(READ_OPERATION, 0, reagentAxis);
    m_loadedReagentPin = reagentAxis;

    // 加载血样区坐标
    QPoint bloodAxis(0, 0);
    pinstance->bloodSampleZonePos(READ_OPERATION, 0, bloodAxis);
    m_loadedBloodPin = bloodAxis;

    // 加载通道坐标
    for(int n = 0; n < 12; ++n) {
        QPoint channelOffHands(0, 0);
        QPoint channelOffReagent(0, 0);

        pinstance->chnZoneAxisPos(READ_OPERATION, n, MOTOR_HANDS_INDEX, channelOffHands);
        m_loadedChannels.setChannel(n, channelOffHands.x(), channelOffHands.y());

        pinstance->chnZoneAxisPos(READ_OPERATION, n, MOTOR_REAGNET_INDEX, channelOffReagent);
        m_loadedChannelOffReagentPin.setChannel(n, channelOffReagent.x(), channelOffReagent.y());
    }

    // 加载托盘坐标 - 修正计算公式
    for(int i = 0; i < 4; ++i) {
        QPoint trayTubeOffHands(0, 0);
        QPoint trayTubeOffBloodPin(0, 0);

        // 修正：使用更合理的索引计算
        int position = i + i * 59;  // 请根据实际设备调整

        pinstance->testTaryZoneAxisPos(READ_OPERATION, position, MOTOR_HANDS_INDEX, trayTubeOffHands);
        m_loadedHandsTaryTube.setTray(i, trayTubeOffHands.x(), trayTubeOffHands.y());

        pinstance->testTaryZoneAxisPos(READ_OPERATION, position, MOTOR_BLOOD_INDEX, trayTubeOffBloodPin);
        m_loadedBloodPinTaryTube.setTray(i, trayTubeOffBloodPin.x(), trayTubeOffBloodPin.y());
    }

    QLOG_DEBUG() << "Machine axis loaded successfully";
    printLoadedCoordinates();
}

void VerifyCoordinates::printLoadedCoordinates() const
{
    QLOG_DEBUG() << "=== 已加载坐标 ===";
    QLOG_DEBUG() << "原点坐标:" << m_loadedOriginAxis.x() << "," << m_loadedOriginAxis.y();
    QLOG_DEBUG() << "丢杯孔坐标:" << m_loadedThrowHoleAxis.x() << "," << m_loadedThrowHoleAxis.y();
    QLOG_DEBUG() << "试剂针坐标:" << m_loadedReagentPin.x() << "," << m_loadedReagentPin.y();
    QLOG_DEBUG() << "血样针坐标:" << m_loadedBloodPin.x() << "," << m_loadedBloodPin.y();
    QLOG_DEBUG() << "清洗区(血针):" << m_loadedCleanZoneBloodPin.x() << "," << m_loadedCleanZoneBloodPin.y();
    QLOG_DEBUG() << "清洗区(试剂针):" << m_loadedCleanZoneReagentPin.x() << "," << m_loadedCleanZoneReagentPin.y();
    // ========== 通道坐标输出 ==========
    QLOG_DEBUG() << "=== 抓手通道坐标 ===";
    for(int i = 0; i < m_loadedChannels.getChannelCount(); ++i) {
        QPoint point = m_loadedChannels.getChannel(i);
        QLOG_DEBUG() << QString("通道 %1: (%2, %3)").arg(i + 1).arg(point.x()).arg(point.y());
    }

    QLOG_DEBUG() << "=== 试剂针通道坐标 ===";
    for(int i = 0; i < m_loadedChannelOffReagentPin.getChannelCount(); ++i) {
        QPoint point = m_loadedChannelOffReagentPin.getChannel(i);
        QLOG_DEBUG() << QString("通道 %1: (%2, %3)").arg(i + 1).arg(point.x()).arg(point.y());
    }

    // ========== 托盘坐标输出 ==========
    QLOG_DEBUG() << "=== 托盘抓手坐标 ===";
    for(int i = 0; i < m_loadedHandsTaryTube.getTrayCount(); ++i) {
        QPoint point = m_loadedHandsTaryTube.getTray(i);
        QLOG_DEBUG() << QString("托盘 %1: (%2, %3)").arg(i + 1).arg(point.x()).arg(point.y());
    }

    QLOG_DEBUG() << "=== 托盘血针坐标 ===";
    for(int i = 0; i < m_loadedBloodPinTaryTube.getTrayCount(); ++i) {
        QPoint point = m_loadedBloodPinTaryTube.getTray(i);
        QLOG_DEBUG() << QString("托盘 %1: (%2, %3)").arg(i + 1).arg(point.x()).arg(point.y());
    }
    QLOG_DEBUG() << "=========================";
}



bool VerifyCoordinates::verifyCoordinates() const
{
    bool allValid = true;

    auto validatePoint = [&](const QString& name, const QPoint& loaded, const QPoint& expected) {
        int dx = abs(loaded.x() - expected.x());
        int dy = abs(loaded.y() - expected.y());

        if (dx > COORDINATE_TOLERANCE || dy > COORDINATE_TOLERANCE) {
            QLOG_DEBUG() << QString("%1 的坐标不匹配：已加载(%2,%3) 与 预期(%4,%5)，差异(%6,%7)")
                            .arg(name)
                            .arg(loaded.x()).arg(loaded.y())
                            .arg(expected.x()).arg(expected.y())
                            .arg(dx).arg(dy);
            allValid = false;
        }
    };

    validatePoint("原点坐标", m_loadedOriginAxis, m_originAxis);
    validatePoint("丢杯孔坐标", m_loadedThrowHoleAxis, m_throwHoleAxis);
    validatePoint("试剂针坐标", m_loadedReagentPin, m_reagentPin);
    validatePoint("血样针坐标", m_loadedBloodPin, m_bloodPin);
    validatePoint("清洗区(血针)", m_loadedCleanZoneBloodPin, m_cleanZoneBloodPin);
    validatePoint("清洗区(试剂针)", m_loadedCleanZoneReagentPin, m_cleanZoneReagentPin);

    // 验证通道坐标
    for(int i = 0; i < m_channels.getChannelCount(); ++i) {
        QPoint loaded = m_loadedChannels.getChannel(i);
        QPoint expected = m_channels.getChannel(i);
        if (abs(loaded.x() - expected.x()) > COORDINATE_TOLERANCE ||
           abs(loaded.y() - expected.y()) > COORDINATE_TOLERANCE) {
           QLOG_DEBUG() << QString("抓手通道 %1 不匹配：已加载(%2,%3) 与 预期(%4,%5)")
                           .arg(i).arg(loaded.x()).arg(loaded.y())
                           .arg(expected.x()).arg(expected.y());
           allValid = false;
        }
    }

    // 验证试剂针通道坐标
    for(int i = 0; i < m_channelOffReagentPin.getChannelCount(); ++i) {
        QPoint loaded = m_loadedChannelOffReagentPin.getChannel(i);
        QPoint expected = m_channelOffReagentPin.getChannel(i);
        if (abs(loaded.x() - expected.x()) > COORDINATE_TOLERANCE ||
            abs(loaded.y() - expected.y()) > COORDINATE_TOLERANCE) {
            QLOG_DEBUG() << QString("试剂针通道 %1 不匹配：已加载(%2,%3) 与 预期(%4,%5)")
                            .arg(i).arg(loaded.x()).arg(loaded.y())
                            .arg(expected.x()).arg(expected.y());
            allValid = false;
        }
    }

    // 验证托盘抓手坐标
    for(int i = 0; i < m_handsTaryTube.getTrayCount(); ++i) {
        QPoint loaded = m_loadedHandsTaryTube.getTray(i);
        QPoint expected = m_handsTaryTube.getTray(i);
        if (abs(loaded.x() - expected.x()) > COORDINATE_TOLERANCE ||
            abs(loaded.y() - expected.y()) > COORDINATE_TOLERANCE) {
            QLOG_DEBUG() << QString("托盘抓手 %1 不匹配：已加载(%2,%3) 与 预期(%4,%5)")
                            .arg(i).arg(loaded.x()).arg(loaded.y())
                            .arg(expected.x()).arg(expected.y());
            allValid = false;
        }
    }

    // 验证托盘血针坐标
    for(int i = 0; i < m_bloodPinTaryTube.getTrayCount(); ++i) {
        QPoint loaded = m_loadedBloodPinTaryTube.getTray(i);
        QPoint expected = m_bloodPinTaryTube.getTray(i);
        if (abs(loaded.x() - expected.x()) > COORDINATE_TOLERANCE ||
            abs(loaded.y() - expected.y()) > COORDINATE_TOLERANCE) {
            QLOG_DEBUG() << QString("托盘血针 %1 不匹配：已加载(%2,%3) 与 预期(%4,%5)")
                            .arg(i).arg(loaded.x()).arg(loaded.y())
                            .arg(expected.x()).arg(expected.y());
            allValid = false;
        }
    }

    if (allValid) {
        QLOG_DEBUG() << "所有坐标已成功验证!";
    } else {
        emit const_cast<VerifyCoordinates*>(this)->coordinateVerificationFailed("坐标验证失败");
    }

    return allValid;
}

QString VerifyCoordinates::getCoordinateDifferences() const
{
    QString diff;
    QTextStream stream(&diff);
    stream.setCodec("UTF-8");

    auto addDifference = [&](const QString& name, const QPoint& loaded, const QPoint& expected) {
        int dx = loaded.x() - expected.x();
        int dy = loaded.y() - expected.y();
        if (dx != 0 || dy != 0) {
            stream << name << QString::fromUtf8( ": 偏差(" )<< dx << "," << dy << ")\n";
        }
    };

    addDifference(QString::fromUtf8("原点坐标"), m_loadedOriginAxis, m_originAxis);
    addDifference(QString::fromUtf8("丢杯孔坐标"), m_loadedThrowHoleAxis, m_throwHoleAxis);
    addDifference(QString::fromUtf8("试剂针坐标"), m_loadedReagentPin, m_reagentPin);
    addDifference(QString::fromUtf8("血样针坐标"), m_loadedBloodPin, m_bloodPin);
    addDifference(QString::fromUtf8("清洗区血针"), m_loadedCleanZoneBloodPin, m_cleanZoneBloodPin);
    addDifference(QString::fromUtf8("清洗区试剂针"), m_loadedCleanZoneReagentPin, m_cleanZoneReagentPin);

    // ==========通道坐标对比 ==========
   bool hasChannelDiff = false;
   for (int i = 0; i < m_channels.getChannelCount(); ++i) {
       QPoint loaded = m_loadedChannels.getChannel(i);
       QPoint expected = m_channels.getChannel(i);
       int dx = loaded.x() - expected.x();
       int dy = loaded.y() - expected.y();
       if (dx != 0 || dy != 0) {
           if (!hasChannelDiff) {
               stream << "\n" << QString::fromUtf8("========== 通道坐标偏差 ==========\n");
               hasChannelDiff = true;
           }
           stream << QString::fromUtf8("通道[抓手] %1: 偏差(%2, %3)\n")
                     .arg(i + 1).arg(dx).arg(dy);
       }
   }

   // 试剂针通道坐标对比
   bool hasReagentChannelDiff = false;
   for (int i = 0; i < m_channelOffReagentPin.getChannelCount(); ++i) {
       QPoint loaded = m_loadedChannelOffReagentPin.getChannel(i);
       QPoint expected = m_channelOffReagentPin.getChannel(i);
       int dx = loaded.x() - expected.x();
       int dy = loaded.y() - expected.y();
       if (dx != 0 || dy != 0) {
           if (!hasReagentChannelDiff) {
               stream << QString::fromUtf8("========== 通道[试剂针]坐标偏差 ==========\n");
               hasReagentChannelDiff = true;
           }
           stream << QString::fromUtf8("通道 %1: 偏差(%2, %3)\n")
                     .arg(i + 1).arg(dx).arg(dy);
       }
   }

   // ========== 托盘坐标对比 ==========
   bool hasTrayHandsDiff = false;
   for (int i = 0; i < m_handsTaryTube.getTrayCount(); ++i) {
       QPoint loaded = m_loadedHandsTaryTube.getTray(i);
       QPoint expected = m_handsTaryTube.getTray(i);
       int dx = loaded.x() - expected.x();
       int dy = loaded.y() - expected.y();
       if (dx != 0 || dy != 0) {
           if (!hasTrayHandsDiff) {
               stream << "\n" << QString::fromUtf8("========== 托盘[抓手]坐标偏差 ==========\n");
               hasTrayHandsDiff = true;
           }
           stream << QString::fromUtf8("托盘 %1: 偏差(%2, %3)\n")
                     .arg(i + 1).arg(dx).arg(dy);
       }
   }

   bool hasTrayBloodDiff = false;
   for (int i = 0; i < m_bloodPinTaryTube.getTrayCount(); ++i) {
       QPoint loaded = m_loadedBloodPinTaryTube.getTray(i);
       QPoint expected = m_bloodPinTaryTube.getTray(i);
       int dx = loaded.x() - expected.x();
       int dy = loaded.y() - expected.y();
       if (dx != 0 || dy != 0) {
           if (!hasTrayBloodDiff) {
               stream << QString::fromUtf8("========== 托盘[血针]坐标偏差 ==========\n");
               hasTrayBloodDiff = true;
           }
           stream << QString::fromUtf8("托盘 %1: 偏差(%2, %3)\n")
                     .arg(i + 1).arg(dx).arg(dy);
       }
   }

   return diff;
}



//------------------------------对比提示-----------------------------------------

void VerifyCoordinates::startComparingCoordinates(const bool initMachine){

    QLOG_DEBUG() << "=== 开始坐标校验 ===";
    //获取设备类型
    quint8 equipmentType = 0;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, equipmentType);

    //加载坐标校验文件
    VerifyCoordinates* verifyCoord = VerifyCoordinates::GetInstance();
    if (!verifyCoord->loadCoordinateVerification(equipmentType)) {
        QMessageBox::warning(this, "错误",
                            QString("加载坐标校验文件失败！\n设备类型: %1").arg(equipmentType));
        return;
    }

    //加载仪器当前坐标
    verifyCoord->loadMachineAxis();

    //自动进行坐标对比
    compareCoordinates(initMachine);
}






void VerifyCoordinates::compareCoordinates(const bool initMachine)
{
    VerifyCoordinates* verifyCoord = VerifyCoordinates::GetInstance();

    QLOG_DEBUG() << "=== Comparing Coordinates ===";

    // 方法1: 简单验证（返回bool）
    bool isValid = verifyCoord->verifyCoordinates();

    if (isValid) {
        QMessageBox::information(this,
            QString::fromUtf8("验证结果"),
            QString::fromUtf8("所有坐标验证通过！\n校验坐标与仪器坐标一致。"));
    } else {
        displayStyledComparison(initMachine);
    }
}


void VerifyCoordinates::displayStyledComparison(const bool initMachine)
{
    // 1. 防重入检查 - 使用成员变量
    if (!m_comparisonDialog.isNull()) {
        // 如果对话框已存在，将其提升到前台
        m_comparisonDialog->raise();
        m_comparisonDialog->activateWindow();
        QLOG_DEBUG() << "Comparison dialog already exists, raising to front";
        return;
    }


    // 创建样式化对比窗口
    StyledComparisonDialog* dialog = new StyledComparisonDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);  // 关闭时自动删除
    m_comparisonDialog = dialog;  // 保存到成员变量

    // 准备基础坐标数据
    QMap<QString, QPair<QPoint, QPoint>> basicPoints;
    basicPoints["原点坐标"] = qMakePair(m_originAxis, m_loadedOriginAxis);
    basicPoints["丢杯孔坐标"] = qMakePair(m_throwHoleAxis, m_loadedThrowHoleAxis);
    basicPoints["试剂针坐标"] = qMakePair(m_reagentPin, m_loadedReagentPin);
    basicPoints["血样针坐标"] = qMakePair(m_bloodPin, m_loadedBloodPin);
    basicPoints["清洗区(血针)"] = qMakePair(m_cleanZoneBloodPin, m_loadedCleanZoneBloodPin);
    basicPoints["清洗区(试剂针)"] = qMakePair(m_cleanZoneReagentPin, m_loadedCleanZoneReagentPin);

    // 准备抓手通道数据
    QMap<int, QPair<QPoint, QPoint>> handsChannels;
    for(int i = 0; i < m_channels.getChannelCount(); ++i) {
        handsChannels[i] = qMakePair(m_channels.getChannel(i), m_loadedChannels.getChannel(i));
    }

    // 准备试剂针通道数据
    QMap<int, QPair<QPoint, QPoint>> reagentChannels;
    for(int i = 0; i < m_channelOffReagentPin.getChannelCount(); ++i) {
        reagentChannels[i] = qMakePair(m_channelOffReagentPin.getChannel(i),
                                       m_loadedChannelOffReagentPin.getChannel(i));
    }

    // 准备托盘抓手数据
    QMap<int, QPair<QPoint, QPoint>> trayHands;
    for(int i = 0; i < m_handsTaryTube.getTrayCount(); ++i) {
        trayHands[i] = qMakePair(m_handsTaryTube.getTray(i), m_loadedHandsTaryTube.getTray(i));
    }

    // 准备托盘血针数据
    QMap<int, QPair<QPoint, QPoint>> trayBlood;
    for(int i = 0; i < m_bloodPinTaryTube.getTrayCount(); ++i) {
        trayBlood[i] = qMakePair(m_bloodPinTaryTube.getTray(i), m_loadedBloodPinTaryTube.getTray(i));
    }

    // 设置数据
    dialog->setComparisonData(basicPoints, handsChannels, reagentChannels,
                              trayHands, trayBlood);

    // 设置公差
    dialog->setTolerance(COORDINATE_TOLERANCE);

    // 设置校准回调
    // 使用 QPointer 防止悬空指针
    QPointer<VerifyCoordinates> thisPtr(this);
    dialog->setCalibrationCallback([thisPtr, initMachine]() -> bool {
        if (thisPtr.isNull()) {
            QLOG_WARN() << "VerifyCoordinates has been destroyed";
            return false;
        }
        if(initMachine)
            return thisPtr->syncFileToInstrument(); //文件坐标覆盖写到仪器
        else
            return thisPtr->performAxisCalibration();//仪器坐标覆盖写到文件
    });

    // 对话框关闭时清理引用并关闭定时器串口
   // 使用成员变量指针进行清理
   QPointer<StyledComparisonDialog> dialogPtr = dialog;
   connect(dialog, &StyledComparisonDialog::destroyed, this, [this, dialogPtr]() {
       // 清理成员变量（仅当当前关闭的对话框正是保存的那个）
       if (m_comparisonDialog == dialogPtr) {
           m_comparisonDialog.clear();
       }
       QLOG_DEBUG() << "Comparison dialog closed, resources cleaned up";
   });


    // 显示对话框
    dialog->exec();
}



//仪器内坐标同步写入到校准坐标文件
bool VerifyCoordinates::performAxisCalibration()
{
    // 防止重入
    static bool isCalibrating = false;
    if (isCalibrating) {
        QLOG_DEBUG() << "校准正在进行中，请勿重复执行";
        return false;
    }

    isCalibrating = true;

    // 使用 RAII 确保标志位被重置
    struct FlagGuard {
        bool& flag;
        ~FlagGuard() { flag = false; }
    } guard{isCalibrating};


    VerifyCoordinates* verifyCoord = VerifyCoordinates::GetInstance();
    if (!verifyCoord) {
        QLOG_DEBUG() << "VerifyCoordinates 实例为空";
        return false;
    }
    SingletonAxis* axis = SingletonAxis::GetInstance();
    if (!axis) {
       QLOG_DEBUG() << "SingletonAxis 实例为空";
       return false;
   }

    QLOG_DEBUG() << "=== 开始轴坐标校准 ===";

    // 获取设备类型
    quint8 equipmentType = 0;
    axis->equipmentKind(READ_OPERRAT, equipmentType);

    QString equipmentName;
    switch (equipmentType) {
        case 0: equipmentName = "KS600"; break;
        case 1: equipmentName = "KS800"; break;
        case 2: equipmentName = "KS1200"; break;
        default: equipmentName = "Unknown"; break;
    }

    // 配置文件路径
    QString configDir = QCoreApplication::applicationDirPath();
    QString fileName = QString("%1coordinateVerification.txt").arg(equipmentName);
    QString filePath = configDir + "/" + fileName;

    // 备份原文件
    QString backupPath = filePath + ".bak";
    if (QFile::exists(filePath)) {
        QFile::remove(backupPath);
        QFile::copy(filePath, backupPath);
        QLOG_DEBUG() << "已备份原配置文件到:" << backupPath;
    }

    // 打开文件准备写入
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QLOG_DEBUG() << "无法打开配置文件进行写入:" << filePath;
        QMessageBox::warning(this, "校准失败", QString("无法打开配置文件:\n%1").arg(filePath));
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    // 写入文件头
    out << "# Coordinate Initialization Verification\n";
    out << "# Equipment Type: " << equipmentName << "\n";
    out << "# Generated: " << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n";
    out << "# This file is auto-generated by axis calibration\n";
    out << "\n";

    // === 读取仪器当前坐标 ===
    QPoint originAxis(0, 0);
    QPoint throwHoleAxis(0, 0);
    QPoint reagentPin(0, 0);
    QPoint bloodPin(0, 0);
    QPoint cleanZoneBloodPin(0, 0);
    QPoint cleanZoneReagentPin(0, 0);

    axis->originPos(READ_OPERRAT, originAxis);
    axis->throwTubeHolePos(READ_OPERRAT, throwHoleAxis);
    axis->reagetZoneAxisPos(READ_OPERRAT, 0, reagentPin);
    axis->bloodSampleZonePos(READ_OPERRAT, 0, bloodPin);
    axis->cleanZoneAxisPos(READ_OPERRAT, MOTOR_BLOOD_INDEX, cleanZoneBloodPin);
    axis->cleanZoneAxisPos(READ_OPERRAT, MOTOR_REAGNET_INDEX, cleanZoneReagentPin);

    // 写入基础坐标
    out << "# === Basic Coordinates ===\n";
    out << QString("OriginAxis=%1,%2\n").arg(originAxis.x()).arg(originAxis.y());
    out << QString("CleanZone(Blood)=%1,%2\n").arg(cleanZoneBloodPin.x()).arg(cleanZoneBloodPin.y());
    out << QString("CleanZone(Reagent)=%1,%2\n").arg(cleanZoneReagentPin.x()).arg(cleanZoneReagentPin.y());
    out << QString("ReagentHole=%1,%2\n").arg(reagentPin.x()).arg(reagentPin.y());
    out << QString("ThrowCups=%1,%2\n").arg(throwHoleAxis.x()).arg(throwHoleAxis.y());
    out << QString("BloodZone=%1,%2\n").arg(bloodPin.x()).arg(bloodPin.y());
    out << "\n";

    // === 读取并写入试剂针通道坐标 ===
    out << "# === Reagent Pin Offset Channel ===\n";
    int channelCount = 12;
    quint8 totalChn = 0;
    axis->equipmentKind(READ_OPERRAT, totalChn);
    switch (totalChn) {
        case 0: channelCount = 4; break;  // KS600
        case 1: channelCount = 8; break;  // KS800
        case 2: channelCount = 12; break; // KS1200
        default: channelCount = 12; break;
    }

    for (int i = 0; i < channelCount; ++i) {
        QPoint chnPoint(0, 0);
        axis->chnZoneAxisPos(READ_OPERRAT, i, MOTOR_REAGNET_INDEX, chnPoint);
        out << QString("ReagentChn_%1=%2,%3\n").arg(i).arg(chnPoint.x()).arg(chnPoint.y());
    }
    out << "\n";

    // === 读取并写入抓手通道坐标 ===
    out << "# === Hands Offset Channel ===\n";
    for (int i = 0; i < channelCount; ++i) {
        QPoint chnPoint(0, 0);
        axis->chnZoneAxisPos(READ_OPERRAT, i, MOTOR_HANDS_INDEX, chnPoint);
        out << QString("HandsChn_%1=%2,%3\n").arg(i).arg(chnPoint.x()).arg(chnPoint.y());
    }
    out << "\n";

    // === 读取并写入托盘抓手坐标 ===
    out << "# === Tray Tube Offset (Hands) ===\n";
    int trayCount = 4;
    switch (totalChn) {
        case 0: trayCount = 2; break;  // KS600 有2个托盘
        case 1: trayCount = 3; break;  // KS800 有3个托盘
        case 2: trayCount = 4; break;  // KS1200 有4个托盘
        default: trayCount = 4; break;
    }

    for (int i = 0; i < trayCount; ++i) {
        QPoint trayPoint(0, 0);
        int position = i * 60;  // 每个托盘60个孔位
        axis->testTaryZoneAxisPos(READ_OPERRAT, position, MOTOR_HANDS_INDEX, trayPoint);
        out << QString("TrayHands_%1=%2,%3\n").arg(i).arg(trayPoint.x()).arg(trayPoint.y());
    }
    out << "\n";

    // === 读取并写入托盘血针坐标 ===
    out << "# === Tray Tube Offset (BloodPin) ===\n";
    for (int i = 0; i < trayCount; ++i) {
        QPoint trayPoint(0, 0);
        int position = i * 60;
        axis->testTaryZoneAxisPos(READ_OPERRAT, position, MOTOR_BLOOD_INDEX, trayPoint);
        out << QString("TrayBloodPin_%1=%2,%3\n").arg(i).arg(trayPoint.x()).arg(trayPoint.y());
    }

    file.close();

    QLOG_DEBUG() << "坐标校准完成，已保存到:" << filePath;
    QLOG_DEBUG() << QString("保存了 %1 个通道, %2 个托盘").arg(channelCount).arg(trayCount);

    if (verifyCoord) {
       bool loadSuccess = verifyCoord->loadCoordinateVerification(equipmentName);
       if (loadSuccess) {
           QLOG_DEBUG() << "配置文件已重新加载";
       }
   }

    return true;
}


//将文件内坐标同步到仪器
bool VerifyCoordinates::syncFileToInstrument()
{
    VerifyCoordinates* verifyCoord = VerifyCoordinates::GetInstance();
    SingletonAxis* axis = SingletonAxis::GetInstance();

    if (!verifyCoord || !axis) {
        QLOG_DEBUG() << "获取实例失败";
        return false;
    }

    qDebug() << "=== 开始将配置文件坐标同步到仪器 ===";

    // 获取设备类型
    quint8 equipmentType = 0;
    axis->equipmentKind(READ_OPERRAT, equipmentType);

    QString equipmentName;
    switch (equipmentType) {
        case KS600: equipmentName = "KS600"; break;
        case KS800: equipmentName = "KS800"; break;
        case KS1200: equipmentName = "KS1200"; break;
        default: equipmentName = "Unknown"; break;
    }

    // 先加载配置文件
    if (!verifyCoord->loadCoordinateVerification(equipmentName)) {
        QLOG_DEBUG() << "加载配置文件失败";
        QMessageBox::warning(this, "同步失败", "无法加载坐标配置文件！");
        return false;
    }

    //QByteArrayList sendCommands;

    // 1. 同步原点坐标
    QPoint origin = verifyCoord->getOriginAxis();
    axis->operOriginAxis(NOTIFY_XPOINT, origin.x());
    axis->operOriginAxis(NOTIFY_YPOINT, origin.y());
    QLOG_DEBUG() << QString("同步原点坐标: (%1,%2)").arg(origin.x()).arg(origin.y());

    // 2. 同步丢杯孔坐标
    QPoint throwHole = verifyCoord->getThrowHoleAxis();
    axis->oper_ThrowTubeHolePos(NOTIFY_XPOINT, throwHole.x());
    axis->oper_ThrowTubeHolePos(NOTIFY_YPOINT, throwHole.y());
    QLOG_DEBUG() << QString("同步丢杯孔坐标: (%1,%2)").arg(throwHole.x()).arg(throwHole.y());

    // 3. 同步清洗区坐标（血针）
    QPoint cleanZoneBlood = verifyCoord->getCleanZoneOffBloodPin();
    axis->oper_CleanZonePos(NOTIFY_XPOINT, MOTOR_BLOOD_INDEX, cleanZoneBlood.x());
    axis->oper_CleanZonePos(NOTIFY_YPOINT, MOTOR_BLOOD_INDEX, cleanZoneBlood.y());
    QLOG_DEBUG() << QString("同步清洗区(血针)坐标: (%1,%2)").arg(cleanZoneBlood.x()).arg(cleanZoneBlood.y());

    // 4. 同步清洗区坐标（试剂针）
    QPoint cleanZoneReagent = verifyCoord->getCleanZoneOffReagentPin();
    axis->oper_CleanZonePos(NOTIFY_XPOINT, MOTOR_REAGNET_INDEX, cleanZoneReagent.x());
    axis->oper_CleanZonePos(NOTIFY_YPOINT, MOTOR_REAGNET_INDEX, cleanZoneReagent.y());
    QLOG_DEBUG() << QString("同步清洗区(试剂针)坐标: (%1,%2)").arg(cleanZoneReagent.x()).arg(cleanZoneReagent.y());

    // 5. 同步试剂区坐标（所有试剂位使用相同的偏移量）
    QPoint reagentPin = verifyCoord->getReagentHoleAxis();
    QMap<quint8, QPoint> allreagentPoints;
    QUIUtils::CreatReagArsOtherAxis(reagentPin, allreagentPoints);
    for (auto iter = allreagentPoints.begin(); iter != allreagentPoints.end(); ++iter) {
        QPoint reagentHole = iter.value();
        axis->reagetZoneAxisPos(WRITE_OPERAT, iter.key(), reagentHole);
    }
    QLOG_DEBUG() << QString("同步试剂区");


    // 6. 同步血样区坐标
    QMap<quint8,QPoint> alltheBloodHoleAxis;
    QPoint bloodPin = verifyCoord->getBloodModuleOffBloodPin();
    QUIUtils::creatBloodSampleAxis(equipmentType, bloodPin, alltheBloodHoleAxis);
    auto iter = alltheBloodHoleAxis.constBegin();
    while(iter != alltheBloodHoleAxis.constEnd())
    {
       quint8 holeIndex = iter.key();
       QPoint holeAxis = iter.value();
       axis->bloodSampleZonePos(WRITE_OPERAT,holeIndex,holeAxis);
       iter++;
    }


    // 7. 同步抓手通道坐标
    int channelCount = 12;
    switch (equipmentType) {
        case KS600: channelCount = 4; break;
        case KS800: channelCount = 8; break;
        case KS1200: channelCount = 12; break;
        default: channelCount = 12; break;
    }

    for (int i = 0; i < channelCount; ++i) {
        QPoint expected = verifyCoord->getChannelOffHands().getChannel(i);

        // 获取当前坐标并计算偏移
        int offsetX_chn = expected.x() + i * 350;
        int offsetY_chn = expected.y();

        if (offsetX_chn != 0 || offsetY_chn != 0) {
            axis->oper_TestChnZoneAxispos(NOTIFY_XPOINT, i, MOTOR_HANDS_INDEX, offsetX_chn);
            axis->oper_TestChnZoneAxispos(NOTIFY_YPOINT, i, MOTOR_HANDS_INDEX, offsetY_chn);
            QLOG_DEBUG() << QString("同步抓手通道 %1: (%2,%3)").arg(i).arg(offsetX_chn).arg(offsetY_chn);
        }
    }

    // 8. 同步试剂针通道坐标
    for (int i = 0; i < channelCount; ++i) {
        QPoint expected = verifyCoord->getChannelOffReagentPin().getChannel(i);
        int offsetX_chn = expected.x() + i * 350;
        int offsetY_chn = expected.y();
        if (expected.x() != 0 || expected.y() != 0) {
            axis->oper_TestChnZoneAxispos(NOTIFY_XPOINT, i, MOTOR_REAGNET_INDEX, offsetX_chn);
            axis->oper_TestChnZoneAxispos(NOTIFY_YPOINT, i, MOTOR_REAGNET_INDEX, offsetY_chn);
            QLOG_DEBUG() << QString("同步试剂针通道 %1: (%2,%3)").arg(i).arg(offsetX_chn).arg(offsetY_chn);
        }
    }

    // 9. 同步托盘抓手坐标
    constexpr int SPACE_TUBE = 150;     // 内部试管横/纵向间距相同
    constexpr int ROWS_PER_TRAY = 10;   // 每个托盘行数
    constexpr int COLS_PER_TRAY = 6;    // 每个托盘列数

    int trayCount = 4;
    switch (equipmentType) {
        case KS600: trayCount = 2; break;
        case KS800: trayCount = 3; break;
        case KS1200: trayCount = 4; break;
        default: trayCount = 4; break;
    }

    quint8 keyHole = 0;
    for (int i = 0; i < trayCount; ++i) {
        QPoint expected = verifyCoord->getTaryTubeOffHands().getTray(i);
        if (expected.x() != 0 || expected.y() != 0) {

            for (int row = 0; row < ROWS_PER_TRAY; ++row)
            {
                const int yPos = expected.y() + row * SPACE_TUBE;

                for (int col = 0; col < COLS_PER_TRAY; ++col)
                {
                    const QPoint tubePos(expected.x() + col * SPACE_TUBE, yPos);

                    // 传递副本（如果函数参数是非const引用）
                    QPoint pos = tubePos;
                    axis->oper_TestTrayZonaPos(NOTIFY_XPOINT, keyHole, MOTOR_HANDS_INDEX, pos.x());
                    axis->oper_TestTrayZonaPos(NOTIFY_YPOINT, keyHole, MOTOR_HANDS_INDEX, pos.y());
                    ++keyHole;
                }
            }
        }
    }

    // 10. 同步托盘血针坐标
    keyHole = 0;
    for (int i = 0; i < trayCount; ++i) {
        QPoint expected = verifyCoord->getTaryTubeOffBloodPin().getTray(i);
        if (expected.x() != 0 || expected.y() != 0) {

            for (int row = 0; row < ROWS_PER_TRAY; ++row)
            {
                const int yPos = expected.y() + row * SPACE_TUBE;

                for (int col = 0; col < COLS_PER_TRAY; ++col)
                {
                    const QPoint tubePos(expected.x() + col * SPACE_TUBE, yPos);

                    // 传递副本（如果函数参数是非const引用）
                    QPoint pos = tubePos;
                    axis->oper_TestTrayZonaPos(NOTIFY_XPOINT, keyHole, MOTOR_BLOOD_INDEX, pos.x());
                    axis->oper_TestTrayZonaPos(NOTIFY_YPOINT, keyHole, MOTOR_BLOOD_INDEX, pos.y());
                    ++keyHole;
                }
            }
        }
    }

    // 重新加载仪器坐标以验证
    verifyCoord->loadMachineAxis();

    return true;
}


bool VerifyCoordinates::createDefaultCoordinateFile(const QString& filePath, const QString& equipmentName)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QLOG_DEBUG() << "无法创建坐标文件:" << filePath << "-" << file.errorString();
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "# Coordinate Initialization Verification\n";
    out << "# Equipment Type: " << equipmentName << "\n";
    out << "# Generated: " << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n";
    out << "# This file is auto-generated by axis calibration\n";
    out << "\n";
    out << "# === Basic Coordinates ===\n";
    out << "OriginAxis=0,0\n";
    out << "CleanZone(Blood)=0,0\n";
    out << "CleanZone(Reagent)=0,0\n";
    out << "ReagentHole=0,0\n";
    out << "ThrowCups=0,0\n";
    out << "BloodZone=0,0\n";
    out << "\n";
    out << "# === Reagent Pin Offset Channel ===\n";
    for (int i = 0; i <= 11; ++i) {
        // 根据设备类型可以调整X值，这里以KS1200为例
        int x = 864 + (i * 350);  // 大致间距，可根据实际修改
        out << QString("ReagentChn_%1=%2,125\n").arg(i).arg(x);
    }
    out << "\n";
    out << "# === Hands Offset Channel ===\n";
    for (int i = 0; i <= 11; ++i) {
        int x = 869 + (i * 350);
        out << QString("HandsChn_%1=%2,363\n").arg(i).arg(x);
    }
    out << "\n";
    out << "# === Tray Tube Offset (Hands) ===\n";
    QList<int> trayHandsX = {863, 2059, 3257, 4460};
    for (int i = 0; i < trayHandsX.size(); ++i) {
        out << QString("TrayHands_%1=%2,980\n").arg(i).arg(trayHandsX[i]);
    }
    out << "\n";
    out << "# === Tray Tube Offset (BloodPin) ===\n";
    QList<int> trayBloodX = {866, 2075, 3270, 4474};
    for (int i = 0; i < trayBloodX.size(); ++i) {
        out << QString("TrayBloodPin_%1=%2,510\n").arg(i).arg(trayBloodX[i]);
    }

    file.close();
    QLOG_DEBUG() << "已创建默认坐标文件:" << filePath;
    return true;
}
