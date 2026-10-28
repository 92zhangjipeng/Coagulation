#ifndef LISMESSAGE_H
#define LISMESSAGE_H

#include "lisconfig.h"

#include <QJsonObject>
#include <QString>
#include <QStringList>

struct LisOutboundText
{
    QString hl7;
    QStringList astmRecords;
    QString error;
};

class LisMessage
{
public:
    static LisOutboundText build(const LisConfigData &cfg,
                                 const QJsonObject &detail,
                                 const QString &localIp,
                                 bool includeCurveUrl);
};

#endif
