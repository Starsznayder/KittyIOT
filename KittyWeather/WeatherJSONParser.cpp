#include "WeatherJSONParser.h"
#include <QtConcurrent>
#include <kittyLogs/log.h>


WeatherJSONParser::WeatherJSONParser()
{
    finishFlag.store(false);
    emitterThreadObject = QtConcurrent::run(this, &WeatherJSONParser::worker);
}

void WeatherJSONParser::worker()
{
    while(!finishFlag.load())
    {
        proctor.lock();
        if(buffer)
        {
            emit data(buffer);
        }
        proctor.unlock();
        QThread::sleep(60);
    }
}

void WeatherJSONParser::onJsonObject(QJsonObject o)
{
    proctor.lock();
    buffer = QSharedPointer<kitty::network::object::WeatherData>::create();

    QJsonObject city = o["city"].toObject();

    buffer->meta.city = city.value("name").toString();
    buffer->meta.country = city.value("country").toString();
    buffer->meta.timezone = city.value("timezone").toString();
    buffer->meta.lat = city.value("coord").toObject().value("lat").toDouble();
    buffer->meta.lon = city.value("coord").toObject().value("lon").toDouble();

    buffer->sun.riseTimestamp = city.value("sunrise").toInt();
    buffer->sun.setTimestamp = city.value("sunset").toInt();
    buffer->sun.riseTimestamp *= 1e6;
    buffer->sun.setTimestamp *= 1e6;
    proctor.unlock();

    //emit data(out);
}
