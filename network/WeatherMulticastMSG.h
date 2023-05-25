#ifndef KITTYNETWORKOBJECTWEATHERMULTICASTMSG_H
#define KITTYNETWORKOBJECTWEATHERMULTICASTMSG_H

#include <QString>
#include <QDataStream>
#include <QByteArray>
#include <QVector>

namespace kitty{
namespace network{
namespace object{

struct Sun
{
    quint64 riseTimestamp;
    quint64 setTimestamp;

    friend QDataStream& operator<<(QDataStream& s, const Sun& no);
    friend QDataStream& operator>>(QDataStream& s, Sun& no);

    static quint64 Magic()
    {
        return 82345310135;
    }

    static bool isMagic(quint64 magic)
    {
        return magic == Magic();
    }
};

struct Meta
{
    QString city;
    QString country;
    QString timezone;
    qreal lat;
    qreal lon;
    qreal alt;

    static quint64 Magic()
    {
        return 82325350135;
    }

    static bool isMagic(quint64 magic)
    {
        return magic == Magic();
    }

    friend QDataStream& operator<<(QDataStream& s, const Meta& no);
    friend QDataStream& operator>>(QDataStream& s, Meta& no);
};

struct WeatherData
{
    Meta meta;
    Sun sun;

    static quint64 Magic()
    {
        return 85350135784;
    }

    static bool isMagic(quint64 magic)
    {
        return magic == Magic();
    }

    friend QDataStream& operator<<(QDataStream& s, const WeatherData& no);
    friend QDataStream& operator>>(QDataStream& s, WeatherData& no);
};

std::ostream& operator<<(std::ostream&, Sun&);
std::ostream& operator<<(std::ostream&, Meta&);
std::ostream& operator<<(std::ostream&, WeatherData&);

}
}
}

#endif // KITTYNETWORKOBJECTSENSORSMULTICASTMSG_H
