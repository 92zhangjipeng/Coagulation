#ifndef OBTAINMAINBOARDCOMMDER_H
#define OBTAINMAINBOARDCOMMDER_H

#include <QObject>

class ObtainMainBoardCommder : public QObject
{
    Q_OBJECT
public:
    explicit ObtainMainBoardCommder(QObject *parent = nullptr);

signals:

public slots:
};

#endif // OBTAINMAINBOARDCOMMDER_H