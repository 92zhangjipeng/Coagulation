#ifndef UPDATECONTROLTHREAD_H
#define UPDATECONTROLTHREAD_H

#include <QThread>

class MachineSetting;

#pragma once
class updateControlthread : public QThread
{
public:
    updateControlthread(QObject *parent);
    ~updateControlthread();

    void setObj(MachineSetting* obj);
protected:
    void run();
private:
    MachineSetting* guiMain = nullptr;
};









#endif // UPDATECONTROLTHREAD_H
