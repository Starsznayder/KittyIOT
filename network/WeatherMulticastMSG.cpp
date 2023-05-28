#include "WeatherMulticastMSG.h"

namespace kitty{
namespace network{
namespace object{

QDataStream& operator<<(QDataStream& s, const Sun& d)
{
    s << d.Magic() << d.riseTimestamp << d.setTimestamp;
    return s;
}

QDataStream& operator>>(QDataStream& s, Sun& d)
{
    s >> d.riseTimestamp;
    s >> d.setTimestamp;
    return s;
}

QDataStream& operator<<(QDataStream& s, const Meta& d)
{
    s << d.Magic() << d.city << d.country << d.timezone << d.lat << d.lon << d.alt;
    return s;
}

QDataStream& operator>>(QDataStream& s, Meta& d)
{
    s >> d.city;
    s >> d.country;
    s >> d.timezone;
    s >> d.lat;
    s >> d.lon;
    s >> d.alt;
    return s;
}


QDataStream& operator<<(QDataStream& s, const WeatherData& d)
{
    s << d.Magic() << d.meta << d.sun;
    return s;
}

QDataStream& operator>>(QDataStream& s, WeatherData& d)
{
    quint64 mBuff;
    s >> mBuff;
    s >> d.meta;
    s >> mBuff;
    s >> d.sun;
    return s;
}

std::ostream& operator<<(std::ostream& stream, Sun& t)
{
    stream << "riseTimestamp=" << t.riseTimestamp / static_cast<uint64_t>(1e6) << std::endl
           << "setTimestamp=" << t.setTimestamp / static_cast<uint64_t>(1e6) << std::endl;
    return stream;
}

std::ostream& operator<<(std::ostream& stream, Meta& t)
{
    stream << "city=" << t.city.toStdString() << std::endl
           << "country=" << t.country.toStdString() << std::endl
           << "timezone=" << t.timezone.toStdString() << std::endl
           << "lat=" << t.lat << std::endl
           << "lon=" << t.lon << std::endl
           << "alt=" << t.alt << std::endl;
    return stream;
}

std::ostream& operator<<(std::ostream& stream, WeatherData& t)
{
    stream << "[Meta]" << std::endl << t.meta << std::endl
           << "[Sun]" << std::endl << t.sun << std::endl;
    return stream;
}

}
}
}
