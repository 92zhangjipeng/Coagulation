#include "SerialPortException.h"

SerialPortException::SerialPortException(const std::string& message, 
                                       QSerialPort::SerialPortError error)
    : std::runtime_error(message), m_error(error) {}

SerialPortException::SerialPortException(const QString& message,
                                       QSerialPort::SerialPortError error)
    : std::runtime_error(message.toStdString()), m_error(error) {}

SerialPortException::SerialPortException(const char* message,
                                       QSerialPort::SerialPortError error)
    : std::runtime_error(message), m_error(error) {}

SerialPortException::SerialPortException(QSerialPort* port)
    : std::runtime_error(port ? port->errorString().toStdString() : "Null serial port"),
      m_error(port ? port->error() : QSerialPort::NoError) {}

QSerialPort::SerialPortError SerialPortException::error() const {
    return m_error;
}

QString SerialPortException::qWhat() const {
    return QString(what());
}

QString SerialPortException::fullDescription() const {
    return QString("[Error %1] %2").arg(m_error).arg(what());
}