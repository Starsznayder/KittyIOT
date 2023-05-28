#include <modbus/modbus.h>
#include <QApplication>
#include <QDebug>
#include "FS.h"
#include "Config.h"
#include "SimpleUdpServer.h"
#include "SensorReader.h"

int main(int argc, char *argv[])
{
    QThreadPool::globalInstance()->setMaxThreadCount(20);
    Config& config = Config::instance();
    QApplication a(argc, argv);

    //kittyLogs::logger::init(kittyLogs::logger::consoleSink);
    kittyLogs::logger::init(kittyLogs::logger::networkSink,
                            config.logs.ip.get(),
                            config.logs.port.get());

    SensorReader sensorReader;
    FS filesystem;
    QObject::connect(&sensorReader, SIGNAL(freshData(SensorsData)),
                     &filesystem, SLOT(onData(SensorsData)));

    kitty::network::SimpleUdpServer dataOutputMulticast(QString::fromStdString(config.network.ip.get()),
                                                        config.network.port.get(),
                                                        QString::fromStdString(config.systemConfig.ip.get()));

    QObject::connect(&sensorReader, SIGNAL(modbusMulticastMSG(QSharedPointer<kitty::network::object::ModbusMulticastMSG>)),
                     &dataOutputMulticast, SLOT(onModbusMulticastMSG(QSharedPointer<kitty::network::object::ModbusMulticastMSG>)));
    QObject::connect(&sensorReader, SIGNAL(sensorsMulticastMSG(QSharedPointer<kitty::network::object::SensorsMulticastMSG>)),
                     &dataOutputMulticast, SLOT(onSensorsMulticastMSG(QSharedPointer<kitty::network::object::SensorsMulticastMSG>)));

    sensorReader.onConnect();
    return a.exec();
}
