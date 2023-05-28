#include <QApplication>
#include <QDebug>
#include "RegDef.h"
#include "DataBufer.h"
#include "SimpleUdpServer.h"


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


    std::string ip = config.systemConfig.ip.get();
    unsigned port = config.viewer.port.get();
    unsigned timeout = config.viewer.timeout.get();

    qDebug() << QString::fromStdString(ip);
    qDebug() << port;
    qDebug() << timeout;

    MuticastMessageParser input;
    DataBufer dataBufer(ip, port, timeout);
    QObject::connect(&input, SIGNAL(freshData(SensorsData)),
                     &dataBufer, SLOT(onData(SensorsData)));

    input.onConnect();
    return a.exec();
}
