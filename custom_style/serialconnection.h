#ifndef SERIALCONNECTION_H
#define SERIALCONNECTION_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QStringList>


class SerialConnection  : public QObject
{
    Q_OBJECT
public:
    explicit SerialConnection(QObject *parent = 0);
    ~SerialConnection();

    enum ConnectionState
    {
        Disconnected = 0,
        Connecting,
        Connected,
        ConnectionFailed
    };

    void findSerialDevices(); //find any tags or anchors that are connected to the PC

    int openSerialPort(QSerialPortInfo x); //open selected serial port

    QStringList portsList(); //return list of available serial ports (list of ports with tag/anchor connected)

    QSerialPort* serialPort() { return _serial; }

signals:
    void clearTags();
    void serialError(void);
    void getCfg(void);
    void nextCmd(void);

    void statusBarMessage(QString status);
    void connectionStateChanged(SerialConnection::ConnectionState);
    void serialOpened(QString, QString);

public slots:
    void closeConnection();
    int  openConnection();
    void readData(void);

protected slots:
    void writeData(const QByteArray &data);
    void handleError(QSerialPort::SerialPortError error);

private:

    QSerialPort *_serial;

    QList<QSerialPortInfo>	_portInfo ;
    QStringList _ports;

    QList<QByteArray> _cmdList ;

    QString _connectionVersion;
    QString _conncectionConfig;
    bool _processingData;
};

#endif // SERIALCONNECTION_H


