#ifndef FILTER_H
#define FILTER_H

#include <QVector>

namespace filters{

enum class FilterType
{
    MEDIAN,
    MEAN,
    MAX,
    MIN
};

class Filter
{
public:
    Filter(int _order) : order(std::max(_order, 1)) {}

    virtual float execute(const float value)
    {
        return value;
    }

    virtual bool isCompatible(int, FilterType)
    {
        return false;
    }

protected:
    int order;
    QVector<float> circBuffer;
};

class Median : public Filter
{
public:
    Median(int _order) : Filter(_order) {}
    virtual float execute(const float value);
    virtual bool isCompatible(int _order, FilterType _type)
    {
        if (_type == FilterType::MEDIAN && order == _order)
        {
            return true;
        }
        return false;
    }
};

class Mean : public Filter
{
public:
    Mean(int _order) : Filter(_order)  {}
    virtual float execute(const float value);
    virtual bool isCompatible(int _order, FilterType _type)
    {
        if (_type == FilterType::MEAN && order == _order)
        {
            return true;
        }
        return false;
    }
};

class Max : public Filter
{
public:
    Max(int _order) : Filter(_order)  {}
    virtual float execute(const float value);
    virtual bool isCompatible(int _order, FilterType _type)
    {
        if (_type == FilterType::MAX && order == _order)
        {
            return true;
        }
        return false;
    }
};

class Min : public Filter
{
public:
    Min(int _order) : Filter(_order)  {}
    virtual float execute(const float value);
    virtual bool isCompatible(int _order, FilterType _type)
    {
        if (_type == FilterType::MIN && order == _order)
        {
            return true;
        }
        return false;
    }
};

}

#endif // FILTER_H
