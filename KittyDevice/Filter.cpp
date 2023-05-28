#include "Filter.h"
#include <algorithm>
#include <numeric>

float filters::Median::execute(const float value)
{
    circBuffer.push_back(value);
    while (circBuffer.size() > order)
    {
        circBuffer.remove(0);
    }
    QVector<float> tmp(circBuffer);
    std::sort(tmp.begin(), tmp.end());

    return tmp[(tmp.size() - 1) / 2];
}

float filters::Min::execute(const float value)
{
    circBuffer.push_back(value);
    while (circBuffer.size() > order)
    {
        circBuffer.remove(0);
    }
    QVector<float> tmp(circBuffer);
    std::sort(tmp.begin(), tmp.end());

    return tmp[0];
}

float filters::Max::execute(const float value)
{
    circBuffer.push_back(value);
    while (circBuffer.size() > order)
    {
        circBuffer.remove(0);
    }
    QVector<float> tmp(circBuffer);
    std::sort(tmp.begin(), tmp.end());

    return tmp.back();
}

float filters::Mean::execute(const float value)
{
    circBuffer.push_back(value);
    while (circBuffer.size() > order)
    {
        circBuffer.remove(0);
    }

    return std::accumulate(circBuffer.begin(), circBuffer.end(), 0.0f) / static_cast<float>(order);
}
