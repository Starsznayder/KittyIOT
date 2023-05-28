#include "ModbusMulticastMSG.h"

namespace kitty{
namespace network{
namespace object{

QDataStream& operator<<(QDataStream& s, const ModbusMulticastCommand& d)
{
    s << d.Magic << d.devIdx << d.regAddr << d.regValue;
    return s;
}

QDataStream& operator>>(QDataStream& s, ModbusMulticastCommand& d)
{
    s >> d.devIdx;
    s >> d.regAddr;
    s >> d.regValue;
    return s;
}

}
}
}
