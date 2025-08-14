#include "updatecontrolthread.h"
#include "machinesetting.h"

updateControlthread::updateControlthread(QObject *parent): QThread(parent)
{

}

updateControlthread::~updateControlthread() {

}

void updateControlthread::setObj(MachineSetting* obj) {
    guiMain = obj;
}

void updateControlthread::run() {
    while (true) {
        msleep(1);
        guiMain->runInThread();
    }
}


