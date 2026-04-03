#include "verifycoordinates.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFile>

VerifyCoordinates* VerifyCoordinates::GetInstance()
{
    static VerifyCoordinates instance;
    return &instance;
}


VerifyCoordinates::VerifyCoordinates(QWidget *parent)
    : QWidget(parent)
    , m_originAxis(0, 0)
    , m_throwHoleAxis(0, 0)
    , m_reagentPin(0, 0)
    , m_bloodPin(0, 0)
//    , m_channels(0)
//    , m_channelOffReagentPin(0)
//    , m_handsTaryTube(0)
//    , m_bloodPinTaryTube(0)
    , m_cleanZoneBloodPin(0, 0)
    , m_cleanZoneReagentPin(0, 0)
{
    qDebug() << "VerifyCoordinates initialized";
}

bool VerifyCoordinates::loadCoordinateVerification(quint8 equipmentType)
{
    QString equipmentName;
    switch (equipmentType) {
        case 0: equipmentName = "KS600"; break;
        case 1: equipmentName = "KS800"; break;
        case 2: equipmentName = "KS1200"; break;
        default: equipmentName = "Unknown"; break;
    }
    return loadCoordinateVerification(equipmentName);
}


bool VerifyCoordinates::loadCoordinateVerification(const QString& equipmentName)
{
    QString configDir = QCoreApplication::applicationDirPath();
    QString fileName = QString("%1coordinateVerification.txt").arg(equipmentName);
    QString filePath = configDir + "/" + fileName;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件:" << filePath;
        return false;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if (line.isEmpty() || line.startsWith("#")) {
            continue;
        }

        int equalPos = line.indexOf('=');
        if (equalPos == -1) {
            continue;
        }

        QString key = line.left(equalPos).trimmed();
        QString value = line.mid(equalPos + 1).trimmed();

        int commaPos = value.indexOf(',');
        if (commaPos == -1) {
            continue;
        }

        bool xOk, yOk;
        int x = value.left(commaPos).toInt(&xOk);
        int y = value.mid(commaPos + 1).toInt(&yOk);

        if (!xOk || !yOk) {
            continue;
        }

        if (key == "OriginAxis") {
            m_originAxis.set(x, y);
        } else if (key == "CleanZone(Blood)") {
            m_cleanZoneBloodPin.set(x, y);
        } else if (key == "CleanZone(Reagent)") {
            m_cleanZoneReagentPin.set(x, y);
        } else if (key == "ReagentHole") {
			m_reagentPin.set(x, y);
        }else if (key == "ThrowCups") {
            m_throwHoleAxis.set(x, y);
        } else if (key == "BloodZone") {
			m_bloodPin.set(x, y);
        }else if (key.startsWith("ReagentChn_")) {
            bool indexOk;
            int index = key.mid(11).toInt(&indexOk);
            if (indexOk && index >= 0 && index < 12) {
                m_channelOffReagentPin.setChannel(index, x, y);
            }
        } else if (key.startsWith("HandsChn_")) {
            bool indexOk;
            int index = key.mid(9).toInt(&indexOk);
            if (indexOk && index >= 0 && index < 12) {
                m_channels.setChannel(index, x, y);
            }
        } else if (key.startsWith("TrayHands_")) {
            bool indexOk;
            int index = key.mid(10).toInt(&indexOk);
            if (indexOk && index >= 0 && index < 4) {
                m_handsTaryTube.setTray(index, x, y);
            }
        } else if (key.startsWith("TrayBloodPin_")) {
            bool indexOk;
            int index = key.mid(14).toInt(&indexOk);
            if (indexOk && index >= 0 && index < 4) {
                m_bloodPinTaryTube.setTray(index, x, y);
            }
        }
    }

    file.close();
    qDebug() << "成功加载坐标校验文件:" << filePath;
    return true;
}
