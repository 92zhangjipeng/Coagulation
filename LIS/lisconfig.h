#ifndef LISCONFIG_H
#define LISCONFIG_H

#include <QString>

struct LisConfigData
{
    bool enabled = false;
    bool pullEnabled = true;
    int pullPort = 8080;
    bool pushEnabled = false;
    QString protocol = "HL7";
    QString host;
    int pushPort = 2575;
    bool sendOnTestComplete = true;
    bool sendOnReview = false;
    QString sendingApp = "KS";
    QString sendingFacility = "SW";
    QString recvApp = "LIS";
    QString recvFacility = "HOSPITAL";
    QString charset = "UTF-8";
    int timeoutSec = 8;
    int retryMax = 5;
};

class LisConfig
{
public:
    static LisConfigData load();
    static void save(const LisConfigData &data);
    static QString localIpv4();
};

#endif
