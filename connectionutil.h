#ifndef CONNECTIONUTIL_H
#define CONNECTIONUTIL_H


#pragma once

#include <QObject>
#include <functional>

namespace ConnectionUtil
{
    typedef QMetaObject::Connection Conn;

    class ReceiverObj : public QObject
    {
        Q_OBJECT

    public:
        explicit ReceiverObj(Conn *conn1, Conn *conn2) : mConn1(conn1), mConn2(conn2)  {}

    public slots:
        void slot()
        {
            QObject::disconnect(*mConn1);
            QObject::disconnect(*mConn2);
            delete mConn1;
            delete mConn2;
            deleteLater();
        }

    private:
        Conn *mConn1, *mConn2;
    };

    // 处理信号为SIGNAL(...)的情况
    template <typename Sender, typename ...Args>
    void connectOnce(Sender &&sender, const char *signal, Args &&...args)
    {
        Conn *conn1 = new Conn;
        Conn *conn2 = new Conn;
        *conn1 = QObject::connect(std::forward<Sender>(sender), signal, std::forward<Args>(args)...);
        *conn2 = QObject::connect(std::forward<Sender>(sender), signal, new ReceiverObj(conn1, conn2), SLOT(slot()));
    }

    // 处理其他情况
    template <typename Sender, typename Signal, typename ...Args>
    void connectOnce(Sender &&sender, Signal &&signal, Args &&...args)
    {
        Conn *conn1 = new Conn;
        Conn *conn2 = new Conn;
        *conn1 = QObject::connect(std::forward<Sender>(sender), std::forward<Signal>(signal), std::forward<Args>(args)...);
        *conn2 = QObject::connect(std::forward<Sender>(sender), std::forward<Signal>(signal), std::bind(&ReceiverObj::slot, new ReceiverObj(conn1, conn2)));
    }
}

#endif // CONNECTIONUTIL_H
