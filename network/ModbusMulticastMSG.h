#ifndef KITTYNETWORKOBJECTMODBUSMULTICASTMSG_H
#define KITTYNETWORKOBJECTMODBUSMULTICASTMSG_H

#include <QString>
#include <QDataStream>
#include <QByteArray>
#include <QVector>


namespace kitty{
namespace network{
namespace object{

struct ModbusMulticastMSG{
    quint64 timestamp;
    QVector<float> regValue;
    ModbusMulticastMSG(const QVector<float>& t, uint64_t _timestamp) : regValue(t), timestamp(_timestamp) {}
    ModbusMulticastMSG(){};

    friend QDataStream& operator<<(QDataStream& s, const ModbusMulticastMSG& no);
    friend QDataStream& operator>>(QDataStream& s, ModbusMulticastMSG& no);

    static bool isMagic(quint64 magic)
    {
        return magic == Magic;
    }

private:

    static const quint64 Magic = 12345310;
};


}
}
}

#endif // KITTYNETWORKOBJECTMODBUSMULTICASTMSG_H
