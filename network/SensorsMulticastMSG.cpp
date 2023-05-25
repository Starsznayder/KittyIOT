#include "SensorsMulticastMSG.h"

namespace kitty{
namespace network{
namespace object{

QDataStream& operator<<(QDataStream& s, const SensorsMulticastMSG& d)
{
    s << d.Magic << d.timestamp << d.regValue << d.deviceIndex;
    return s;
}

QDataStream& operator>>(QDataStream& s, SensorsMulticastMSG& d)
{
    s >> d.timestamp;
    s >> d.regValue;
    s >> d.deviceIndex;
    return s;
}

}
}
}
