#ifndef OPERREGISTRY_H
#define OPERREGISTRY_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include "qstring.h"

#define keyname                     "Suoweiparameter"//创建的注册表名称
#define regPath                     "HKEY_CURRENT_USER\\Software"//创建的注册表路径
#define initial_key                  "0"//默认初始的值

class Operregistry : public QObject
{
    Q_OBJECT
public:
    explicit Operregistry(QObject *parent = 0);

public:
    static Operregistry *getInstance();

public:
    void detection_regrdit();   //注册表检测
    void set_regedit(QString regvalue); //写注册表
    QString get_regedit_numb();     //读注册表值

signals:

public slots:

protected:


private:
    static QAtomicPointer<Operregistry> _instance;
    static QMutex _mutex;
};

#endif // Operregistry
