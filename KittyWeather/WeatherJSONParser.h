#ifndef WEATHERJSONPARSER_H
#define WEATHERJSONPARSER_H

#include <QObject>
#include <QSharedPointer>
#include "Downloader.h"
#include "WeatherMulticastMSG.h"

class WeatherJSONParser : public QObject
{
    Q_OBJECT
public:
    WeatherJSONParser();
signals:
    void data(QSharedPointer<kitty::network::object::WeatherData>);

public slots:
    void onJsonObject(QJsonObject o);

private:

};

#endif // WEATHERJSONPARSER_H
