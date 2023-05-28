#ifndef KITTYNETWORKOBJECTMODBUSMULTICASTMSG_H
#define KITTYNETWORKOBJECTMODBUSMULTICASTMSG_H

#include <QString>
#include <QDataStream>
#include <QByteArray>
#include <QVector>


namespace kitty{
namespace network{
namespace object{

struct ModbusMulticastCommand
{
    quint32 devIdx;
    quint32 regAddr;
    quint32 regValue;
    ModbusMulticastCommand(quint32 dev, quint32 addr, quint32 val) : devIdx(dev), regAddr(addr), regValue(val) {}
    ModbusMulticastCommand(){};

    friend QDataStream& operator<<(QDataStream& s, const ModbusMulticastCommand& no);
    friend QDataStream& operator>>(QDataStream& s, ModbusMulticastCommand& no);

    static bool isMagic(quint64 magic)
    {
        return magic == Magic;
    }

    bool operator==(const ModbusMulticastCommand &dc)
    {
        return dc.devIdx == devIdx &&
               dc.regAddr == regAddr;
    }

    bool operator!=(const ModbusMulticastCommand &dc)
    {
        return !(*this == dc);
    }

private:
    static const quint64 Magic = 9234531081;
};


}
}
}

#endif // KITTYNETWORKOBJECTMODBUSMULTICASTMSG_H
