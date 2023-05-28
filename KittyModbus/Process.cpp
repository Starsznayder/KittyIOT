#include "Process.h"
#include "Config.h"
#include <stdlib.h>
#include <kittyLogs/log.h>

Process::Process() {}

void Process::onData(QSharedPointer<kitty::network::object::ModbusMulticastCommand> values)
{
    Config& config = Config::instance();

    bool gotItAlready = false;
    for (int i = 0; i < stateMemory.size(); ++i)
    {
        if (stateMemory[i] == *values)
        {
            if (stateMemory[i].regValue != values->regValue)
            {
                stateMemory[i].regValue = values->regValue;
                if (stateMemory[i].regValue > 0)
                {
                    emit start(static_cast<unsigned>(stateMemory[i].devIdx), static_cast<unsigned>(stateMemory[i].regAddr));
                }
                else
                {
                    emit stop(static_cast<unsigned>(stateMemory[i].devIdx), static_cast<unsigned>(stateMemory[i].regAddr));
                }
            }
            gotItAlready = true;
        }
    }
    if (!gotItAlready && values->devIdx < config.modbusConfig.size())
    {
        stateMemory.push_back(*values);
        if (values->regValue > 0)
        {
            emit start(static_cast<unsigned>(values->devIdx), static_cast<unsigned>(values->regAddr));
        }
        else
        {
            emit stop(static_cast<unsigned>(values->devIdx), static_cast<unsigned>(values->regAddr));
        }
    }
}
