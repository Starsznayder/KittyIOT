#include "MultiregOperator.h"
#include <QDebug>

float operators::Sum::execute(SensorsData values)
{
    float t = 0;
    for (unsigned i = 0; i < registers.size(); ++i)
    {
        if (registers[i] < values.values->size())
        {
            t += (*values.values)[registers[i]];
        }
    }
    return t;
}

float operators::Average::execute(SensorsData values)
{
    float t = 0;
    for (int i = 0; i < registers.size(); ++i)
    {
        if (registers[i] < values.values->size())
        {
            t += (*values.values)[registers[i]];
        }
    }
    return t / static_cast<float>(registers.size());
}
