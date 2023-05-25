#ifndef KITTYNETWORKOBJECTSENSORSMULTICASTMSG_H
#define KITTYNETWORKOBJECTSENSORSMULTICASTMSG_H

#include <QString>
#include <QDataStream>
#include <QByteArray>
#include <QVector>


namespace kitty{
namespace network{
namespace object{

struct SensorsMulticastMSG{   
    quint64 timestamp;
    QVector<float> regValue;
    quint32 deviceIndex;
    SensorsMulticastMSG(const QVector<float>& t, uint64_t _timestamp, int _deviceIndex) : regValue(t), timestamp(_timestamp), deviceIndex(_deviceIndex) {}
    SensorsMulticastMSG(){};

    friend QDataStream& operator<<(QDataStream& s, const SensorsMulticastMSG& no);
    friend QDataStream& operator>>(QDataStream& s, SensorsMulticastMSG& no);

    static bool isMagic(quint64 magic)
    {
        return magic == Magic;
    }

private:

    static const quint64 Magic = 82345310123;
};


}
}
}

#endif // KITTYNETWORKOBJECTSENSORSMULTICASTMSG_H
