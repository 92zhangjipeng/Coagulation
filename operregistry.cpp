#include "operregistry.h"

#include <QSettings>

QAtomicPointer<Operregistry> Operregistry::_instance = 0;
QMutex Operregistry::_mutex;

Operregistry::Operregistry(QObject *parent) : QObject(parent)
{

}

Operregistry * Operregistry::getInstance()
{
#ifndef Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE
    if(!QAtomicPointer::isTestAndSetNative())//运行时检测
        qDebug() << "Error: TestAndSetNative not supported!";
#endif
    //使用双重检测。
    /*! testAndSetOrders操作保证在原子操作前和后的的内存访问
     * 不会被重新排序。
     */
    if(_instance.testAndSetOrdered(0, 0))//第一次检测
    {
        QMutexLocker locker(&_mutex);//加互斥锁。

        _instance.testAndSetOrdered(0, new Operregistry);//第二次检测。
    }
    return _instance;
}

void Operregistry::detection_regrdit()
{
    bool have_key = false;
    QSettings *programReg = new QSettings(regPath,QSettings::NativeFormat);//文件路径存在则打开，不存在则创建
    QStringList keys = programReg->allKeys();//读取目录下所有键值
    for(int i  = 0 ; i < keys.size();i++)
    {
        //检测注册表键是否存在，不存在就创建
        QString id = keys.at(i);
        if(id.contains(keyname))
        {
            have_key = true;
            break;
        }
    }
    if(have_key==false)//如果不存在，创建
    {
        set_regedit(initial_key);//写注册表
    }
    delete programReg;


    QString regvalue = get_regedit_numb();//读取注册表值

    set_regedit(regvalue);//写注册表

}
//写注册表
void Operregistry::set_regedit(QString regvalue)
{
    QSettings *writeReg = new QSettings(regPath,QSettings::NativeFormat);//文件路径存在则打开，不存在则创建
    writeReg->setValue(keyname, regvalue);//写注册表 使用平台最合适的存储格式设置
    delete writeReg;
}
//读注册表值
QString Operregistry::get_regedit_numb()
{
    QSettings *ReadReg = new QSettings(regPath,QSettings::NativeFormat);//文件路径存在则打开，不存在则创建
    QString value = ReadReg->value(keyname).toString();//读注册表
    delete ReadReg;

    return value;

}
