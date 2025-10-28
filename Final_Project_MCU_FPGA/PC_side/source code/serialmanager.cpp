#include "serialmanager.h"
#include <QSerialPortInfo>

SerialManager::SerialManager(QObject *parent) : QObject(parent) {
    serial = new QSerialPort(this);
    connect(serial, &QSerialPort::readyRead, this, &SerialManager::handleReadyRead);
    connect(serial, &QSerialPort::errorOccurred, this, &SerialManager::handleError);
}

SerialManager::~SerialManager() {
    closePort();
}

QStringList SerialManager::availablePorts() const {
    QStringList list;
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts())
        list << info.portName();
    return list;
}

bool SerialManager::openPort(const QString &portName, qint32 baudRate) {
    if (serial->isOpen()) serial->close();

    serial->setPortName(portName);
    serial->setBaudRate(baudRate);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (!serial->open(QIODevice::ReadWrite)) {
        emit errorOccurred("Failed to open port");
        return false;
    }

    emit connected();
    return true;
}

void SerialManager::closePort() {
    if (serial->isOpen()) {
        serial->close();
        emit disconnected();
    }
}

bool SerialManager::isOpen() const {
    return serial->isOpen();
}

void SerialManager::sendCommand(const QByteArray &data) {
    if (serial->isOpen())
        serial->write(data);
}

void SerialManager::sendBytes(const QByteArray &bytes) {
    if (serial->isOpen())
        serial->write(bytes);
}

void SerialManager::handleReadyRead() {

    QByteArray bytes = serial->readAll();
    //qDebug()<<bytes.toHex(' ');
    /*for (char byte : bytes) {
        if (byte == '@') {
            messageBuffer.clear();
            receivingMessage = true;
        }

        if (receivingMessage) {
            messageBuffer.append(byte);
            if (byte == '#') {
                receivingMessage = false;
                emit fullMessageReceived(messageBuffer);
                messageBuffer.clear();
            }
        }
    }*/
    radar_message_t msg{};
    for (unsigned char b : bytes)
    {
        int rc = radar_parse_char(&msg, b);
        if (rc == 1) {
            // הודעה שלמה: אפשר לפענח לפי command_id
            //handleRadarMessage(msg);
            emit fullMessageReceived(msg);
        }
    }
}

void SerialManager::handleError(QSerialPort::SerialPortError error) {
    if (error != QSerialPort::NoError)
        emit errorOccurred(serial->errorString());
}
