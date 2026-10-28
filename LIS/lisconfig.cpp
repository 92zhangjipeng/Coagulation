#include "lisconfig.h"

#include <QCoreApplication>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QSettings>

static QString iniPath()
{
    return QCoreApplication::applicationDirPath() + "/Config.ini";
}

static QSettings *openIni()
{
    QSettings *ini = new QSettings(iniPath(), QSettings::IniFormat);
    ini->setIniCodec("UTF-8");
    return ini;
}

LisConfigData LisConfig::load()
{
    LisConfigData data;
    QSettings *ini = openIni();
    ini->beginGroup("LIS");
    data.enabled = ini->value("Enabled", false).toBool();
    data.pullEnabled = ini->value("PullEnabled", true).toBool();
    data.pullPort = ini->value("PullPort", 8080).toInt();
    data.pushEnabled = ini->value("PushEnabled", false).toBool();
    data.protocol = ini->value("Protocol", "HL7").toString().trimmed().toUpper();
    data.host = ini->value("Host").toString().trimmed();
    data.pushPort = ini->value("PushPort", data.protocol == "ASTM" ? 5000 : 2575).toInt();
    data.sendOnTestComplete = ini->value("SendOnTestComplete", true).toBool();
    data.sendOnReview = ini->value("SendOnReview", false).toBool();
    data.sendingApp = ini->value("SendingApp", "KS").toString().trimmed();
    data.sendingFacility = ini->value("SendingFacility", "SW").toString().trimmed();
    data.recvApp = ini->value("RecvApp", "LIS").toString().trimmed();
    data.recvFacility = ini->value("RecvFacility", "HOSPITAL").toString().trimmed();
    data.charset = ini->value("Charset", "UTF-8").toString().trimmed();
    data.timeoutSec = ini->value("TimeoutSec", 8).toInt();
    data.retryMax = ini->value("RetryMax", 5).toInt();
    ini->endGroup();
    delete ini;

    if (data.protocol != "ASTM") {
        data.protocol = "HL7";
    }
    if (data.pullPort < 1 || data.pullPort > 65535) {
        data.pullPort = 8080;
    }
    if (data.pushPort < 1 || data.pushPort > 65535) {
        data.pushPort = data.protocol == "ASTM" ? 5000 : 2575;
    }
    if (data.timeoutSec < 2) {
        data.timeoutSec = 2;
    }
    if (data.timeoutSec > 60) {
        data.timeoutSec = 60;
    }
    if (data.retryMax < 1) {
        data.retryMax = 1;
    }
    if (data.retryMax > 20) {
        data.retryMax = 20;
    }
    if (data.sendingApp.isEmpty()) data.sendingApp = "KS";
    if (data.sendingFacility.isEmpty()) data.sendingFacility = "SW";
    if (data.recvApp.isEmpty()) data.recvApp = "LIS";
    if (data.recvFacility.isEmpty()) data.recvFacility = "HOSPITAL";
    if (data.charset.toUpper() != "GBK") {
        data.charset = "UTF-8";
    } else {
        data.charset = "GBK";
    }
    return data;
}

void LisConfig::save(const LisConfigData &data)
{
    QSettings *ini = openIni();
    ini->beginGroup("LIS");
    ini->setValue("Enabled", data.enabled);
    ini->setValue("PullEnabled", data.pullEnabled);
    ini->setValue("PullPort", data.pullPort);
    ini->setValue("PushEnabled", data.pushEnabled);
    ini->setValue("Protocol", data.protocol);
    ini->setValue("Host", data.host);
    ini->setValue("PushPort", data.pushPort);
    ini->setValue("SendOnTestComplete", data.sendOnTestComplete);
    ini->setValue("SendOnReview", data.sendOnReview);
    ini->setValue("SendingApp", data.sendingApp);
    ini->setValue("SendingFacility", data.sendingFacility);
    ini->setValue("RecvApp", data.recvApp);
    ini->setValue("RecvFacility", data.recvFacility);
    ini->setValue("Charset", data.charset);
    ini->setValue("TimeoutSec", data.timeoutSec);
    ini->setValue("RetryMax", data.retryMax);
    ini->endGroup();
    ini->sync();
    delete ini;
}

QString LisConfig::localIpv4()
{
    const QHostAddress localhost(QHostAddress::LocalHost);
    const QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    for (int i = 0; i < addresses.size(); ++i) {
        const QHostAddress &address = addresses.at(i);
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost) {
            return address.toString();
        }
    }
    return QString();
}
