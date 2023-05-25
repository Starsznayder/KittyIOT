#include "WeatherJSONParser.h"


WeatherJSONParser::WeatherJSONParser()
{

}

void WeatherJSONParser::onJsonObject(QJsonObject o)
{
    QSharedPointer<kitty::network::object::WeatherData> out =
            QSharedPointer<kitty::network::object::WeatherData>::create();

    QJsonObject city = o["city"].toObject();

    out->meta.city = city.value("name").toString();
    out->meta.country = city.value("country").toString();
    out->meta.timezone = city.value("timezone").toString();
    out->meta.lat = city.value("coord").toObject().value("lat").toDouble();
    out->meta.lon = city.value("coord").toObject().value("lon").toDouble();

    out->sun.riseTimestamp = city.value("sunrise").toInt();
    out->sun.setTimestamp = city.value("sunset").toInt();
    out->sun.riseTimestamp *= 1e6;
    out->sun.setTimestamp *= 1e6;
    emit data(out);
}
