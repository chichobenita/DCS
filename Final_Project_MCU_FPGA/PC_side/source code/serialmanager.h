#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#pragma once

#include <QObject>
#include <QSerialPort>
#include <QTimer>
extern "C" {
#include "radar_protocol.h"
}
#include <QDebug>

class SerialManager : public QObject {
    Q_OBJECT

public:
    explicit SerialManager(QObject *parent = nullptr);
    ~SerialManager();

    QStringList availablePorts() const;
    bool openPort(const QString &portName, qint32 baudRate = 9600);
    void closePort();
    bool isOpen() const;

    void sendCommand(const QByteArray &data);
    void sendBytes(const QByteArray &bytes);

signals:
    void dataReceived(QByteArray data);
    void fullMessageReceived(const radar_message_t& msg);
    void errorOccurred(QString error);
    void connected();
    void disconnected();

private slots:
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);

private:
    QSerialPort *serial;
    QByteArray messageBuffer;
    bool receivingMessage = false;
};

#endif // SERIALMANAGER_H
