#ifndef DUMMY_H
#define DUMMY_H

#endif // DUMMY_H


#include <QtCore/QCoreApplication>
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QDebug>

class Dummy:public QObject
{
    Q_OBJECT
public:
    Dummy(QObject* parent=0):QObject(parent)     {}
public slots:
    void emitsig()
    {
        emit sig();
    }
signals:
    void sig();
};
