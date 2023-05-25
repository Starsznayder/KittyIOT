#include "kittyweather.h"
#include "WeatherJSONParser.h"
#include "FS.h"
#include "Config.h"
#include <QCoreApplication>
#include <QThreadPool>
#include "SimpleUdpServer.h"


int main(int argc, char *argv[])
{
    QThreadPool::globalInstance()->setMaxThreadCount(20);
    QCoreApplication a(argc, argv);
    Config& config = Config::instance();
    kittyLogs::logger::init(kittyLogs::logger::consoleSink);
    kittyLogs::logger::init(kittyLogs::logger::networkSink,
                            config.logs.ip.get(),
                            config.logs.port.get());

    WeatherJSONParser parser;
    kitty::Downloader downloader;
    FS fio;

    qRegisterMetaType<QJsonObject>("QJsonObject");
    qRegisterMetaType<QSharedPointer<kitty::network::object::WeatherData>>("QSharedPointer<kitty::network::object::WeatherData>");

    QObject::connect(&downloader, SIGNAL(jsonObject(QJsonObject)), &parser, SLOT(onJsonObject(QJsonObject)));
    QObject::connect(&parser, SIGNAL(data(QSharedPointer<kitty::network::object::WeatherData>)),
                     &fio, SLOT(onData(QSharedPointer<kitty::network::object::WeatherData>)));

    kitty::network::SimpleUdpServer dataOutputMulticast(QString::fromStdString(config.network.ip.get()),
                                                        config.network.port.get(),
                                                        QString::fromStdString(config.systemConfig.ip.get()));

    QObject::connect(&parser, SIGNAL(data(QSharedPointer<kitty::network::object::WeatherData>)),
                     &dataOutputMulticast, SLOT(onWeatherMulticastMSG(QSharedPointer<kitty::network::object::WeatherData>)));

    downloader.startStatusUpdateThread();

    return a.exec();
}
