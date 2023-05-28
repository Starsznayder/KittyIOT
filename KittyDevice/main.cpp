#include <modbus/modbus.h>
#include <QApplication>
#include <QDebug>
#include "Config.h"
#include "Triggers.h"
#include "SunTrigger.h"
#include "Process.h"

int main(int argc, char *argv[])
{
    QThreadPool::globalInstance()->setMaxThreadCount(20);
    Config& config = Config::instance();
    QApplication a(argc, argv);

    //kittyLogs::logger::init(kittyLogs::logger::consoleSink);
    kittyLogs::logger::init(kittyLogs::logger::networkSink,
                            config.logs.ip.get(),
                            config.logs.port.get());

    Triggers& triggers = Triggers::instance();

    MuticastMessageParser input;
    QVector<QSharedPointer<Process>> processes;

    kitty::network::SimpleUdpServer dataOutputMulticast(QString::fromStdString(config.network.ip.get()),
                                                        config.network.port.get(),
                                                        QString::fromStdString(config.systemConfig.ip.get()));

    for (int i = 0; i < triggers.triggers.size(); ++i)
    {
        processes.push_back(QSharedPointer<Process>::create(i));
        QObject::connect(&input, SIGNAL(freshData(SensorsData)),
                         &*processes.back(), SLOT(onData(SensorsData)));

        QObject::connect(&*processes.back(), SIGNAL(command(QSharedPointer<kitty::network::object::ModbusMulticastCommand>)),
                         &dataOutputMulticast, SLOT(onModbusMulticastCommand(QSharedPointer<kitty::network::object::ModbusMulticastCommand>)));

        processes.back()->onRun();
    }

    SunTrigger& sunTrigger = SunTrigger::instance();

    for (int i = 0; i < sunTrigger.triggers.size(); ++i)
    {
        processes.push_back(QSharedPointer<Process>::create(sunTrigger.triggers[i]));
        QObject::connect(&input, SIGNAL(freshSunData(kitty::network::object::Sun)),
                         &*processes.back(), SLOT(onData(kitty::network::object::Sun)));

        QObject::connect(&*processes.back(), SIGNAL(command(QSharedPointer<kitty::network::object::ModbusMulticastCommand>)),
                         &dataOutputMulticast, SLOT(onModbusMulticastCommand(QSharedPointer<kitty::network::object::ModbusMulticastCommand>)));
        processes.back()->onRunBySun();
    }
    input.onConnect();

    return a.exec();
}
