#ifndef WEATHERJSONPARSER_H
#define WEATHERJSONPARSER_H

#include <QObject>
#include <QSharedPointer>
#include <QFuture>
#include <QThread>
#include <atomic>
#include <mutex>
#include "Downloader.h"
#include "WeatherMulticastMSG.h"

class WeatherJSONParser : public QObject
{
    Q_OBJECT
public:
    WeatherJSONParser();

    ~WeatherJSONParser()
    {
        finishFlag.store(true);
        emitterThreadObject.waitForFinished();
    }

signals:
    void data(QSharedPointer<kitty::network::object::WeatherData>);

public slots:
    void onJsonObject(QJsonObject o);

private:
    QSharedPointer<kitty::network::object::WeatherData> buffer;
    std::mutex proctor;
    std::atomic<bool> finishFlag;
    QFuture<void> emitterThreadObject;

    void worker();
};

#endif // WEATHERJSONPARSER_H
