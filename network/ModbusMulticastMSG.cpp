#include "ModbusMulticastMSG.h"

namespace kitty{
namespace network{
namespace object{

QDataStream& operator<<(QDataStream& s, const ModbusMulticastMSG& d)
{
    s << d.Magic << d.timestamp << d.regValue;
    return s;
}

QDataStream& operator>>(QDataStream& s, ModbusMulticastMSG& d)
{
    s >> d.timestamp;
    s >> d.regValue;
    return s;
}

}
}
}
