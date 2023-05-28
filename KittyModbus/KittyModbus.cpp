#include <modbus/modbus.h>
#include <QApplication>
#include <QDebug>
#include "FS.h"
#include "RegDef.h"
#include "Process.h"
#include "ModbusReader.h"

int main(int argc, char *argv[])
{
    QThreadPool::globalInstance()->setMaxThreadCount(20);
    Config& config = Config::instance();
    QApplication a(argc, argv);

    //kittyLogs::logger::init(kittyLogs::logger::consoleSink);
    kittyLogs::logger::init(kittyLogs::logger::networkSink,
                            config.logs.ip.get(),
                            config.logs.port.get());

    const RegDef& regdef = RegDef::instance();

    ModbusReader modbusReader;
    FS filesystem;
    QObject::connect(&modbusReader, SIGNAL(freshData(SensorsData)),
                     &filesystem, SLOT(onData(SensorsData)));

    QVector<QSharedPointer<Process>> processes;

    MuticastMessageParser input;
    processes.push_back(QSharedPointer<Process>::create());
    QObject::connect(&input, SIGNAL(freshCommand(QSharedPointer<kitty::network::object::ModbusMulticastCommand>)),
                     &*processes.back(), SLOT(onData(QSharedPointer<kitty::network::object::ModbusMulticastCommand>)));
    QObject::connect(&*processes.back(), SIGNAL(start(unsigned, unsigned)),
                             &modbusReader, SLOT(onStart(unsigned, unsigned)));
    QObject::connect(&*processes.back(), SIGNAL(stop(unsigned, unsigned)),
                     &modbusReader, SLOT(onStop(unsigned, unsigned)));

    kitty::network::SimpleUdpServer dataOutputMulticast(QString::fromStdString(config.network.ip.get()),
                                                        config.network.port.get(),
                                                        QString::fromStdString(config.systemConfig.ip.get()));

    QObject::connect(&modbusReader, SIGNAL(sensorsMulticastMSG(QSharedPointer<kitty::network::object::SensorsMulticastMSG>)),
                     &dataOutputMulticast, SLOT(onSensorsMulticastMSG(QSharedPointer<kitty::network::object::SensorsMulticastMSG>)));

    modbusReader.onConnect();
    input.onConnect();
    return a.exec();
}
