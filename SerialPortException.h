// SerialPortException.h
#pragma once

#include <stdexcept>
#include <QString>
#include <QSerialPort>

class SerialPortException : public std::runtime_error
{
public:
    // 支持所有常见字符串类型
    explicit SerialPortException(const std::string& message,
                               QSerialPort::SerialPortError error = QSerialPort::NoError);

    explicit SerialPortException(const QString& message,
                               QSerialPort::SerialPortError error = QSerialPort::NoError);

    explicit SerialPortException(const char* message,
                               QSerialPort::SerialPortError error = QSerialPort::NoError);

    explicit SerialPortException(QSerialPort* port);

    virtual ~SerialPortException() throw() = default;

    QSerialPort::SerialPortError error() const;
    QString qWhat() const;
    QString fullDescription() const;

private:
    QSerialPort::SerialPortError m_error;
};
