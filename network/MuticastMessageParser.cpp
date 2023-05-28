#include "MuticastMessageParser.h"
#include <modbus/modbus.h>
#include <QDebug>
#include <QThread>
#include <QtConcurrent>
#include <chrono>
#include "Config.h"

MuticastMessageParser::MuticastMessageParser() : listener(QSharedPointer<kitty::network::SimpleUdpServer>::create(QString::fromStdString(Config::instance().network.ip.get()),
                                                                                                                  Config::instance().network.port.get(),
                                                                                                                  QString::fromStdString(Config::instance().systemConfig.ip.get())))
{
    qRegisterMetaType<SensorsData>("SensorsData");
    qRegisterMetaType<kitty::network::object::Sun>("kitty::network::object::Sun");
    connect(&*listener, SIGNAL(recvModbusMulticastCommand(QSharedPointer<kitty::network::object::ModbusMulticastCommand>)),
            this, SLOT(onData(QSharedPointer<kitty::network::object::ModbusMulticastCommand>)));

    connect(&*listener, SIGNAL(recvSensorsMulticastMSG(QSharedPointer<kitty::network::object::SensorsMulticastMSG>)),
            this, SLOT(onSensorData(QSharedPointer<kitty::network::object::SensorsMulticastMSG>)));

    connect(&*listener, SIGNAL(recvWeatherMulticastMSG(QSharedPointer<kitty::network::object::WeatherData>)),
            this, SLOT(onWeatherData(QSharedPointer<kitty::network::object::WeatherData>)));

    connect(this, SIGNAL(startListening()), &*listener, SLOT(onStartListeningBoardcast()));
    connect(this, SIGNAL(stopListening()), &*listener, SLOT(onStopListening()));
    listener->moveToThread(&udpThread);
    udpThread.start();

}

void MuticastMessageParser::onDisconnect()
{
    emit stopListening();
}
void MuticastMessageParser::onConnect()
{
    emit startListening();
}

void MuticastMessageParser::onData(QSharedPointer<kitty::network::object::ModbusMulticastCommand> data)
{
    emit freshCommand(data);
}

void MuticastMessageParser::onSensorData(QSharedPointer<kitty::network::object::SensorsMulticastMSG> data)
{
    SensorsData buffer;
    buffer.timestamp = data->timestamp;
    buffer.values = QSharedPointer<QVector<float>>::create(data->regValue);
    buffer.devIndex = data->deviceIndex;

    emit freshData(buffer);
}

void MuticastMessageParser::onWeatherData(QSharedPointer<kitty::network::object::WeatherData> data)
{
    kitty::network::object::Sun values = data->sun;
    emit freshSunData(values);
}
