#ifndef MULTICASTMESSAGEPARSR_H
#define MULTICASTMESSAGEPARSR_H

#include <QObject>
#include <QSharedPointer>
#include <QVector>
#include <QThread>
#include <atomic>
#include "SimpleUdpServer.h"

struct SensorsData
{
    uint64_t timestamp;
    QSharedPointer<QVector<float>> values;
    int devIndex;
};

class MuticastMessageParser : public QObject
{
Q_OBJECT
public:
    MuticastMessageParser();

    ~MuticastMessageParser()
    {
        onDisconnect();
    }

signals:
    void freshData(SensorsData values);
    void startListening();
    void stopListening();

public slots:
    void onData(QSharedPointer<kitty::network::object::ModbusMulticastMSG> data);
    void onSensorData(QSharedPointer<kitty::network::object::SensorsMulticastMSG> data);
    void onDisconnect();
    void onConnect();

private:
    QSharedPointer<kitty::network::SimpleUdpServer> listener;
    QThread udpThread;
};

#endif // MODBUSREADER_H
