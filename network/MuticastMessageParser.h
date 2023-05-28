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
    void freshCommand(QSharedPointer<kitty::network::object::ModbusMulticastCommand> values);
    void freshSunData(kitty::network::object::Sun values);
    void startListening();
    void stopListening();

public slots:
    void onData(QSharedPointer<kitty::network::object::ModbusMulticastCommand> data);
    void onSensorData(QSharedPointer<kitty::network::object::SensorsMulticastMSG> data);
    void onWeatherData(QSharedPointer<kitty::network::object::WeatherData> data);
    void onDisconnect();
    void onConnect();

private:
    QSharedPointer<kitty::network::SimpleUdpServer> listener;
    QThread udpThread;
};

#endif // MODBUSREADER_H
