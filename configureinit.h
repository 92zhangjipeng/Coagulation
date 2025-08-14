#ifndef CONFIGUREINIT_H
#define CONFIGUREINIT_H

#include <QObject>
#include"AnalyticalJson.h"

class ConfigureInit : public QObject
{
    Q_OBJECT
public:
    explicit ConfigureInit(QObject *parent = nullptr);

signals:

public slots:
private slots:
    bool AlarmSoundInitMain(); //初始化警报响否

private:
    AnalyticalJson  m_AnalyJson;
};

#endif // CONFIGUREINIT_H
